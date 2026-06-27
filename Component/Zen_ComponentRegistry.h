#pragma once

#include "../Utils/Zen_TypeUtils.h"
#include "Zen_ComponentInfo.h"

namespace Zen
{
    class ComponentRegistry
    {
        using GetComponentInfoSignature = ComponentInfo (*)(U32);

    public:
        template <typename... ComponentTypes>
        consteval ComponentRegistry(Tuple<ComponentTypes...>)
            : m_componentIndexer(Tuple<ComponentTypes...>{})
            , m_getComponentInfo(&GetComponentInfoImpl<ComponentTypes...>)
        {}

        template <typename T>
        consteval U32 GetComponentIndex() const
        {
            return m_componentIndexer.GetIndex<T>();
        }

        consteval ComponentInfo GetComponentInfo(U32 p_componentIndex) const
        {
            return m_getComponentInfo(p_componentIndex);
        }

    private:
        template <typename... ComponentTypes>
        static constexpr ComponentInfo GetComponentInfoImpl(U32 p_componentIndex)
        {
            constexpr auto componentInfos = []() {
                return std::array<ComponentInfo, sizeof...(ComponentTypes)>{
                    ComponentInfo::GetInfo<ComponentTypes>()...
                };
            }();

            if (p_componentIndex >= componentInfos.size())
            {
                return ComponentInfo();
            }
            return componentInfos[p_componentIndex];
        }

    private:
        TypeUtils::TypeIndexer m_componentIndexer;
        GetComponentInfoSignature m_getComponentInfo;
    };
} // namespace Zen