#pragma once

#include "Component/Zen_ComponentRegistry.h"
#include "Entity/Archetype/Zen_Archetype.h"
#include "Entity/Archetype/Zen_ArchetypeArena.h"
#include "Entity/Zen_Entity.h"
#include "System/Zen_SystemRegistry.h"
#include "Utils/Zen_TypeListUtils.h"

namespace Zen
{
    // TODO: Validate that all systems operate on components that are registered in the component registry

    class Garden
    {
    public:
        template <typename... ComponentTypes, typename... SystemTypes>
        constexpr Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>);

        constexpr void Tick(/*Delta Time?*/) { m_systemRegistry.Execute(m_archetypeArena); }

        template <typename... Components>
        constexpr Entity Spawn(Components const&... /*p_components*/)
        {
            return m_archetypeArena.Spawn(Archetype(TypeList<Components...>{}));
        }

    private:
        EntityProxyStorage m_entities;
        ComponentRegistry m_componentRegistry;
        SystemRegistry m_systemRegistry;
        SizedArchetypeArena<1024> m_archetypeArena;
    };

    template <typename... ComponentTypes, typename... SystemTypes>
    constexpr Garden::Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>)
        : m_componentRegistry(
            TypeListUtils::SortTypes_T<TypeListUtils::CompareLargerAlignment,
                                       TypeListUtils::FilterDuplicates_T<TypeList<ComponentTypes...>>>{})
        , m_systemRegistry(TypeListUtils::FilterDuplicates_T<TypeList<SystemTypes...>>{})
        , m_archetypeArena()
    {}
} // namespace Zen