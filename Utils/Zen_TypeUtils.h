#pragma once

#include "Zen_MathUtils.h"
#include "Zen_TupleUtils.h"

#include <array>

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
#elifndef __PRETTY_FUNCTION
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
            constexpr static U32 value = MathUtils::HashFNV1A32(GetUniqueTypeString<T>());
        };

        template <typename T>
        struct HashType<T, U64>
        {
            constexpr static U64 value = MathUtils::HashFNV1A64(GetUniqueTypeString<T>());
        };

        template <typename T, typename HashSize>
        constexpr inline HashSize HashType_V = HashType<T, HashSize>::value;

        // -=-=-=-= Consteval Type Indexing =-=-=-=-
        struct TypeIndexer
        {
            using GetIDSignature = U32 (*)(U64 const&);

        public:
            template <typename... Types>
            static consteval TypeIndexer IndexTypes(Tuple<Types...>)
            {
                return TypeIndexer(&GetIndexImpl<Types...>);
            }

        public:
            template <typename T>
            consteval U32 GetIndex() const
            {
                return m_getID(HashType_V<T, U64>);
            }

        private:
            template <typename... Types>
            static constexpr U32 GetIndexImpl(U64 const& p_hash)
            {
                constexpr SizeT typeCount = sizeof...(Types);
                constexpr auto typeHashes = []() {
                    return std::array<U64, typeCount>{ HashType_V<Types, U64>... };
                }();

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
            TypeIndexer() = delete;
            consteval TypeIndexer(GetIDSignature const& p_getIDFunction)
                : m_getID(p_getIDFunction)
            {}

        private:
            GetIDSignature m_getID;
        };

    } // namespace TypeUtils
} // namespace Zen