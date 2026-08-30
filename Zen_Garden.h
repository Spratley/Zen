#pragma once

#include "Component/Zen_ComponentRegistry.h"
#include "Entity/Archetype/Zen_ArchetypeArena.h"
#include "Entity/Zen_Entity.h"
#include "Entity/Zen_EntityView.h"
#include "System/Zen_SystemRegistry.h"
#include "Utils/Zen_TypeListUtils.h"
#include "Zen_Types.h"

namespace Zen
{
    // TODO: Validate that all systems operate on components that are registered in the component registry

    class Garden
    {
    public:
        template <typename... ComponentTypes, typename... SystemTypes>
        constexpr Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>);
        void Initialize(SizeT p_arenaSizeBytes) { m_archetypeArena.Initialize(p_arenaSizeBytes); }

        void Tick() { m_systemRegistry.Execute(m_archetypeArena); }

        template <typename... Components>
        constexpr Entity Spawn();
        template <typename... Components>
        constexpr Entity Spawn(Components&&... p_components);

        // This should live somewhere else
        template <typename Component>
        Component* GetComponent(Entity const& p_entity)
        {
            return m_archetypeArena.GetComponent<Component>(m_entities[static_cast<SizeT>(p_entity.GetID())]);
        }

        template <typename... Components>
        EntityView<Components...> ViewComponents() const
        {
            return EntityView<Components...>(m_archetypeArena);
        }

    private:
        EntityProxyStorage m_entities;
        ComponentRegistry m_componentRegistry;
        SystemRegistry m_systemRegistry;
        ArchetypeArena m_archetypeArena;
    };

    template <typename... ComponentTypes, typename... SystemTypes>
    constexpr Garden::Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>)
        : m_componentRegistry(
            TypeListUtils::SortTypes_T<TypeListUtils::CompareECSPacking,
                                       TypeListUtils::FilterDuplicates_T<TypeList<ComponentTypes...>>>{})
        , m_systemRegistry(TypeListUtils::FilterDuplicates_T<TypeList<SystemTypes...>>{})
        , m_archetypeArena()
    {}

    template <typename... Components>
    constexpr Entity Garden::Spawn()
    {
        return Spawn(Components{}...);
    }

    template <typename... Components>
    constexpr Entity Garden::Spawn(Components&&... p_components)
    {
        EntityKey entityKey = m_archetypeArena.Spawn<Components...>(std::forward<Components>(p_components)...);
        m_entities.push_back(entityKey);
        return Entity(Entity::ID(m_entities.size() - 1), this);
    }

    // TODO: This is here so that it knows about m_garden->GetComponent() instead of in the actual Entity code
    // It NEEDS to be relocated
    template <typename Component>
    Component* Entity::GetComponent() const
    {
        return m_garden->GetComponent<Component>(*this);
    }
} // namespace Zen