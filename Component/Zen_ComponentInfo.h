#pragma once

#include "../Utils/Zen_TypeUtils.h"
#include "../Zen_Types.h"
#include "Zen_TypelessComponentFactory.h"

namespace Zen
{
    struct ComponentInfo
    {
        template <typename ComponentType>
        static consteval ComponentInfo GetInfo()
        {
            return ComponentInfo(TypelessComponentFactory::GetFactory<ComponentType>(),
                                 TypeUtils::HashType_V<ComponentType, U64>,
                                 sizeof(ComponentType),
                                 alignof(ComponentType));
        };

        consteval ComponentInfo() = default;
        consteval ComponentInfo(TypelessComponentFactory p_factory, U64 p_typeHash, SizeT p_size, SizeT p_alignment)
            : m_factory(p_factory)
            , m_typeHash(p_typeHash)
            , m_size(p_size)
            , m_alignment(p_alignment)
        {}

        TypelessComponentFactory m_factory = TypelessComponentFactory();
        U64 m_typeHash = 0u;
        SizeT m_size = 0u;
        SizeT m_alignment = 0u;
    };
} // namespace Zen