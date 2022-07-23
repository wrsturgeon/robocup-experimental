// No include guards; taken care of in code_checker.sh

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



#ifndef _IMAGE_H
#define IMAGE_H 960
#else

#if _IMAGE_H > 0
#define IMAGE_H _IMAGE_H
#else
#error "_IMAGE_H <= 0"
#endif // _IMAGE_H > 0

#endif // ifdef _IMAGE_H



#ifndef _IMAGE_W
#define IMAGE_W 1280
#else

#if _IMAGE_W > 0
#define IMAGE_W _IMAGE_W
#else
#error "_IMAGE_W <= 0"
#endif // _IMAGE_W > 0

#endif // ifdef _IMAGE_W



#define IMAGE_DIAG_SQ ((IMAGE_W * IMAGE_W) + (IMAGE_H * IMAGE_H))



#ifdef _NAO_HEIGHT_MM
#define NAO_HEIGHT_MM _NAO_HEIGHT_MM
#else
// TODO: this is a ROUGH ESTIMATE
#define NAO_HEIGHT_MM 500
#endif // ifdef _NAO_HEIGHT_MM



#ifdef _UTIL_ENABLED
#define UTIL_ENABLED _UTIL_ENABLED
#else
#define UTIL_ENABLED 1
#endif // ifdef _UTIL_ENABLED

#ifdef _VISION_ENABLED
#define VISION_ENABLED _VISION_ENABLED
#else
#define VISION_ENABLED 1
#endif // ifdef _VISION_ENABLED



#ifdef _DEMOS_ENABLED
#define DEMOS_ENABLED _DEMOS_ENABLED
#else
#define DEMOS_ENABLED 0
#endif // ifdef _DEMOS_ENABLED

#ifdef _LEGACY_ENABLED
#define LEGACY_ENABLED _LEGACY_ENABLED
#else
#define LEGACY_ENABLED 0
#endif // ifdef _LEGACY_ENABLED

#ifdef _MEASURE_ENABLED
#define MEASURE_ENABLED _MEASURE_ENABLED
#else
#define MEASURE_ENABLED 0
#endif // ifdef _MEASURE_ENABLED

#ifdef _NAO_ENABLED
#define NAO_ENABLED _NAO_ENABLED
#else
#define NAO_ENABLED 0
#endif // ifdef _NAO_ENABLED

#ifdef _RND_ENABLED
#define RND_ENABLED _RND_ENABLED
#else
#define RND_ENABLED 0
#endif // ifdef _RND_ENABLED

#ifdef _SDL_ENABLED
#define SDL_ENABLED _SDL_ENABLED
#else
#define SDL_ENABLED 0
#endif // ifdef _SDL_ENABLED

#ifdef _TRAINING_ENABLED
#define TRAINING_ENABLED _TRAINING_ENABLED
#else
#define TRAINING_ENABLED 0
#endif // ifdef _TRAINING_ENABLED

#ifdef _WASSERSTEIN_ENABLED
#define WASSERSTEIN_ENABLED _WASSERSTEIN_ENABLED
#else
#define WASSERSTEIN_ENABLED 0
#endif // ifdef _WASSERSTEIN_ENABLED
