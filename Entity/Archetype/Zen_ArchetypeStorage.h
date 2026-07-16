#pragma once

#include "../../Component/Zen_ComponentInfo.h"
#include "../../Utils/Zen_LoopUtils.h"
#include "../../Utils/Zen_TypeListUtils.h"
#include "../../Zen_Types.h"
#include "Zen_Archetype.h"

#include <tuple>

namespace Zen
{
    struct ArchetypeStorage
    {
        ArchetypeStorage() = delete;

        template <typename Component>
        constexpr Component* GetBuffer() const
        {
            SizeT bufferOffset = 0;
            SizeT const componentIndex = m_archetype.GetLocalComponentIndex<Component>();
            for (auto i : LoopUtils::CountTo(componentIndex))
            {
                bufferOffset += m_archetype.GetComponentInfo(i).m_size * m_capacity;
            }
            return reinterpret_cast<Component*>(reinterpret_cast<SizeT>(m_buffer) + bufferOffset);
        }

        constexpr void* GetEnd() const
        {
            return reinterpret_cast<void*>(reinterpret_cast<SizeT>(m_buffer) + (m_count * m_archetype.GetStride()));
        }

        Archetype m_archetype;
        SizeT m_capacity;
        SizeT m_count;
        void* m_buffer;
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
            constexpr Iterator(ArchetypeView const& p_view, SizeT p_index = 0);

            // Should this return a Zen type?
            constexpr std::tuple<ViewedComponents&...> operator*() const;
            constexpr Iterator& operator++();
            friend constexpr bool operator!=(Iterator const& /*p_lhs*/, Iterator const& /*p_rhs*/)
            {
                return false;

                // This has to be in the class because it's a dependant type
                // return p_lhs.m_view != p_rhs.m_view && p_lhs.m_index != p_rhs.m_index;
            }

            template <typename Component>
            constexpr Component* GetBuffer() const;

        private:
            ArchetypeView const& m_view;
            SizeT m_index = 0;
        };

        constexpr ArchetypeView(ArchetypeStorage const& p_archetypeStorage)
            : m_componentBuffers{ static_cast<void*>(p_archetypeStorage.GetBuffer<ViewedComponents>())... }
            , m_entityCount(p_archetypeStorage.m_count)
        {}

        constexpr Iterator begin() const { return Iterator(*this); }
        constexpr Iterator end() const { return Iterator(*this, m_entityCount); }

    private:
        void* m_componentBuffers[ComponentCount];
        SizeT const m_entityCount;
    };

    // -=-=-=-= Iterator =-=-=-=-
    template <typename... ViewedComponents>
    constexpr ArchetypeView<ViewedComponents...>::Iterator::Iterator(ArchetypeView const& p_view, SizeT p_index)
        : m_view(p_view)
        , m_index(p_index)
    {}

    template <typename... ViewedComponents>
    constexpr inline std::tuple<ViewedComponents&...> ArchetypeView<ViewedComponents...>::Iterator::operator*() const
    {
        return std::tie<ViewedComponents&...>(GetBuffer<ViewedComponents>()[m_index]...);
    }

    template <typename... ViewedComponents>
    constexpr inline ArchetypeView<ViewedComponents...>::Iterator&
      ArchetypeView<ViewedComponents...>::Iterator::operator++()
    {
        ++m_index;
        return *this;
    }

    template <typename... ViewedComponents>
    template <typename Component>
    constexpr inline Component* ArchetypeView<ViewedComponents...>::Iterator::GetBuffer() const
    {
        return static_cast<Component*>(
          m_view.m_componentBuffers[TypeListUtils::IndexOf_V<Component, TypeList<ViewedComponents...>>]);
    }

} // namespace Zen