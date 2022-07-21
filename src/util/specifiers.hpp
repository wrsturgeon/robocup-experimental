#include <options.hpp>
#if UTIL_ENABLED
#ifndef UTIL_SPECIFIERS_HPP_
#define UTIL_SPECIFIERS_HPP_

namespace util {



#define MEMBER_INLINE inline __attribute__((always_inline))
#define INLINE static MEMBER_INLINE



} // namespace util

#endif // UTIL_SPECIFIERS_HPP_

#endif // UTIL_ENABLED
