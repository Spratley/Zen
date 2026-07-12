#pragma once

// Can these includes be moved or removed?
#include <cstdint>
#include <vector>

namespace Zen
{
    struct Entity
    {
        enum class ID : uint32_t;
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
        Entity::ID m_index;
        uint32_t m_archetypeIndex;
    };

    using EntityProxyStorage = std::vector<EntityKey>;
} // namespace Zen