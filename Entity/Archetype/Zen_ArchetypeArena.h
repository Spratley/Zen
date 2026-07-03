#pragma once

#include "../../Zen_Types.h"

namespace Zen
{
    class ArchetypeArena
    {
    public:
        constexpr SizeT ArchetypeCount() const { return m_archetypeCount; }

        constexpr ArchetypeStorage const& GetArchetypeStorage(SizeT p_index) {}

        constexpr Entity Spawn(Archetype const& p_archetype)
        {
            
        }

    private:
        SizeT m_archetypeCount = 0;
        // Manually pad to 16 bytes to make sure the array of the child class is properly aligned
        const SizeT dummyPadding = 0xDEADBEEFDEADBEEF;
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
    struct ArchetypeArenaView<Tuple<Components...>>
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

            constexpr ArchetypeStorage& operator*() { return m_source.GetArchetypeStorage(m_archetypeIndex); }

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