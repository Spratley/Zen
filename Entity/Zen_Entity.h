#pragma once

#include "../Zen_Types.h"

// Can these includes be moved or removed?
#include <vector>

namespace Zen
{
    struct Entity
    {
        enum class ID : U32;

        Entity(ID p_id)
            : m_id(p_id)
        {}

        ID GetID() const { return m_id; }

        // Gotta figure out the interface for the rest of the system
        // This is the ideal API, end user doesn't have to care about anything
        /*template <typename ComponentType>
        Component& GetComponent()
        {
            return ???;
        }*/

    private:
        ID m_id;
    };
    static_assert(sizeof(Entity) == sizeof(Entity::ID), "Entity should just be an alias for an ID!");

    struct EntityKey
    {
        SizeT m_index;
        SizeT m_archetypeIndex;
    };

    using EntityProxyStorage = std::vector<EntityKey>;
} // namespace Zen