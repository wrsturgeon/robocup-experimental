#ifndef UTIL_OPTIONS_HPP_
#define UTIL_OPTIONS_HPP_



// Detects system architecture (32b/64b) and inform various algorithms accordingly.
// Compile with the compiler argument -D_BITS={32,64} (whichever it is)
#ifdef _BITS

#if _BITS == 32 || _BITS == 64
#define BITS _BITS
#else
#error "Invalid value for _BITS"
#endif // 32/64

#else
#error "Compile with argument -D_BITS=$(getconf LONG_BIT)"
#endif // ifdef _BITS



#ifdef _TRAINING
#define TRAINING _TRAINING
#else
#define TRAINING 0
#endif // ifdef _TRAINING



#ifdef _IMAGE_H

#if _IMAGE_H > 0
#define IMAGE_H _IMAGE_H
#else
#error "_IMAGE_H <= 0"
#endif // _IMAGE_H > 0

#else
#error "Compile with argument -D_IMAGE_H=<image height>"
#endif // ifdef _IMAGE_H



#ifdef _IMAGE_W

#if _IMAGE_W > 0
#define IMAGE_W _IMAGE_W
#else
#error "_IMAGE_W <= 0"
#endif // _IMAGE_W > 0
#else

#error "Compile with argument -D_IMAGE_W=<image width>"
#endif // ifdef _IMAGE_W



#define IMAGE_DIAG_SQ ((IMAGE_W * IMAGE_W) + (IMAGE_H * IMAGE_H))



#ifdef _NAO_HEIGHT_MM
#define NAO_HEIGHT_MM _NAO_HEIGHT_MM
#else
// TODO: this is a ROUGH ESTIMATE
#define NAO_HEIGHT_MM 500
#endif // ifdef _NAO_HEIGHT_MM



#ifdef _DISPLAY
#define DISPLAY _DISPLAY
#else
#define DISPLAY 0
#endif // ifdef _DISPLAY



#endif // UTIL_OPTIONS_HPP_
