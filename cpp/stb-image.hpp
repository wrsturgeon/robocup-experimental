#pragma once

#ifdef NDEBUG
#error "Please don't use image IO in release mode <3"
#endif  // NDEBUG

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
// NOLINTBEGIN
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// NOLINTEND
#pragma clang diagnostic pop
