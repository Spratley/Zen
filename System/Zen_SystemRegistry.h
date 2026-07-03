#pragma once

#include "../Entity/Archetype/Zen_ArchetypeArena.h"
#include "../Utils/Zen_TupleUtils.h"
#include "Zen_System.h"

namespace Zen
{
    class SystemRegistry
    {
    public:
        using ExecuteSignature = void (*)(ArchetypeArena&);

        template <typename... SystemTypes>
        consteval SystemRegistry(Tuple<SystemTypes...>)
            : m_execute(&ExecuteSystemsImpl<SystemTypes...>)
        {}

        constexpr void Execute(ArchetypeArena& p_archetypeArena) { m_execute(p_archetypeArena); }

    private:
        template <typename SystemType>
        static constexpr void ExecuteSystem(ArchetypeArena& p_archetypeArena)
        {
            static_assert(IsSystem<SystemType>, "Non-System type detected in Tick!");

            // Gather view of all archetypes that match the component signature of this system
            // Loop and pass them group at a time to Execute

            ArchetypeArenaView<typename SystemType::ComponentList> arenaView(p_archetypeArena);

            for (ArchetypeStorage const& archetypeStorage : arenaView)
            {
                SystemType::Execute(archetypeStorage);
            }
        }

        template <typename... SystemTypes>
        static constexpr void ExecuteSystemsImpl(ArchetypeArena& p_archetypeArena)
        {
            (ExecuteSystem<SystemTypes>(p_archetypeArena), ...);
        }

    private:
        ExecuteSignature m_execute;
    };
} // namespace Zen