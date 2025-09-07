/**
 * Data structures used for communicating state from the http server to the pixels.
 *
 */

#ifndef SRC_CORE_STATE_HPP
#define SRC_CORE_STATE_HPP

#include <cstdint>
#include <chrono>
#include <mutex>
#include <map>
#include <functional>
#include <utility>

#include "../lib/json.hpp"
using json = nlohmann::json;

#include "common.hpp"

struct AnimationState {
    bool animating;
    std::chrono::milliseconds frameDuration = std::chrono::milliseconds(100);
    std::chrono::time_point<std::chrono::steady_clock> previousFrameTime;
    int cycleSpeed = 1000;
    int currentCyclePos;

    void setAnimating(bool a) {
        animating = a;
        restart();
    }

    void setFrameDuration(int ms) {
        frameDuration = std::chrono::milliseconds(static_cast<int64_t>(ms < 50 ? 50 : ms));
        restart();
    }

    // cycle speed can be less than the fram duration, which kinda randomises
    // where the cycle is each frame
    void setCycleSpeed(int ms) {
        cycleSpeed = ms < 1 ? 1 : ms;
        restart();
    }

    void restart() {
        previousFrameTime = std::chrono::steady_clock::now() - frameDuration;
        currentCyclePos = 0;
    }

    /** note that this method sets the current frame time if it returns true
     * We do this immediately so that the frame rate is not affected by the time i takes to render the animation
     */

    bool nextFrameReady() {
        if (!animating) {
            return false;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - previousFrameTime;

        if (elapsed >= frameDuration) {
            previousFrameTime = now;

            int ms = (int) std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
            currentCyclePos += ms;
            while (currentCyclePos > cycleSpeed)
                currentCyclePos -= cycleSpeed;

            return true;
        } else {
            return false;
        }
    }
};

struct EffectSOLIDState {
};

struct EffectGRADIENTState {
};

struct EffectTHEATREState {
};

struct ColorRangeState {
    AnimationState animation;
    RGB from;
    RGB to;
    HSV fromHsv;
    HSV toHsv;
    RgbInterpolationType interpolation = RgbInterpolationType::FADE;
    double midpoint; // >0 to <1 default .5 Solve toget the quadratic coeficients
    bool seamless;
};

struct SectionState {
    SectionMode mode;
    int density;
    bool touched;
    bool needsRepaint;

    int start; // this is redundant, but it's neater to have it here.
    int length; // this gets computed when a section is brought in or taken out. And at startup, obviously.

    SectionEffectType effect;

    ColorRangeState colors[NCOLORANGES];

    inline int npixels() {
        return (length - 1) / density + 1;
    }

    inline int pixel(int i) {
        return i * density + start;
    }

    union {
#define EFFECT(name) Effect##name##State name;
        EFFECT_TYPE_LIST
#undef EFFECT
    } effectState;
};

struct GlobalState {
    bool on;
    int brightness;
    bool touched;
    bool needsRepaint = true;

    struct SectionState section[NSECTIONS];

};

extern GlobalState sharedState;
extern std::mutex sharedStateMutex;
extern json getGlobalState();
extern json getSectionState(Section section);

// C++ weirdness, I have no idea what these ampersands do, or std:forward, or any of this stuff.

template<typename F>
inline auto inSharedStateMutex(F &&f) -> decltype(std::forward<F>(f)()) {
    std::lock_guard<std::mutex> lock(sharedStateMutex);
    return std::forward<F>(f)();
}

extern void recompute_sections();

#endif
