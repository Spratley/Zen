#pragma once

#include "../Utils/Zen_ConceptUtils.h"
#include "../Utils/Zen_TypeListUtils.h"
#include "../Zen_Types.h"
#include "Archetype/Zen_ArchetypeArena.h"
#include "Archetype/Zen_ArchetypeStorage.h"

#include <tuple>

namespace Zen
{
    template <typename... Components>
    requires(!ConceptUtils::IsIndirectType<Components> && ...)
    struct EntityView
    {
        static constexpr SizeT ComponentCount = sizeof...(Components);
        struct Iterator
        {
            static constexpr Iterator GetEndIterator(ArchetypeArena const& p_source)
            {
                Iterator iterator(p_source);
                iterator.m_archetypeIndex = p_source.ArchetypeCount();
                return iterator;
            }

            constexpr Iterator(ArchetypeArena const& p_source)
                : m_source(p_source)
                , m_archetypeIndex(0)
                , m_entityIndex(0)
            {
                if (!ValidateArchetypeComponents())
                {
                    // Increment so we start with the first valid archetype
                    TryIncrementArchetype();
                }
                else
                {
                    // TODO: This is duplicate code, encapsulate!
                    ArchetypeStorage const* activeStorage = m_source.GetArchetypeStorage(m_archetypeIndex);
                    m_activeEntityCount = activeStorage->m_count;
                    ((m_activeComponentBuffers[IndexOf<Components>()] =
                        static_cast<void*>(activeStorage->GetBuffer<Components>())),
                     ...);
                }
            }

            friend constexpr bool operator!=(Iterator const& p_lhs, Iterator const& p_rhs)
            {
                return &p_lhs.m_source != &p_rhs.m_source || p_lhs.m_archetypeIndex != p_rhs.m_archetypeIndex
                       || p_lhs.m_entityIndex != p_rhs.m_entityIndex;
            }

            template <typename Component>
            constexpr SizeT IndexOf() const
            {
                return TypeListUtils::IndexOf_V<Component, TypeList<Components...>>;
            }

            template <typename Component>
            constexpr Component* GetBuffer() const
            {
                return static_cast<Component*>(m_activeComponentBuffers[IndexOf<Component>()]);
            }

            constexpr std::tuple<Components&...> operator*() const
            requires(sizeof...(Components) > 1)
            {
                return std::tie<Components&...>(GetBuffer<Components>()[m_entityIndex]...);
            }

            constexpr TypeListUtils::GetFirst_T<TypeList<Components...>>& operator*() const
            requires(sizeof...(Components) == 1)
            {
                return GetBuffer<TypeListUtils::GetFirst_T<TypeList<Components...>>>()[m_entityIndex];
            }

            constexpr bool ValidateArchetypeComponents() const
            {
                ArchetypeStorage const* storage = m_source.GetArchetypeStorage(m_archetypeIndex);
                return storage->m_archetype.Contains<Components...>();
            }

            constexpr Iterator& operator++()
            {
                if (!TryIncrementEntity())
                {
                    TryIncrementArchetype();
                }
                return *this;
            }

            constexpr bool TryIncrementEntity()
            {
                ++m_entityIndex;
                return m_entityIndex < m_activeEntityCount;
            }

            constexpr bool TryIncrementArchetype()
            {
                m_entityIndex = 0;
                SizeT const archetypeCount = m_source.ArchetypeCount();
                while (true)
                {
                    if (++m_archetypeIndex >= archetypeCount)
                    {
                        m_archetypeIndex = archetypeCount;
                        return false;
                    }

                    if (ValidateArchetypeComponents())
                    {
                        break;
                    }
                }
                ArchetypeStorage const* activeStorage = m_source.GetArchetypeStorage(m_archetypeIndex);
                m_activeEntityCount = activeStorage->m_count;

                ((m_activeComponentBuffers[IndexOf<Components>()] =
                    static_cast<void*>(activeStorage->GetBuffer<Components>())),
                 ...);
                return true;
            }

        private:
            ArchetypeArena const& m_source;
            void* m_activeComponentBuffers[ComponentCount] = {};
            SizeT m_archetypeIndex = 0;
            SizeT m_entityIndex = 0;
            SizeT m_activeEntityCount = 0;
        };

        constexpr EntityView(ArchetypeArena const& p_source)
            : m_source(p_source)
        {}

        constexpr SizeT CountU() const
        {
            SizeT count = 0;
            for (SizeT archetypeIndex = 0, endIndex = m_source.ArchetypeCount(); archetypeIndex < endIndex;
                 ++archetypeIndex)

            {
                ArchetypeStorage const* storage = m_source.GetArchetypeStorage(archetypeIndex);

                if (!storage->m_archetype.Contains<Components...>())
                {
                    continue;
                }
                count += storage->m_count;
            }
            return count;
        }

        constexpr Iterator begin() const { return Iterator(m_source); }
        constexpr Iterator end() const { return Iterator::GetEndIterator(m_source); }

        ArchetypeArena const& m_source;
    };
} // namespace Zen
