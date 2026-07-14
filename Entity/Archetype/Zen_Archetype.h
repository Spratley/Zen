#pragma once

#include "../../Component/Zen_ComponentInfo.h"
#include "../../Utils/Zen_TypeListUtils.h"
#include "../../Utils/Zen_TypeUtils.h"
#include "../../Zen_Types.h"

#include <array>

namespace Zen
{
    // TODO: I think Archetype should be a private construct of something...
    // You can't manually instantiate an Archetype
    // Or can you? Factory isn't doing anything anymore? Should I remove it?
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

        template <typename... Components>
        constexpr bool Contains() const
        {
            return (m_localTypeIndexer.IsIndexed<Components>() && ...);
        }

    private:
        template <typename... Components>
        constexpr Archetype(TypeList<Components...> p_componentTypes)
            : m_getComponentInfo(&GetComponentInfoImpl<Components...>)
            , m_localTypeIndexer(p_componentTypes)
            , m_signature(GenerateSignature<Components...>())
            , m_componentCount(static_cast<U32>(sizeof...(Components)))
        {}

        template <typename... Components>
        static constexpr ComponentInfo const& GetComponentInfoImpl(SizeT p_componentIndex)
        {
            constexpr std::array<ComponentInfo, sizeof...(Components)> componentInfos = {
                ComponentInfo::GetInfo<Components>()...
            };
            return componentInfos[p_componentIndex];
        }

        template <typename... Components>
        static constexpr U64 GenerateSignature()
        {
            return (TypeUtils::HashType_V<Components>() ^ ...);
        }

    private:
        ComponentInfo const& (*m_getComponentInfo)(SizeT p_componentIndex);
        TypeUtils::TypeIndexer m_localTypeIndexer;
        U64 m_signature;
        U32 m_componentCount;
    };

    class ArchetypeFactory
    {
    public:
        template <typename... Components>
        constexpr Archetype MakeArchetype()
        {
            using SortedComponents = TypeListUtils::SortTypes_T<TypeListUtils::CompareECSPacking, TypeList<Components...>>;
            return Archetype(SortedComponents{});
        }
    };
} // namespace Zen