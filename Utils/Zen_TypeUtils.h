#pragma once

#include "Zen_MathUtils.h"
#include "Zen_TupleUtils.h"

#include <array>
#include <limits>

namespace Zen
{
    namespace TypeUtils
    {
        // -=-=-=-= Type Hash =-=-=-=-
        // Oh C++26 reflection, how I yearn for you

        // Note: Doesn't return the name of T, just a string that is unqiue for T
        // Used for hashing, but feel free to also use otherwise if for some reason you need to?
        template <typename T>
        consteval const char* GetUniqueTypeString()
        {
#ifdef _MSC_VER
            return __FUNCSIG__;
#elifndef __PRETTY_FUNCTION__
            static_assert(false,
                          "__PRETTY_FUNCTION__ not available in your compiler! Please provide a utility to access the "
                          "function signature including template arguments to keep Zen working!");
#else
            return __PRETTY_FUNCTION__;
#endif
        }

        template <typename T, typename HashSize>
        struct HashType
        {
            static_assert(false, "Unsupported hash type");
        };

        template <typename T>
        struct HashType<T, U32>
        {
            static constexpr U32 value = MathUtils::HashFNV1A32(GetUniqueTypeString<T>());
        };

        template <typename T>
        struct HashType<T, U64>
        {
            static constexpr U64 value = MathUtils::HashFNV1A64(GetUniqueTypeString<T>());
        };

        template <typename T, typename HashSize>
        constexpr inline HashSize HashType_V = HashType<T, HashSize>::value;

        // -=-=-=-= Type Hash Collision Validation =-=-=-=-
        template <typename T>
        struct TypeHashCollisionError
        {
            static_assert(sizeof(T) == 0, "Hash Collision Detected! This type needs to be renamed.");
        };

        template <typename Types, typename HashSize>
        struct ValidateHashCollisions;

        template <typename... Types, typename HashSize>
        struct ValidateHashCollisions<Tuple<Types...>, HashSize>
        {
            static constexpr SizeT invalidIndex = std::numeric_limits<SizeT>::max();

            static constexpr SizeT FindViolatingIndex()
            {
                constexpr SizeT typeCount = sizeof...(Types);
                if constexpr (typeCount < 2)
                {
                    return invalidIndex;
                }

                constexpr auto typeHashes = std::array<U64, typeCount>{ HashType_V<Types, U64>... };
                for (SizeT i = 0; i < (typeCount - 1); ++i)
                {
                    for (SizeT j = i + 1; j < typeCount; ++j)
                    {
                        if (typeHashes[i] == typeHashes[j])
                        {
                            return j;
                        }
                    }
                }
                return invalidIndex;
            }
            static constexpr SizeT violatingIndex = FindViolatingIndex();
            
            static constexpr bool value = []() {
                if constexpr (violatingIndex != invalidIndex)
                {
                    using CollidingType = typename std::tuple_element_t<violatingIndex, Tuple<Types...>>;
                    [[maybe_unused]] TypeHashCollisionError<CollidingType> error_trigger;
                }
                return violatingIndex == invalidIndex;
            }();
        };

        // -=-=-=-= Consteval Type Indexing =-=-=-=-
        struct TypeIndexer
        {
            using GetIndexSignature = U32 (*)(U64 const&);

        public:
            template <typename... Types>
            consteval TypeIndexer(Tuple<Types...>)
                : m_getIndex(&GetIndexImpl<Types...>)
            {}

        public:
            template <typename T>
            consteval U32 GetIndex() const
            {
                return m_getIndex(HashType_V<T, U64>);
            }

        private:
            template <typename... Types>
            static constexpr U32 GetIndexImpl(U64 const& p_hash)
            {
                constexpr SizeT typeCount = sizeof...(Types);
                constexpr auto typeHashes = std::array<U64, typeCount>{ HashType_V<Types, U64>... };
                constexpr bool hashCollisionDetected = !ValidateHashCollisions<Tuple<Types...>, U64>::value;
                if constexpr (hashCollisionDetected)
                {
                    return std::numeric_limits<U32>::max();
                }

                for (U32 i = 0; i < typeHashes.size(); ++i)
                {
                    if (p_hash == typeHashes[i])
                    {
                        return i;
                    }
                }
                return std::numeric_limits<U32>::max();
            }

        private:
            GetIndexSignature m_getIndex;
        };

    } // namespace TypeUtils
} // namespace Zen