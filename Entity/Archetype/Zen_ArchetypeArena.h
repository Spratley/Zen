#pragma once

#include "../../Utils/Zen_DebugUtils.h"
#include "../../Utils/Zen_MemoryUtils.h"
#include "../../Zen_Types.h"
#include "../Zen_Entity.h"
#include "Zen_Archetype.h"
#include "Zen_ArchetypeStorage.h"

#pragma warning(push)
#pragma warning(disable : 4324)

namespace Zen
{
    // Double ended arena buffer
    // Front (Lower address) end stores the actual data segments
    // Back (Higher address) end stores the metadata for each segment, reverse order as they appear
    // [A_Block, B_Block, C_Block, ....., C, B, A]

    class ArchetypeArena
    {
    public:
        consteval ArchetypeArena() = default;

        constexpr SizeT ArchetypeCount() const { return m_archetypeCount; }

        constexpr EntityKey Spawn(Archetype const& /*p_archetype*/)
        {
            //// Step 1: Get Archetype Storage from Archetype
            //// Step 1.2: If Archetype is not found, push a new segment
            // ArchetypeStorage& storage = GetArchetypeStorage(p_archetype);

            //// Step 3: Check if Archetype Storage needs to grow (Count + 1 >= Capacity)
            // if (storage.m_count + 1 >= storage.m_capacity)
            //{
            //     // Step 3.1: Double size of Archety Storage to increase capacity
            //     SizeT const newCapacity = storage.m_capacity * 2;
            //     // TODO: Actually grow the storage and bump siblings back to fit
            // }

            //// Step 4: Append new entity to Archetype Storage
            // SizeT const entityIndex = storage.m_count;

            //// Step 5: Return EntityKey object
            // return EntityKey{ .m_index = static_cast<U32>(entityIndex), .m_archetypeIndex = archetypeIndex };
            return EntityKey();
        }

    protected:
        constexpr ArchetypeStorage const* GetArchetypeStorage(SizeT p_index) const
        {
            ZEN_ASSERT(p_index < m_archetypeCount, "Index out of bounds!");
            return static_cast<ArchetypeStorage*>(m_archetypeBufferEnd) - p_index - 1;
        }

        constexpr ArchetypeStorage* GetArchetypeStorage(SizeT p_index)
        {
            return const_cast<ArchetypeStorage*>(const_cast<ArchetypeArena const*>(this)->GetArchetypeStorage(p_index));
        }

    public:
        constexpr ArchetypeStorage& PushArchetype(/*Add Params*/)
        {
            m_archetypeCount++;
            void* newArchetypeDestination = GetArchetypeStorage(m_archetypeCount - 1);
            ArchetypeStorage* archetypeStorage = MemoryUtils::PlacementNew<ArchetypeStorage>(newArchetypeDestination);

            if (m_archetypeCount == 1)
            {
                archetypeStorage->m_buffer = m_archetypeBufferBegin;
            }
            else
            {
                // Since we're growing backwards, the saved address of the last most recent archetype will also be the
                // address for a new buffer with size 0. As we add elements, it will grow down in addresses.
                ArchetypeStorage* lastArchetypeStorage = GetArchetypeStorage(m_archetypeCount - 2);
                archetypeStorage->m_buffer = lastArchetypeStorage->m_buffer;
            }

            return *archetypeStorage;
        }

    protected:
        SizeT m_archetypeCount = 0;
        SizeT m_usedSize = 0;
        void* m_archetypeBufferBegin = nullptr;
        void* m_archetypeBufferEnd = nullptr;
    };

    // I have separated this out into another class to get around
    // not needing to know the template parameter at compile time
    // Later I'm going to come back and figure out a better way to do this
    template <SizeT ArenaSizeBytes>
    class SizedArchetypeArena : public ArchetypeArena
    {
    public:
        consteval SizedArchetypeArena()
            : ArchetypeArena()
            , m_arena{}
        {
            m_archetypeBufferBegin = static_cast<void*>(m_arena);
            m_archetypeBufferEnd = static_cast<void*>(m_arena + ArenaSizeBytes);

            static_assert((ArenaSizeBytes % 16) == 0, "Arena Size must be a multiple of 16 bytes to preserve array layout!");
        }

    private:
        alignas(16) Byte m_arena[ArenaSizeBytes];
    };

} // namespace Zen

#pragma warning(pop)