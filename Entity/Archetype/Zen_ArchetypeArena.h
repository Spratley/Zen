#pragma once

#include "../../Utils/Zen_DebugUtils.h"
#include "../../Utils/Zen_MemoryUtils.h"
#include "../../Zen_Types.h"
#include "../Zen_Entity.h"
#include "Zen_Archetype.h"
#include "Zen_ArchetypeStorage.h"

#include <type_traits>

// I'd like to come back to this and split up with single responsibility in mind
// Right now ArchetypeArena allocates, distributes, and constructs
// Really that should be three distinct classes that work in harmony

namespace Zen
{
    // Double ended arena buffer
    // Front (Lower address) end stores the actual data segments
    // Back (Higher address) end stores the metadata for each segment, reverse order as they appear
    // [A_Block, B_Block, C_Block, ....., C, B, A]

    class ArchetypeArena
    {
    public:
        ArchetypeArena() = default;
        ArchetypeArena(ArchetypeArena&) = delete;
        ArchetypeArena(ArchetypeArena&&) = delete;

        void Initialize(SizeT p_arenaCapacityBytes);

        SizeT ArchetypeCount() const { return m_archetypeCount; }

        template <typename... Components>
        EntityKey Spawn(Components&&... p_components);

        void Destroy(EntityKey p_entity);

        ArchetypeStorage const* GetArchetypeStorage(SizeT p_index) const;

        template <typename Component>
        Component* GetComponent(EntityKey p_entityKey)
        {
            ArchetypeStorage* archetypeStorage = GetArchetypeStorage(p_entityKey.m_archetypeIndex);
            if (!archetypeStorage || p_entityKey.m_index >= archetypeStorage->m_count)
            {
                return nullptr;
            }

            return &archetypeStorage->GetBuffer<Component>()[p_entityKey.m_index];
        }

    private:
        ArchetypeStorage* GetArchetypeStorage(SizeT p_index);

        SizeT FindArchetypeStorage(U64 p_archetypeSignature) const;

        template <typename... Components>
        void ConstructEntity(EntityKey p_entity, Components&&... p_components);
        EntityKey PushEntity(U64 p_archetypeSignature);

        template <typename Component>
        std::remove_cvref_t<Component>* ConstructComponent(ArchetypeStorage const& p_archetypeStorage,
                                                           SizeT p_entityIndex,
                                                           Component&& p_component);

        template <typename... Components>
        void EnsureArchetype();

        template <typename... Components>
        ArchetypeStorage& PushArchetype();

        bool AllocateArchetypeSize(SizeT p_archetypeIndex, SizeT p_entityCount);

        bool ValidateBufferOverrun(SizeT p_lowerBound) const;

    private:
        SizeT m_archetypeCount = 0;
        SizeT m_usedSizeBytes = 0;
        Byte* m_archetypeBufferBegin = nullptr;
        ArchetypeStorage* m_archetypeStorageBuffer = nullptr;
    };

    template <typename... Components>
    EntityKey ArchetypeArena::Spawn(Components&&... p_components)
    {
        EnsureArchetype<std::remove_cvref_t<Components>...>();
        EntityKey entity = PushEntity(ArchetypeFactory::GenerateSignature<std::remove_cvref_t<Components>...>());
        ConstructEntity<Components...>(entity, std::forward<Components>(p_components)...);
        return entity;
    }

    template <typename... Components>
    void ArchetypeArena::ConstructEntity(EntityKey p_entity, Components&&... p_components)
    {
        ArchetypeStorage const& archetypeStorage = *GetArchetypeStorage(p_entity.m_archetypeIndex);

        ZEN_ASSERT(sizeof...(Components) == archetypeStorage.m_archetype.GetComponentCount(),
                   "ConstructEntity() called with an incorrect number of components!");

        (ConstructComponent<Components>(archetypeStorage, p_entity.m_index, std::forward<Components>(p_components)),
         ...);
    }

    template <typename Component>
    std::remove_cvref_t<Component>* ArchetypeArena::ConstructComponent(ArchetypeStorage const& p_archetypeStorage,
                                                                       SizeT p_entityIndex,
                                                                       Component&& p_component)
    {
        using ComponentType = std::remove_cvref_t<Component>;
        ComponentType* buffer = p_archetypeStorage.GetBuffer<ComponentType>();
        return MemoryUtils::PlacementNew<ComponentType>(buffer + p_entityIndex, std::forward<Component>(p_component));
    }

    template <typename... Components>
    void ArchetypeArena::EnsureArchetype()
    {
        if (FindArchetypeStorage(ArchetypeFactory::GenerateSignature<Components...>()) == SizeT_Max)
        {
            PushArchetype<Components...>();
        }
    }

    template <typename... Components>
    ArchetypeStorage& ArchetypeArena::PushArchetype()
    {
        m_archetypeCount++;
        ArchetypeStorage* archetypeStorage =
          MemoryUtils::PlacementNew<ArchetypeStorage>(GetArchetypeStorage(m_archetypeCount - 1),
                                                      TypeList<Components...>{});

        // I don't like this branch here, but PushArchetype shouldn't be called often enough to care
        if (m_archetypeCount == 1)
        {
            archetypeStorage->m_begin = m_archetypeBufferBegin;
        }
        else
        {
            ArchetypeStorage* lastArchetypeStorage = GetArchetypeStorage(m_archetypeCount - 2);
            archetypeStorage->m_begin = lastArchetypeStorage->m_end;
        }
        archetypeStorage->m_end = archetypeStorage->m_begin;

        return *archetypeStorage;
    }

} // namespace Zen