#include "Zen_ArchetypeArena.h"

#include "../../Entity/Zen_Entity.h"
#include "../../Utils/Zen_DebugUtils.h"
#include "../../Utils/Zen_LoopUtils.h"
#include "../../Zen_Types.h"
#include "Zen_ArchetypeStorage.h"

#include <algorithm>
#include <malloc.h>

namespace Zen
{
    void ArchetypeArena::Initialize(SizeT p_arenaCapacityBytes)
    {
        // TODO: Swap out malloc here for something cooler
        // That is, a more portable better for grabbing one huge chunk of memory
        void* buffer = std::malloc(p_arenaCapacityBytes);
        ZEN_ASSERT(buffer, "Failed to gather arena memory!");
        m_archetypeBufferBegin = static_cast<Byte*>(buffer);
        m_archetypeStorageBuffer =
          static_cast<ArchetypeStorage*>(static_cast<void*>(m_archetypeBufferBegin + p_arenaCapacityBytes)) - 1;
    }

    ArchetypeStorage const* ArchetypeArena::GetArchetypeStorage(SizeT p_index) const
    {
        ZEN_ASSERT(p_index < m_archetypeCount, "Index out of bounds!");
        return m_archetypeStorageBuffer - p_index;
    }

    ArchetypeStorage* ArchetypeArena::GetArchetypeStorage(SizeT p_index)
    {
        return const_cast<ArchetypeStorage*>(const_cast<ArchetypeArena const*>(this)->GetArchetypeStorage(p_index));
    }

    SizeT ArchetypeArena::FindArchetypeStorage(U64 p_archetypeSignature) const
    {
        ArchetypeStorage* first = m_archetypeStorageBuffer;
        for (auto i : LoopUtils::CountTo(m_archetypeCount))
        {
            ArchetypeStorage* archetypeStorage = first - i;
            if (archetypeStorage->m_archetype == p_archetypeSignature)
            {
                return i;
            }
        }
        return SizeT_Max;
    }

    EntityKey ArchetypeArena::PushEntity(U64 p_archetypeSignature)
    {
        SizeT archetypeIndex = FindArchetypeStorage(p_archetypeSignature);
        ZEN_ASSERT(archetypeIndex != SizeT_Max,
                   "Archetype not found! Please call EnsureArchetype before you try to Spawn");
        ArchetypeStorage& archetypeStorage = *GetArchetypeStorage(archetypeIndex);

        if (archetypeStorage.m_count + 1 >= archetypeStorage.m_capacity)
        {
            SizeT newCapacity = (archetypeStorage.m_capacity == 0) ? 1 : archetypeStorage.m_capacity * 2;
            AllocateArchetypeSize(archetypeIndex, newCapacity);
        }

        SizeT const entityIndex = archetypeStorage.m_count++;
        return EntityKey{ .m_index = entityIndex, .m_archetypeIndex = archetypeIndex };
    }

    bool ArchetypeArena::AllocateArchetypeSize(SizeT p_archetypeIndex, SizeT p_entityCount)
    {
        ZEN_ASSERT(p_archetypeIndex < m_archetypeCount, "Index out of bounds!");

        ArchetypeStorage* archetypeStorage = GetArchetypeStorage(p_archetypeIndex);
        if (archetypeStorage->m_capacity > p_entityCount)
        {
            // We already have enough space, no alloc needed :)
            return true;
        }

        SizeT newSizeBytes = archetypeStorage->m_archetype.GetStride() * p_entityCount;
        // Pad up to the nearest 16 byte boundary
        newSizeBytes = (newSizeBytes + 15) & ~15;

        // TODO: Validate that we're not overrunning the arena by doing this!

        SizeT const allocatedSize = archetypeStorage->GetAllocatedSize();
        ZEN_ASSERT(newSizeBytes >= allocatedSize, "Somehow we're shrinking the buffer! Something's gone wrong");
        if (newSizeBytes == allocatedSize)
        {
            return true;
        }

        // Bump buffers in front by the space needed
        SizeT additionalSpaceNeeded = newSizeBytes - allocatedSize;
        for (SizeT i = m_archetypeCount - 1; i > p_archetypeIndex; --i)
        {
            ArchetypeStorage* archetypeToMove = GetArchetypeStorage(i);

            Byte* const begin = archetypeToMove->m_begin;
            Byte* const end = archetypeToMove->m_end;
            Byte* const newEnd = end + additionalSpaceNeeded;
            std::copy_backward(begin, end, newEnd);

            archetypeToMove->m_begin = begin + additionalSpaceNeeded;
            archetypeToMove->m_end = newEnd;
        }

        archetypeStorage->m_end = archetypeStorage->m_end + additionalSpaceNeeded;
        SizeT const oldCapacity = archetypeStorage->m_capacity;
        archetypeStorage->m_capacity = p_entityCount;

        // TODO: Move to better home
        // Move existing component buffers to new buffer locations
        for (SizeT i = archetypeStorage->m_archetype.GetComponentCount() - 1; i > 0; --i)
        {
            Byte* oldComponentBuffer = static_cast<Byte*>(archetypeStorage->CalculateBufferForCapacity(i, oldCapacity));
            SizeT oldBufferSize = archetypeStorage->m_archetype.GetComponentInfo(i).m_size * oldCapacity;
            Byte* newComponentBuffer = static_cast<Byte*>(archetypeStorage->GetBuffer(i));

            std::copy_backward(oldComponentBuffer,
                               oldComponentBuffer + oldBufferSize,
                               newComponentBuffer + oldBufferSize);
        }
        return true;
    }

} // namespace Zen