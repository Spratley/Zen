#pragma once

#include <limits>

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
    constexpr SizeT SizeT_Max = std::numeric_limits<SizeT>::max();
    static_assert(sizeof(SizeT) >= sizeof(void*),
                  "For whatever reason, SizeT is not big enough to store a pointer in your program! This will cause "
                  "problems with Zen, please reach out to Spratley and he'll fix this. Until someone hits this assert "
                  "though, we're rocking SizeT for pointer math! (cool monkey emoji)");

    enum class Byte : U8 {};

    // TypeList is an empty object type that smuggles other types as template parameters. Originally I had used a Tuple
    // but found myself needing to instantiate the tuple to call constructors. This way, instantiating a TypeList to
    // call a templated constructor will always be a simple call, even if complex types are used as parameters.
    template <typename... Types>
    struct TypeList {};

} // namespace Zen

#define ZEN_VALIDATE_INT_SIZE(Bits) static_assert(sizeof(Zen::I##Bits) == sizeof(Zen::U##Bits) && sizeof(Zen::I##Bits) == (Bits / 8), #Bits " Bit Integers are Sized Wrong!");
ZEN_VALIDATE_INT_SIZE(8);
ZEN_VALIDATE_INT_SIZE(16);
ZEN_VALIDATE_INT_SIZE(32);
ZEN_VALIDATE_INT_SIZE(64);
#undef ZEN_VALIDATE_INT_SIZE