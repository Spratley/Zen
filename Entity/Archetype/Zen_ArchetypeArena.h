#pragma once

#include "../../Utils/Zen_DebugUtils.h"
#include "../../Utils/Zen_MemoryUtils.h"
#include "../../Zen_Types.h"
#include "../Zen_Entity.h"
#include "Zen_Archetype.h"
#include "Zen_ArchetypeStorage.h"

#include <algorithm>

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
        ArchetypeArena() = default;

        SizeT ArchetypeCount() const { return m_archetypeCount; }

        template <typename... Components>
        EntityKey Spawn()
        {
            EnsureArchetype<Components...>();
            EntityKey entity = PushEntity(ArchetypeFactory::GenerateSignature<Components...>());
            ConstructEntity<Components...>(entity);
            return entity;
        }

        ArchetypeStorage const* GetArchetypeStorage(SizeT p_index) const
        {
            ZEN_ASSERT(p_index < m_archetypeCount, "Index out of bounds!");
            return GetArchetypeStorageBuffer() - p_index;
        }

    private:
        ArchetypeStorage* GetArchetypeStorage(SizeT p_index)
        {
            return const_cast<ArchetypeStorage*>(const_cast<ArchetypeArena const*>(this)->GetArchetypeStorage(p_index));
        }

        ArchetypeStorage* GetArchetypeStorageBuffer() const
        {
            return static_cast<ArchetypeStorage*>(static_cast<void*>(m_archetypeBufferEnd)) - 1;
        }

        SizeT FindArchetypeStorage(U64 p_archetypeSignature)
        {
            ArchetypeStorage* first = GetArchetypeStorageBuffer();
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

        template <typename... Components>
        ArchetypeStorage& PushArchetype()
        {
            m_archetypeCount++;
            void* newArchetypeDestination = GetArchetypeStorage(m_archetypeCount - 1);
            ArchetypeStorage* archetypeStorage =
              MemoryUtils::PlacementNew<ArchetypeStorage>(newArchetypeDestination, TypeList<Components...>{});

            // I don't like this branch here, but PushArchetype shouldn't be called often enough to care
            if (m_archetypeCount == 1)
            {
                archetypeStorage->m_begin = m_archetypeBufferBegin;
            }
            else
            {
                ArchetypeStorage* lastArchetypeStorage = GetArchetypeStorage(m_archetypeCount - 2);
                archetypeStorage->m_begin = lastArchetypeStorage->m_end;
            }
            archetypeStorage->m_end = archetypeStorage->m_begin;

            return *archetypeStorage;
        }

        EntityKey PushEntity(U64 p_archetypeSignature)
        {
            SizeT archetypeIndex = FindArchetypeStorage(p_archetypeSignature);
            ZEN_ASSERT(archetypeIndex != SizeT_Max,
                       "Archetype not found! Please call EnsureArchetype before you try to Spawn");
            ArchetypeStorage& archetypeStorage = *GetArchetypeStorage(archetypeIndex);

            if (archetypeStorage.m_count + 1 >= archetypeStorage.m_capacity)
            {
                SizeT newCapacity = (archetypeStorage.m_capacity == 0) ? 1 : archetypeStorage.m_capacity * 2;
                AllocateSize(archetypeIndex, newCapacity);
            }

            SizeT const entityIndex = archetypeStorage.m_count++;
            return EntityKey{ .m_index = entityIndex, .m_archetypeIndex = archetypeIndex };
        }

        template <typename... Components>
        void EnsureArchetype()
        {
            if (FindArchetypeStorage(ArchetypeFactory::GenerateSignature<Components...>()) == SizeT_Max)
            {
                PushArchetype<Components...>();
            }
        }

        template <typename... Components>
        void ConstructEntity(EntityKey p_entity)
        {
            ArchetypeStorage const& archetypeStorage = *GetArchetypeStorage(p_entity.m_archetypeIndex);

            ZEN_ASSERT(sizeof...(Components) == archetypeStorage.m_archetype.GetComponentCount(),
                       "ConstructEntity() called with an incorrect number of components!");

            (ConstructComponent<Components>(archetypeStorage, p_entity.m_index), ...);
        }

        template <typename Component>
        Component* ConstructComponent(ArchetypeStorage const& p_archetypeStorage, SizeT p_entityIndex)
        {
            Component* buffer = p_archetypeStorage.GetBuffer<Component>();
            return MemoryUtils::PlacementNew<Component>(buffer + p_entityIndex /*, args*/);
        }

        bool AllocateSize(SizeT p_archetypeIndex, SizeT p_entityCount)
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
                Byte* oldComponentBuffer =
                  static_cast<Byte*>(archetypeStorage->CalculateBufferForCapacity(i, oldCapacity));
                SizeT oldBufferSize = archetypeStorage->m_archetype.GetComponentInfo(i).m_size * oldCapacity;
                Byte* newComponentBuffer = static_cast<Byte*>(archetypeStorage->GetBuffer(i));

                std::copy_backward(oldComponentBuffer,
                                   oldComponentBuffer + oldBufferSize,
                                   newComponentBuffer + oldBufferSize);
            }
            return true;
        }

    protected:
        SizeT m_archetypeCount = 0;
        SizeT m_usedSize = 0;
        Byte* m_archetypeBufferBegin = nullptr;
        Byte* m_archetypeBufferEnd = nullptr;
    };

    // I have separated this out into another class to get around
    // not needing to know the template parameter at compile time
    // Later I'm going to come back and figure out a better way to do this
    template <SizeT ArenaSizeBytes>
    class SizedArchetypeArena : public ArchetypeArena
    {
    public:
        SizedArchetypeArena()
            : ArchetypeArena()
            , m_arena{}
        {
            m_archetypeBufferBegin = m_arena;
            m_archetypeBufferEnd = m_arena + ArenaSizeBytes;

            static_assert((ArenaSizeBytes % 16) == 0,
                          "Arena Size must be a multiple of 16 bytes to preserve array layout!");
        }

    private:
        alignas(16) Byte m_arena[ArenaSizeBytes];
    };

} // namespace Zen

#pragma warning(pop)