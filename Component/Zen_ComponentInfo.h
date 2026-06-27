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
            return ComponentInfo{ .m_factory = TypelessComponentFactory::GetFactory<ComponentType>(),
                                  .m_size = sizeof(ComponentType),
                                  .m_alignment = alignof(ComponentType) };
        }

        TypelessComponentFactory m_factory;
        SizeT m_size;
        SizeT m_alignment;
    };
} // namespace Zen