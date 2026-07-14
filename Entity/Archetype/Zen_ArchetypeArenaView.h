#pragma once

#include "../../Utils/Zen_TypeListUtils.h"
#include "../../Zen_Types.h"
#include "Zen_ArchetypeArena.h"
#include "Zen_ArchetypeStorage.h"

namespace Zen
{
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
                SizeT const archetypeCount = m_source.ArchetypeCount();
                while (true)
                {
                    if (++m_archetypeIndex >= archetypeCount)
                    {
                        m_archetypeIndex = archetypeCount;
                        break;
                    }

                    ArchetypeStorage const& storage = *(*this);
                    if (storage.m_archetype.Contains<Components...>())
                    {
                        break;
                    }
                }
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