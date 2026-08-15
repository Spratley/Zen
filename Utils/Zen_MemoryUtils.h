#pragma once

#include <memory>

namespace Zen
{
    namespace MemoryUtils
    {
        template <typename T, typename... Args>
        [[nodiscard]] constexpr T* PlacementNew(T* p_address, Args&&... p_args)
        {
            return std::construct_at<T, Args...>(p_address, std::forward<Args>(p_args)...);
        }
    } // namespace MemoryUtils
} // namespace Zen