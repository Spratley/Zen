#pragma once

#include "../../Component/Zen_ComponentInfo.h"
#include "../../Component/Zen_ComponentRegistry.h"
#include "../../Utils/Zen_TypeListUtils.h"
#include "../../Utils/Zen_TypeUtils.h"
#include "../../Zen_Types.h"

#include <array>

namespace Zen
{
    // TODO: Figure out a mechanism to ensure the component list is in the same order as the root registry so we don't
    // get two archetypes generated for the same components in different orders

    // TODO: I think Archetype should be a private construct of something...
    // You can't manually instantiate an Archetype
    struct Archetype
    {
        friend class ArchetypeFactory;

        constexpr ComponentInfo const& GetComponentInfo(SizeT p_componentIndex) const
        {
            return m_getComponentInfo(p_componentIndex);
        }

        template <typename Component>
        constexpr SizeT GetLocalComponentIndex() const
        {
            return m_localTypeIndexer.GetIndex<Component>();
        }

    private:
        template <typename... Components>
        constexpr Archetype(TypeList<Components...> p_componentTypes)
            : m_getComponentInfo(&GetComponentInfoImpl<Components...>)
            , m_localTypeIndexer(p_componentTypes)
        {}

        template <typename... Components>
        static constexpr ComponentInfo const& GetComponentInfoImpl(SizeT p_componentIndex)
        {
            constexpr std::array<ComponentInfo, sizeof...(Components)> componentInfos = {
                ComponentInfo::GetInfo<Components>()...
            };
            return componentInfos[p_componentIndex];
        }

    private:
        ComponentInfo const& (*m_getComponentInfo)(SizeT p_componentIndex);
        TypeUtils::TypeIndexer m_localTypeIndexer;
    };

    class ArchetypeFactory
    {
    public:
        template <typename... Components>
        static constexpr Archetype MakeArchetype(ComponentRegistry const& p_componentRegistry)
        {
            using SortedComponents = TypeListUtils::SortTypes_T<, Components...>;
            return Archetype(SortedComponents{});
        }

    private:
    };
} // namespace Zen