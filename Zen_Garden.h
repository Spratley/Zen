#pragma once

#include "Component/Zen_ComponentRegistry.h"
#include "Entity/Archetype/Zen_Archetype.h"
#include "Entity/Archetype/Zen_ArchetypeArena.h"
#include "Entity/Zen_Entity.h"
#include "System/Zen_SystemRegistry.h"
#include "Utils/Zen_TypeListUtils.h"
#include "Zen_Types.h"

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
        constexpr EntityKey Spawn(Components const&... /*p_components*/)
        {
            return m_archetypeArena.Spawn<Components...>();
        }

    private:
        EntityProxyStorage m_entities;
        ComponentRegistry m_componentRegistry;
        SystemRegistry m_systemRegistry;
        ArchetypeArena m_archetypeArena;
    };

    template <typename... ComponentTypes, typename... SystemTypes>
    constexpr Garden::Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>)
        : m_componentRegistry(
            TypeListUtils::SortTypes_T<TypeListUtils::CompareECSPacking,
                                       TypeListUtils::FilterDuplicates_T<TypeList<ComponentTypes...>>>{})
        , m_systemRegistry(TypeListUtils::FilterDuplicates_T<TypeList<SystemTypes...>>{})
        , m_archetypeArena()
    {
        m_archetypeArena.Initialize(1024);
    }
} // namespace Zen