#pragma once

#if defined(ZEN_USE_ASSERTS) || 1
#include <cstdlib>
#include <intrin.h>
#include <iostream>

#define ZEN_ASSERT(Condition, Message) DebugUtils::Assert(Condition, __FILE__, __LINE__, Message);

namespace Zen
{
    namespace DebugUtils
    {
        constexpr inline void Assert(bool p_condition,
                                     char const* p_file,
                                     int p_line,
                                     char const* p_message)
        {
            if (!p_condition)
            {
                std::cerr << "Error: Assertion failed in " << p_file << " on line " << p_line << "\n\"" << p_message
                          << "\"" << std::endl;
                
                // TODO: Make this portable
                // And here's where I'd put my C++ 26 std::breakpoint_if_debugging
                // IF I HAD A STABLE RELEASEs
                __debugbreak();
                abort();
            }
        }
    } // namespace DebugUtils
} // namespace Zen
#else
#define ZEN_ASSERT(Condition, Message)
#endif // defined(ZEN_ASSERT)