#pragma once

namespace Zen
{
    using I8 = char;
    using I16 = short;
    using I32 = int;
    using I64 = long long int;

    using U8 = unsigned char;
    using U16 = unsigned short;
    using U32 = unsigned int;
    using U64 = unsigned long long int;

    using SizeT = decltype(sizeof(0));

    enum class Byte : U8 {};
} // namespace Zen

#define ZEN_VALIDATE_INT_SIZE(Bits) static_assert(sizeof(Zen::I##Bits) == sizeof(Zen::U##Bits) && sizeof(Zen::I##Bits) == (Bits / 8), #Bits " Bit Integers are Sized Wrong!");
ZEN_VALIDATE_INT_SIZE(8);
ZEN_VALIDATE_INT_SIZE(16);
ZEN_VALIDATE_INT_SIZE(32);
ZEN_VALIDATE_INT_SIZE(64);
#undef ZEN_VALIDATE_INT_SIZE