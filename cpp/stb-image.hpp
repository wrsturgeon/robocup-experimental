#pragma once

#ifdef NDEBUG
#ifndef PROFILING_COMPILATION  // so we don't shoot ourselves in the foot trying to measure things
#error "Please don't use image IO in release mode <3"
#endif  // PROFILING_COMPILATION
#endif  // NDEBUG

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
// NOLINTBEGIN
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// NOLINTEND
#pragma clang diagnostic pop
