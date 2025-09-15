// main.cpp
#include <atomic>
#include <thread>
#include <csignal>
#include <iostream>
#include <stdexcept>

#include "common.hpp"

#include "webserver.hpp"
#include "state.hpp"
#include "pixelloop.hpp"
#include "pixels.hpp"

//#ifdef __APPLE__
//namespace fs = std::__fs::filesystem;
//#else
//namespace fs = std::filesystem;
//#endif

static std::atomic<bool> g_stop { false };
void handle_signal(int) {
    g_stop.store(true);
}

int zzz(int pix, int of) {
    // it's not obvious, but this works for odds and for evens
    of = of / 2 + 1;
    if (pix >= of) {
        pix -= of;
        pix = of - pix - 2;
    }

    return pix;
}

int main_test_pixel_bookfolding() {
    for (int pix = 0; pix < 8; pix++) {
        std::cout << pix << ' ' << zzz(pix, 8) << '\n';
    }
    std::cout << '\n';
    for (int pix = 0; pix < 9; pix++) {
        std::cout << pix << ' ' << zzz(pix, 9) << '\n';
    }
    return 0;
}

int main_test_color_wheel() {
    HSV c;

    c.v = 100;
    c.s = 50;

    for (c.h = 0; c.h < 256 * 6; c.h++) {

        if (c.h % 256 == 0) {
            std::cout << "--------\n";

        }
        RGB r = hsv2rgb(c);
        std::cout << c.h << " | " << (int) r.r << ' ' << (int) r.g << ' ' << (int) r.b << '\n';
    }

    return 0;
}

int main_test_color_wheel_2() {
    int x = 0;

    for (int r = 0; r < 256; r++) {
        for (int g = 0; g < 256; g++) {
            for (int b = 0; b < 256; b++) {
                RGB c = RGB(r, g, b);
                HSV h = rgb2hsv(c);
                RGB c2 = hsv2rgb(h);

                if (std::abs(((int) c2.r) - ((int) c.r)) > 1) {
                    std::cout << //
                            ((int) c.r) << ' ' << ((int) c.g) << ' ' << ((int) c.b) << "\t| " << //
                            ((int) h.h) / 256 << '/' << ((int) h.h) % 256 << ' ' << ((int) h.s) << ' ' << ((int) h.v) << "\t| " << //
                            ((int) c2.r) << ' ' << ((int) c2.g) << ' ' << ((int) c2.b) << "\t| " << //
                            ((int) c2.r) - ((int) c.r) << ' ' << ((int) c2.g) - ((int) c.g) << ' ' << ((int) c2.b) - ((int) c.b) << "\t| " << //
                            '\n';

                    throw std::logic_error("red");
                }
                if (std::abs(((int) c2.g) - ((int) c.g)) > 1) {
                    std::cout << //
                            ((int) c.r) << ' ' << ((int) c.g) << ' ' << ((int) c.b) << "\t| " << //
                            ((int) h.h) / 256 << '/' << ((int) h.h) % 256 << ' ' << ((int) h.s) << ' ' << ((int) h.v) << "\t| " << //
                            ((int) c2.r) << ' ' << ((int) c2.g) << ' ' << ((int) c2.b) << "\t| " << //
                            ((int) c2.r) - ((int) c.r) << ' ' << ((int) c2.g) - ((int) c.g) << ' ' << ((int) c2.b) - ((int) c.b) << "\t| " << //
                            '\n';

                    throw std::logic_error("green");
                }
                if (std::abs(((int) c2.b) - ((int) c.b)) > 1) {
                    std::cout << //
                            ((int) c.r) << ' ' << ((int) c.g) << ' ' << ((int) c.b) << "\t| " << //
                            ((int) h.h) / 256 << '/' << ((int) h.h) % 256 << ' ' << ((int) h.s) << ' ' << ((int) h.v) << "\t| " << //
                            ((int) c2.r) << ' ' << ((int) c2.g) << ' ' << ((int) c2.b) << "\t| " << //
                            ((int) c2.r) - ((int) c.r) << ' ' << ((int) c2.g) - ((int) c.g) << ' ' << ((int) c2.b) - ((int) c.b) << "\t| " << //
                            '\n';

                    throw std::logic_error("blue");
                }

                x++;
            }
        }
    }
    std::cout << x << " values checked \n";

    return 0;
}

int main_test_color_wheel_3() {
    int x = 0;

    for (int r = 0; r < 256; r++) {
        for (int g = 0; g < 256; g++) {
            for (int b = 0; b < 256; b++) {
                RGB c = RGB(r, g, b);
                HSQ h = rgb2hsq(c);
                RGB c2 = hsq2rgb(h);

                try {
                    if (std::abs(((int) c2.r) - ((int) c.r)) > 1) {
                        throw std::logic_error("red");
                    }
                    if (std::abs(((int) c2.g) - ((int) c.g)) > 1) {
                        throw std::logic_error("green");
                    }
                    if (std::abs(((int) c2.b) - ((int) c.b)) > 1) {
                        throw std::logic_error("blue");
                    }

                    if (h.h < 0 || h.h > APPARENT_BRIGHTNESS_SCALE * 6) {
                        throw std::logic_error("hue");
                    }
                    if (h.v < 0 || h.v > APPARENT_BRIGHTNESS_SCALE) {
                        throw std::logic_error("brite");
                    }
                    if (h.s < 0 || h.s > h.v) {
                        throw std::logic_error("sat");
                    }
                } catch (std::logic_error ex) {
                    std::cout << //
                            "init " << ((int) c.r) << ' ' << ((int) c.g) << ' ' << ((int) c.b) << "\t| " << //
                            "hsq " << ((int) h.h) / APPARENT_BRIGHTNESS_SCALE << '/' << ((int) h.h) % APPARENT_BRIGHTNESS_SCALE << ' ' << ((int) h.s) << ' ' << ((int) h.v) << "\t| " << //
                            "roundtrip " << ((int) c2.r) << ' ' << ((int) c2.g) << ' ' << ((int) c2.b) << "\t| " << //
                            "diff " << ((int) c2.r) - ((int) c.r) << ' ' << ((int) c2.g) - ((int) c.g) << ' ' << ((int) c2.b) - ((int) c.b) << "\t| " << //
                            '\n';

                    throw ex;
                }

                x++;
            }
        }
    }
    std::cout << x << " values checked \n";

    return 0;
}

extern HSQ TEST_INTERP(HSQ a, HSQ b, double relative);

int main_TEST_INTERP() {

    HSQ a = HSQ(APPARENT_BRIGHTNESS_SCALE, APPARENT_BRIGHTNESS_SCALE * 5, APPARENT_BRIGHTNESS_SCALE * 5);
    HSQ b = HSQ(APPARENT_BRIGHTNESS_SCALE * 4, APPARENT_BRIGHTNESS_SCALE * 5, APPARENT_BRIGHTNESS_SCALE * 5);

    for (double relative = .66; relative <= .67; relative += .001) {
        HSQ z = TEST_INTERP(b, a, relative);
        std::cout << relative << "\t" << z << "\t" << hsq2rgb(z) << "\n";
    }

    return 0;
}

int main() {
// === Config ===
//    const int GPIO_PIN = 18;      // PWM0 (requires level shifter to 5V on data)
//    const int LED_COUNT = 60;

    std::cout << "Starting ...\n";

    for (SectionState &s : sharedState.section) {
        s.mode = SectionMode::on;
        s.touched = true;
        s.needsRepaint = true;
        s.density = 1;
        for (ColorRangeState &cr : s.colors) {
            cr.animation.frameDuration = std::chrono::milliseconds(static_cast<int64_t>(100));
            cr.from = RGB(255, 0, 0);
            cr.to = RGB(0, 255, 0);
            cr.fromHsq = rgb2hsq(cr.from);
            cr.toHsq = rgb2hsq(cr.to);
            cr.interpolation = RgbInterpolationType::FADE;
        }
    }

    for (SectionState &s : sharedState.section) {
        s.mode = SectionMode::out;
    }
    sharedState.on = true;
    sharedState.brightness = 16;
    sharedState.section[0].mode = SectionMode::on;
    sharedState.section[0].effect = SectionEffectType::GRADIENT;
    sharedState.section[0].colors[0].animation.animating = true;

    sharedState.touched = true;
    sharedState.needsRepaint = true;

    recompute_sections();

    init_pixels();

    start_webserver();
    start_pixelloop();

// Graceful shutdown
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "Started ...\n";

// Poll for stop signal
    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Stopping ...\n";

    stop_pixelloop();
    stop_webserver();

    std::cout << "Stopped.\n";
    return 0;
}
