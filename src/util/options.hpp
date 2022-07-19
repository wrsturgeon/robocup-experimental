#ifndef UTIL_OPTIONS_HPP_
#define UTIL_OPTIONS_HPP_



// Detects system architecture (32b/64b) and inform various algorithms accordingly.
// Compile with the compiler argument -D_BITS={32,64} (whichever it is)
#ifndef _BITS
#error "Compile with argument -D_BITS=$(getconf LONG_BIT)"
#else

#if _BITS == 32 || _BITS == 64
#define BITS _BITS
#else
#error "Invalid value for _BITS"
#endif // 32/64

#endif // ...ndef _BITS



#ifndef TRAINING
#define TRAINING 0
#endif // TRAINING



#ifndef _IMAGE_H
#error "Compile with argument -D_IMAGE_H=<image height>"
#else
#if _IMAGE_H > 0
#define IMAGE_H _IMAGE_H
#else
#error "_IMAGE_H <= 0"
#endif // _IMAGE_H > 0
#endif // ...ndef _IMAGE_H

#ifndef _IMAGE_W
#error "Compile with argument -D_IMAGE_W=<image width>"
#else
#if _IMAGE_W > 0
#define IMAGE_W _IMAGE_W
#else
#error "_IMAGE_W <= 0"
#endif // _IMAGE_W > 0
#endif // ...ndef _IMAGE_W

#define IMAGE_DIAG ((IMAGE_W * IMAGE_W) + (IMAGE_H * IMAGE_H))



#ifndef NAO_HEIGHT_MM
// This could be incredibly wrong please check
#define NAO_HEIGHT_MM 250
#endif // NAO_HEIGHT_MM



#endif // UTIL_OPTIONS_HPP_
