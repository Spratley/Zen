#pragma once

#include "../../Component/Zen_ComponentInfo.h"
#include "../../Utils/Zen_TupleUtils.h"

namespace Zen
{
    struct Archetype
    {
        template <typename... Components>
        constexpr Archetype(Tuple<Components...>)
            : m_getComponentInfo(&GetComponentInfoImpl<Components...>)
        {}

        constexpr ComponentInfo GetComponentInfo(SizeT p_componentIndex) const
        {
            return m_getComponentInfo(p_componentIndex);
        }

    private:
        template <typename... Components>
        static constexpr ComponentInfo GetComponentInfoImpl(SizeT p_componentIndex)
        {
            constexpr std::array<ComponentInfo, sizeof...(Components)> componentInfos = {
                ComponentInfo::GetInfo<Components>()...
            };
            return componentInfos[p_componentIndex];
        }

    private:
        ComponentInfo (*m_getComponentInfo)(SizeT p_componentIndex);
    };
} // namespace Zen