/**
 * Data structures used for communicating state from the http server to the pixels.
 */

#ifndef STATE_HPP
#define STATE_HPP

#include <cstdint>

enum class Mode { Off, Solid };

struct Command {
    Mode mode{Mode::Off};
    uint8_t r{0}, g{0}, b{0};
    uint8_t brightness{255};
};

#endif
