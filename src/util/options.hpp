#ifndef UTIL_OPTIONS_HPP_
#define UTIL_OPTIONS_HPP_



// Detects system architecture (32b/64b) and inform various algorithms accordingly.
// Compile with the compiler argument -D_BITS={32,64} (whichever it is)
#ifndef BITS

#ifndef _BITS
#error "Compile with argument -D_BITS=$(getconf LONG_BIT)"
#endif // _BITS

#if _BITS == 32 || _BITS == 64
#define BITS _BITS
#else
#error "Invalid value for _BITS"
#endif // 32/64

#endif // BITS



#ifndef TRAINING
#define TRAINING 0
#endif // TRAINING



#endif // UTIL_OPTIONS_HPP_
