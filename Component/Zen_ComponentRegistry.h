#pragma once

#include "../Utils/Zen_TypeListUtils.h"
#include "../Utils/Zen_TypeUtils.h"
#include "../Zen_Types.h"
#include "Zen_ComponentInfo.h"

#include <array>
#include <limits>

namespace Zen
{
    class ComponentRegistry
    {
        using GetComponentInfoSignature = ComponentInfo (*)(U32);

    public:
        template <typename... ComponentTypes>
        consteval ComponentRegistry(TypeList<ComponentTypes...>)
            : m_componentIndexer(TypeList<ComponentTypes...>{})
            , m_getComponentInfo(&GetComponentInfoImpl<ComponentTypes...>)
        {}

        template <typename T>
        constexpr U32 GetComponentIndex() const
        {
            return m_componentIndexer.GetIndex<T>();
        }

        template <typename T>
        constexpr bool IsRegistered() const
        {
            return GetComponentIndex<T>() != std::numeric_limits<U32>::max();
        }

        constexpr ComponentInfo GetComponentInfo(U32 p_componentIndex) const
        {
            return m_getComponentInfo(p_componentIndex);
        }

    private:
        template <typename... ComponentTypes>
        static constexpr ComponentInfo GetComponentInfoImpl(U32 p_componentIndex)
        {
            constexpr SizeT componentCount = sizeof...(ComponentTypes);
            static constexpr auto componentInfos =
              std::array<ComponentInfo, componentCount>{ ComponentInfo::GetInfo<ComponentTypes>()... };

            if (p_componentIndex >= componentInfos.size())
            {
                return ComponentInfo::GetNullInfo();
            }
            return componentInfos[p_componentIndex];
        }

    private:
        TypeUtils::TypeIndexer m_componentIndexer;
        GetComponentInfoSignature m_getComponentInfo;
    };
} // namespace Zen