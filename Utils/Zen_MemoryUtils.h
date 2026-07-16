#pragma once

#include <new>

namespace Zen
{
    namespace MemoryUtils
    {
        template <typename T, typename... Args>
        [[nodiscard]] constexpr inline T* PlacementNew(void* p_address, Args... p_args)
        {
            T* result = ::new (p_address) T(p_args...);
            return std::launder(result);
        }
    } // namespace MemoryUtils
} // namespace Zen