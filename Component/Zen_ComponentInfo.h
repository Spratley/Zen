#pragma once

#include "../Zen_Types.h"
#include "Zen_TypelessComponentFactory.h"

namespace Zen
{
    struct ComponentInfo
    {
        template <typename ComponentType>
        static consteval ComponentInfo GetInfo()
        {
            ComponentInfo info;
            info.m_factory = TypelessComponentFactory::GetFactory<ComponentType>();
            info.m_size = sizeof(ComponentType);
            info.m_alignment = alignof(ComponentType);
            return info;
        };

        // TODO: Remove me
        static consteval ComponentInfo GetNullInfo() { return ComponentInfo(); }

        consteval ComponentInfo()
            : m_factory(TypelessComponentFactory())
            , m_size(0u)
            , m_alignment(0u)
        {}

        TypelessComponentFactory m_factory;
        SizeT m_size;
        SizeT m_alignment;
    };
} // namespace Zen