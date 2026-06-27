#pragma once

#include <new>

namespace Zen
{
    namespace MemoryUtils
    {
        template <typename T>
        [[nodiscard]] constexpr inline T* PlacementNew(void* p_address)
        {
            T* result = ::new (p_address) T;
            return std::launder(result);
        }
    } // namespace MemoryUtils
} // namespace Zen