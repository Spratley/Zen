#pragma once

#include "../../Utils/Zen_TypeListUtils.h"
#include "../../Zen_Types.h"
#include "../Zen_Entity.h"
#include "Zen_Archetype.h"
#include "Zen_ArchetypeStorage.h"

namespace Zen
{
    // I can't remember why I split this into two classes?
    // I'll try recombining them once Zen is working

    class ArchetypeArena
    {
    public:
        constexpr SizeT ArchetypeCount() const { return m_archetypeCount; }

        constexpr Entity Spawn(Archetype const& /*p_archetype*/)
        {
            // Step 1: Get Archetype Storage index from Archetype
            SizeT const archetypeIndex = 0;

            // Step 2: Get Archetype Storage by index
            ArchetypeStorage& storage = GetArchetypeStorage(archetypeIndex);

            // Step 3: Check if Archetype Storage needs to grow (Count + 1 >= Capacity)
            if (storage.m_count + 1 >= storage.m_capacity)
            {
                // Step 3.1: Double size of Archety Storage to increase capacity
                SizeT const newCapacity = storage.m_capacity * 2;
            }

            // Step 4: Append new entity to Archetype Storage
            

            // Step 5: Return Entity object

            return Entity();
        }

    protected:
        constexpr ArchetypeStorage const& GetArchetypeStorage(SizeT /*p_index*/) const
        {
            // TODO: Fill
            return *reinterpret_cast<ArchetypeStorage*>(0);
        }

        constexpr ArchetypeStorage& GetArchetypeStorage(SizeT p_index)
        {
            return const_cast<ArchetypeStorage&>(const_cast<ArchetypeArena const*>(this)->GetArchetypeStorage(p_index));
        }

    protected:
        alignas(16) SizeT m_archetypeCount = 0;
    };

    template <SizeT ArenaSizeBytes>
    class SizedArchetypeArena : public ArchetypeArena
    {
    public:
        consteval SizedArchetypeArena() = default;

    private:
        alignas(16) Byte m_arena[ArenaSizeBytes] = {};
    };

    template <typename Components>
    struct ArchetypeArenaView;

    template <typename... Components>
    struct ArchetypeArenaView<TypeList<Components...>>
    {
        struct Iterator
        {
            constexpr Iterator(ArchetypeArena& p_source, SizeT p_archetypeIndex = 0)
                : m_source(p_source)
                , m_archetypeIndex(p_archetypeIndex)
            {}

            friend constexpr bool operator!=(Iterator const& p_lhs, Iterator const& p_rhs)
            {
                return &p_lhs.m_source != &p_rhs.m_source && p_lhs.m_archetypeIndex != p_rhs.m_archetypeIndex;
            }

            constexpr Iterator& operator++()
            {
                m_archetypeIndex++;
                return *this;
            }

            constexpr ArchetypeStorage const& operator*() { return m_source.GetArchetypeStorage(m_archetypeIndex); }

        private:
            ArchetypeArena& m_source;
            SizeT m_archetypeIndex;
        };

        constexpr ArchetypeArenaView(ArchetypeArena& p_source)
            : m_source(p_source)
        {}

        constexpr Iterator begin() const { return Iterator(m_source); }
        constexpr Iterator end() const { return Iterator(m_source, m_source.ArchetypeCount()); }

        ArchetypeArena& m_source;
    };
} // namespace Zen