/*
 * Pixel.cpp
 *
 *  Created on: 22 Aug 2025
 *      Author: pmurray
 */

#include <iostream>
#include <stdexcept>

#include "../core/common.hpp"
#include "../core/pixels.hpp"

extern "C" {
#include "/home/p/rpi_ws281x/ws2811.h"
}

#define TARGET_FREQ WS2811_TARGET_FREQ
#define GPIO_PIN    18
#define DMA         10
#define LED_COUNT   16
#define STRIP_TYPE  WS2811_STRIP_GRB

ws2811_t ledstring = { .freq = TARGET_FREQ, .dmanum = DMA, .channel = { [0] = { .gpionum = GPIO_PIN, .invert = 0, .count = NPIXELS, .strip_type = STRIP_TYPE, .brightness = 255, }, [1] = { 0 }, }, };

void init_pixels() {
    if (ws2811_init(&ledstring) != WS2811_SUCCESS) {
        throw new std::runtime_error("Failed to initialize ws2811");

    }
}

void clear_pixels(int start, int length) {
    for (int i = 0; i < length; i++) {
        ledstring.channel[0].leds[start + i] = 0;

    }
}

void set_pixel_global_brightness(int brightness) {
    ledstring.channel[0].brightness = brightness;
}

void set_pixel(int at, uint8_t r, uint8_t g, uint8_t b) {
    ledstring.channel[0].leds[at] = ((uint32_t) r << 16) | ((uint32_t) g << 8) | ((uint32_t) b << 0);
}

void send_pixels() {
    ws2811_render(&ledstring);
}

