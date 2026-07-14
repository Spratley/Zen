#pragma once

// #include "../../Utils/Zen_LoopUtils.h"
// #include "../../Utils/Zen_TypeListUtils.h"
#include "../../Zen_Types.h"
#include "../Zen_Entity.h"
#include "Zen_Archetype.h"
#include "Zen_ArchetypeStorage.h"

namespace Zen
{
    // Double ended arena buffer
    // Front end stores the actual data segments
    // Back end stores the metadata for each segment, reverse order as they appear
    // [A, B, C, ....., C, B, A]

    class ArchetypeArena
    {
    public:
        consteval ArchetypeArena() = default;

        constexpr SizeT ArchetypeCount() const { return m_archetypeCount; }

        constexpr EntityKey Spawn(Archetype const& /*p_archetype*/)
        {
            //// Step 1: Get Archetype Storage from Archetype
            //// Step 1.2: If Archetype is not found, push a new segment
            //ArchetypeStorage& storage = GetArchetypeStorage(p_archetype);

            //// Step 3: Check if Archetype Storage needs to grow (Count + 1 >= Capacity)
            //if (storage.m_count + 1 >= storage.m_capacity)
            //{
            //    // Step 3.1: Double size of Archety Storage to increase capacity
            //    SizeT const newCapacity = storage.m_capacity * 2;
            //    // TODO: Actually grow the storage and bump siblings back to fit
            //}

            //// Step 4: Append new entity to Archetype Storage
            //SizeT const entityIndex = storage.m_count;

            //// Step 5: Return EntityKey object
            //return EntityKey{ .m_index = static_cast<U32>(entityIndex), .m_archetypeIndex = archetypeIndex };
            return EntityKey();
        }

    protected:
        constexpr ArchetypeStorage const& GetArchetypeStorage(SizeT /*p_index*/) const
        {
            return *reinterpret_cast<ArchetypeStorage const*>(0);
        }

        constexpr ArchetypeStorage& GetArchetypeStorage(SizeT p_index)
        {
            return const_cast<ArchetypeStorage&>(const_cast<ArchetypeArena const*>(this)->GetArchetypeStorage(p_index));
        }

    protected:
        SizeT m_archetypeCount = 0;
        void* m_archetypeBufferBegin = nullptr;
        void* m_archetypeBufferEnd = nullptr;
        SizeT dummy;
    };

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
        }

    private:
        alignas(16) Byte m_arena[ArenaSizeBytes];
    };

} // namespace Zen