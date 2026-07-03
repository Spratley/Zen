#pragma once

#include "Component/Zen_ComponentRegistry.h"
#include "Entity/Zen_Entity.h"
#include "System/Zen_SystemRegistry.h"
#include "Utils/Zen_TupleUtils.h"

namespace Zen
{
    // TODO: Validate that all systems operate on components that are registered in the component registry

    class Garden
    {
    public:
        template <typename... ComponentTypes, typename... SystemTypes>
        constexpr Garden(Tuple<ComponentTypes...>, Tuple<SystemTypes...>);

        constexpr void Tick(/*Delta Time?*/) { m_systemRegistry.Execute(m_archetypeArena); }

        template <typename... Components>
        constexpr Entity Spawn(/*Archetype? Component list?*/)
        {
            return m_archetypeArena.Spawn<Components...>();
        }

    private:
        EntityProxyStorage m_entities;
        ComponentRegistry m_componentRegistry;
        SystemRegistry m_systemRegistry;
        SizedArchetypeArena<1024> m_archetypeArena;
    };

    template <typename... ComponentTypes, typename... SystemTypes>
    constexpr Garden::Garden(Tuple<ComponentTypes...>, Tuple<SystemTypes...>)
        : m_componentRegistry(TupleUtils::SortTypes_T<TupleUtils::CompareLargerAlignment,
                                                      TupleUtils::FilterDuplicates_T<Tuple<ComponentTypes...>>>{})
        , m_systemRegistry(TupleUtils::FilterDuplicates_T<Tuple<SystemTypes...>>{})
        , m_archetypeArena()
    {}
} // namespace Zen