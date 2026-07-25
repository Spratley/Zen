#pragma once

#include "../../Component/Zen_ComponentInfo.h"
#include "../../Utils/Zen_DebugUtils.h"
#include "../../Utils/Zen_LoopUtils.h"
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
            ZEN_ASSERT(p_componentIndex < m_componentCount, "GetComponentInfo called out of bounds!");
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

        constexpr SizeT CalculateStride() const
        {
            SizeT stride = 0;
            for (auto i : LoopUtils::CountTo(m_componentCount))
            {
                stride += GetComponentInfo(i).m_size;
            }
            return stride;
        }

        SizeT GetComponentCount() const { return m_componentCount; }

        friend constexpr bool operator==(Archetype const& p_archetype, U64 const& p_signature)
        {
            return p_archetype.m_signature == p_signature;
        }

    private:
        template <typename... Components>
        constexpr Archetype(TypeList<Components...> p_componentTypes, U64 p_signature)
            : m_getComponentInfo(&GetComponentInfoImpl<Components...>)
            , m_localTypeIndexer(p_componentTypes)
            , m_signature(p_signature)
            , m_componentCount(sizeof...(Components))
        {}

        template <typename... Components>
        static constexpr ComponentInfo const& GetComponentInfoImpl(SizeT p_componentIndex)
        {
            static constexpr std::array<ComponentInfo, sizeof...(Components)> componentInfos = {
                ComponentInfo::GetInfo<Components>()...
            };
            return componentInfos[p_componentIndex];
        }

    private:
        ComponentInfo const& (*m_getComponentInfo)(SizeT p_componentIndex);
        TypeUtils::TypeIndexer m_localTypeIndexer;
        U64 m_signature;
        SizeT m_componentCount;
    };

    class ArchetypeFactory
    {
    public:
        template <typename... Components>
        static constexpr Archetype MakeArchetype()
        {
            using SortedComponents =
              TypeListUtils::SortTypes_T<TypeListUtils::CompareECSPacking, TypeList<Components...>>;
            return Archetype(SortedComponents{}, GenerateSignature<Components...>());
        }

        template <typename... Components>
        static consteval U64 GenerateSignature()
        {
            return (TypeUtils::HashType_V<Components, U64> ^ ...);
        }
    };
} // namespace Zen