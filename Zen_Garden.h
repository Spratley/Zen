#pragma once

#include "Component/Zen_ComponentRegistry.h"
#include "Entity/Archetype/Zen_ArchetypeArena.h"
#include "Entity/Zen_Entity.h"
#include "Entity/Zen_EntityView.h"
#include "System/Zen_SystemRegistry.h"
#include "Utils/Zen_ConceptUtils.h"
#include "Utils/Zen_TypeListUtils.h"
#include "Zen_Types.h"

#include <vector>

namespace Zen
{
    // TODO: Validate that all systems operate on components that are registered in the component registry

    class Garden
    {
        friend struct Entity;

    public:
        template <typename... ComponentTypes, typename... SystemTypes>
        requires((ConceptUtils::IsPureType<ComponentTypes> && ...) && (ConceptUtils::IsPureType<SystemTypes> && ...))
        constexpr Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>);

        void Initialize(SizeT p_arenaSizeBytes) { m_archetypeArena.Initialize(p_arenaSizeBytes); }

        void Tick() { m_systemRegistry.Execute(m_archetypeArena); }

        template <typename... Components>
        requires(ConceptUtils::IsPureType<Components> && ...)
        constexpr Entity Spawn();

        template <typename... Components>
        constexpr Entity Spawn(Components&&... p_components);

        void Destroy(Entity const& p_entity) { m_archetypeArena.Destroy(GetEntityKey(p_entity)); }

        template <typename... Components>
        requires((!ConceptUtils::IsIndirectType<Components>) && ...)
        EntityView<Components...> ViewComponents() const
        {
            return EntityView<Components...>(m_archetypeArena);
        }

    private:
        template <typename Component>
        requires(!ConceptUtils::IsIndirectType<Component>)
        Component* GetComponent(Entity const& p_entity)
        {
            return m_archetypeArena.GetComponent<Component>(GetEntityKey(p_entity));
        }

        constexpr EntityKey GetEntityKey(Entity const& p_entity) const
        {
            return m_entities[static_cast<SizeT>(p_entity.GetID())];
        }

    private:
        using EntityProxyStorage = std::vector<EntityKey>;
        EntityProxyStorage m_entities;

        ComponentRegistry m_componentRegistry;
        SystemRegistry m_systemRegistry;
        ArchetypeArena m_archetypeArena;
    };

    template <typename... ComponentTypes, typename... SystemTypes>
    requires((ConceptUtils::IsPureType<ComponentTypes> && ...) && (ConceptUtils::IsPureType<SystemTypes> && ...))
    constexpr Garden::Garden(TypeList<ComponentTypes...>, TypeList<SystemTypes...>)
        : m_componentRegistry(
            TypeListUtils::SortTypes_T<TypeListUtils::CompareECSPacking,
                                       TypeListUtils::FilterDuplicates_T<TypeList<ComponentTypes...>>>{})
        , m_systemRegistry(TypeListUtils::FilterDuplicates_T<TypeList<SystemTypes...>>{})
        , m_archetypeArena()
    {}

    template <typename... Components>
    requires(ConceptUtils::IsPureType<Components> && ...)
    constexpr Entity Garden::Spawn()
    {
        return Spawn(Components{}...);
    }

    template <typename... Components>
    constexpr Entity Garden::Spawn(Components&&... p_components)
    {
        EntityKey entityKey = m_archetypeArena.Spawn<Components...>(std::forward<Components>(p_components)...);
        m_entities.push_back(entityKey);
        return Entity(Entity::ID(m_entities.size() - 1), this);
    }

    // Definition for Entity::GetComponent since it needs to be aware of Garden's API
    template <typename Component>
    requires(!ConceptUtils::IsIndirectType<Component>)
    Component* Entity::GetComponent() const
    {
        return m_garden->GetComponent<Component>(*this);
    }
} // namespace Zen