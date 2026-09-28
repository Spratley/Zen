#pragma once

#include "../Utils/Zen_ConceptUtils.h"
#include "../Zen_Types.h"

namespace Zen
{
    class Garden;

    struct Entity
    {
        enum class ID : SizeT;

        constexpr Entity() = default;
        constexpr Entity(ID p_id, Garden* p_garden)
            : m_id(p_id)
            , m_garden(p_garden)
        {}

        constexpr bool IsValid() const { return m_id != ID(SizeT_Max); }
        constexpr bool operator!() const { return !IsValid(); }

        constexpr ID GetID() const { return m_id; }

        template <typename Component>
        requires(!Zen::ConceptUtils::IsIndirectType<Component>)
        Component* GetComponent() const;

    private:
        // TODO: Add generation so we don't get old handles pointing to new entities
        ID m_id = ID(SizeT_Max);
        Garden* m_garden = nullptr;
    };

    struct EntityKey
    {
        SizeT m_index;
        SizeT m_archetypeIndex;
    };
} // namespace Zen