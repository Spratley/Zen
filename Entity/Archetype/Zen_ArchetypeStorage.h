#pragma once

#include "../../Utils/Zen_TupleUtils.h"
#include "../../Zen_Types.h"
#include "Zen_Archetype.h"

namespace Zen
{
    struct ArchetypeStorage
    {
        Archetype m_archetype;
        SizeT m_capacity;
        SizeT m_count;
        void* buffer;
    };

    // Warning! This assumes the ArchetypeStorage won't add or remove entities during iteration!
    // This is a SAFE assumption for Zen, since all spawn and destroy calls are deferred to the end of the frame
    // Just be aware that if for some reason this invariant is changed, this WILL cause problems
    template <typename... ViewedComponents>
    struct ArchetypeView
    {
        static constexpr SizeT ComponentCount = sizeof...(ViewedComponents);

        struct Iterator
        {
            constexpr Iterator(void* const* p_componentBuffers, SizeT p_index = 0)
                : m_componentBuffers()
                , m_index(p_index)
            {
                for (U32 i = 0; i < ComponentCount; ++i)
                {
                    m_componentBuffers[i] = p_componentBuffers[i];
                }
            }

            constexpr Tuple<ViewedComponents&...> operator*() const
            {
                return Tuple<ViewedComponents&...>{ (GetBuffer<ViewedComponents>()[m_index], ...) };
            }

            constexpr Iterator& operator++()
            {
                ++m_index;
                return *this;
            }

            friend constexpr bool operator!=(Iterator const& p_lhs, Iterator const& p_rhs)
            {
                return p_lhs.m_componentBuffers[0] != p_rhs.m_componentBuffers[0] && p_lhs.m_index != p_rhs.m_index;
            }

            template <typename Component>
            constexpr Component* GetBuffer() const
            {
                return static_cast<Component*>(
                  m_componentBuffers[TupleUtils::IndexOf_V<Component, Tuple<ViewedComponents...>>]);
            }

        private:
            void* m_componentBuffers[ComponentCount];
            SizeT m_index = 0;
        };

        constexpr ArchetypeView(ArchetypeStorage const& p_archetypeStorage)
            : m_componentBuffers()
            , m_entityCount(p_archetypeStorage.m_count)
        {
            // 
        }

        constexpr Iterator begin() const { return Iterator(m_componentBuffers); }
        constexpr Iterator end() const { return Iterator(m_componentBuffers, m_entityCount); }

    private:
        void* m_componentBuffers[ComponentCount];
        SizeT const m_entityCount;
    };
} // namespace Zen