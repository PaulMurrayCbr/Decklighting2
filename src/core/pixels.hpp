#ifndef SRC_CORE_PIXELS_HPP
#define SRC_CORE_PIXELS_HPP

#include "common.hpp"

extern void clear_pixels(int start, int length);
extern void set_pixel_global_brightness(int brightness);
extern void set_pixel(int at, uint8_t r, uint8_t g, uint8_t b);
extern void send_pixels();

#endif
