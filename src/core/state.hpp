/**
 * Data structures used for communicating state from the http server to the pixels.
 *
 */

#ifndef SRC_CORE_STATE_HPP
#define SRC_CORE_STATE_HPP

#include <cstdint>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <map>
#include <functional>

#import "../lib/json.hpp"
using json = nlohmann::json;

#import "common.hpp"

struct AnimationState {
    bool animating;
    std::chrono::milliseconds frameDuration;
    std::chrono::time_point<std::chrono::steady_clock> previousFrameTime;

    void setFrameDuration(double ms) {
        frameDuration = std::chrono::milliseconds(static_cast<int64_t>(ms));
    }

    void restart() {
        previousFrameTime = std::chrono::steady_clock::now() - frameDuration;
    }

    /** note that this method sets the current frame time if it returns true
     * We do this immediately so that the frame rate is not affected by the time i takes to render the animation
     */

    bool nextFrameReady() {
        auto now = std::chrono::steady_clock::now();

        if (now - previousFrameTime >= frameDuration) {
            previousFrameTime = now;
            return true;
        } else {
            return false;
        }
    }
};

// dont need this here
//class Animator {
//	virtual void animate() = 0;
//};
//
//#define EFFECT(name) class Animate_##name : Animator;
//    EFFECT_LIST
//#undef EFFECT

struct EffectSOLIDState {
};

struct EffectTHEATREState {
};

struct ColorRangeState {
    AnimationState animation;
    RGB from;
    RGB to;
    double midpoint; // >0 to <1 default .5 Solve toget the quadratic coeficients
    bool seamless;
    std::chrono::milliseconds cycleSpeed;
};

struct SectionState {
    SectionMode mode;
    int brightness;
    int density;
    bool touched;
    bool needsRepaint;

    SectionEffectType effect;

    ColorRangeState colors[NCOLORANGES];

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
    bool needsRepaint;

    struct SectionState section[NSECTIONS];

};

extern GlobalState sharedState;
extern std::mutex sharedStateMutex;
extern json getGlobalState();
extern json getSectionState(Section section);

inline void inSharedStateMutex(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(sharedStateMutex);
    f();
}

#endif
