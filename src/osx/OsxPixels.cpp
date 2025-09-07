/*
 * Pixel.cpp
 *
 *  Created on: 22 Aug 2025
 *      Author: pmurray
 */

#include <iostream>

#include "../core/pixels.hpp"

namespace {
    RGB out[NPIXELS];
}

void init_pixels() {

}

void clear_pixels(int start, int length) {
    for (int i = start; i < start + length; i++) {
        out[i].r = 0;
        out[i].g = 0;
        out[i].b = 0;
    }
}

void set_pixel_global_brightness(int brightness) {
}

void set_pixel(int at, uint8_t r, uint8_t g, uint8_t b) {
    out[at].r = r;
    out[at].g = g;
    out[at].b = b;
}

void send_pixels() {
    std::cout << '|';
    for (int i = 0; i < NPIXELS; i++) {
        std::cout << (int) out[i].r << '/' << (int) out[i].g << '/' << (int) out[i].b << '/' << '|';
    }
    std::cout << '\n';
}

