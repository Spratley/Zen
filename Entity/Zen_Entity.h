#pragma once

#include "../Zen_Types.h"

// Can these includes be moved or removed?
#include <vector>

namespace Zen
{
    class Garden;

    struct Entity
    {
        enum class ID : SizeT;

        Entity(ID p_id, Garden* p_garden)
            : m_id(p_id)
            , m_garden(p_garden)
        {}

        Entity()
            : m_id(ID(SizeT_Max))
            , m_garden(nullptr)
        {}

        ID GetID() const { return m_id; }

        // Gotta figure out the interface for the rest of the system
        // This is the ideal API, end user doesn't have to care about anything
        // This is a circular dependency though so it won't work
        template <typename Component>
        Component* GetComponent() const;

    private:
        ID m_id;
        Garden* m_garden;
    };

    struct EntityKey
    {
        SizeT m_index;
        SizeT m_archetypeIndex;
    };

    using EntityProxyStorage = std::vector<EntityKey>;
} // namespace Zen