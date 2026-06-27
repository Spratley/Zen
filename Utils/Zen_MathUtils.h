#pragma once

#include "../Zen_Types.h"

namespace Zen
{
	namespace MathUtils
	{
            constexpr inline U32 FNV32_Base = 0x811C9DC5;
            constexpr inline U32 FNV32_Prime = 0x1000193;

            constexpr inline U64 FNV64_Base = 0xCBF29CE484222325;
            constexpr inline U64 FNV64_Prime = 0x100000001B3;

            constexpr U32 HashFNV1A32(const char* p_str)
            {
                U32 hash = FNV32_Base;
                while (*p_str)
                {
                    hash ^= static_cast<U32>(*p_str++);
                    hash *= FNV32_Prime;
                }
                return hash;
            }

            constexpr U64 HashFNV1A64(const char* p_str)
            {
                U64 hash = FNV64_Base;
                while (*p_str)
                {
                    hash ^= static_cast<U64>(*p_str++);
                    hash *= FNV64_Prime;
                }
                return hash;
            }
	}
}