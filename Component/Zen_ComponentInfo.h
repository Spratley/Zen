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

        // TODO: Remove me
        static consteval ComponentInfo GetNullInfo() { return ComponentInfo(); }

        consteval ComponentInfo(TypelessComponentFactory p_factory = TypelessComponentFactory(),
                                U64 p_typeHash = 0u,
                                SizeT p_size = 0u,
                                SizeT p_alignment = 0u)
            : m_factory(p_factory)
            , m_typeHash(p_typeHash)
            , m_size(p_size)
            , m_alignment(p_alignment)
        {}

        TypelessComponentFactory m_factory;
        U64 m_typeHash;
        SizeT m_size;
        SizeT m_alignment;
    };
} // namespace Zen