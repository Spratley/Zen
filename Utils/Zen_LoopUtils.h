#pragma once

#include "../Zen_Types.h"

namespace Zen
{
    namespace LoopUtils
    {
        struct CountTo
        {
            constexpr CountTo(SizeT p_limit)
                : m_limit(p_limit)
            {}

            struct Iterator
            {
                constexpr Iterator(SizeT p_i)
                    : m_i(p_i)
                {}

                constexpr Iterator& operator++()
                {
                    ++m_i;
                    return *this;
                }

                constexpr SizeT operator*() { return m_i; }
                constexpr friend bool operator!=(Iterator const& p_lhs, Iterator const& p_rhs) { return p_lhs.m_i != p_rhs.m_i; }

                SizeT m_i;
            };

            constexpr Iterator begin() const { return Iterator(0); }
            constexpr Iterator end() const { return Iterator(m_limit); }

            SizeT const m_limit;
        };
    } // namespace LoopUtils
} // namespace Zen