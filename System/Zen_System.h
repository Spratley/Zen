#pragma once

#include "../Entity/Archetype/Zen_ArchetypeStorage.h"

#include <concepts>

namespace Zen
{
    template <typename System, typename... Components>
    class SystemBase
    {
        friend class SystemRegistry;

        using CRTPType = System;
        using BaseType = SystemBase<System, Components...>;
        static_assert(!std::is_same_v<TypeList<>, TypeList<Components...>>, "No components requested for system tick!");

    public:
        using ComponentList = TypeList<Components...>;

    protected:
        using ComponentView = ArchetypeView<Components...>;

    private:
        static constexpr void Execute(ArchetypeStorage const& p_archetypeData /*Delta Time?*/)
        {
            // Create and pass view of components to CRTP Tick function
            // Tick function can then just ranged for loop the view
            System::Tick(ComponentView(p_archetypeData));
        }
    };

    template <typename System>
    concept IsSystem = requires(System) { &System::Tick; } && std::derived_from<System, typename System::BaseType>
                       && std::same_as<System, typename System::CRTPType>;

} // namespace Zen