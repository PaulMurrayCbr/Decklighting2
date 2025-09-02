/*
 * optional.h
 *
 *  Created on: 2 Sept 2025
 *      Author: pmurray
 */

#ifndef SRC_LIB_OPTIONAL_HPP_
#define SRC_LIB_OPTIONAL_HPP_

#include <utility>  // for std::move
#include <stdexcept>

// C++11 doesn't have optional. Here's a simple implementation.

template<typename T>
class Optional {
private:
    bool has_value_;
    T value_;

public:
    Optional() :
            has_value_(false) {
    }
    Optional(const T &v) :
            has_value_(true), value_(v) {
    }
    Optional(T &&v) :
            has_value_(true), value_(std::move(v)) {
    }

    bool has_value() const {
        return has_value_;
    }

    T& value() {
        if (!has_value_)
            throw std::runtime_error("No value");
        return value_;
    }

    const T& value() const {
        if (!has_value_)
            throw std::runtime_error("No value");
        return value_;
    }

    void reset() {
        has_value_ = false;
    }

    Optional& operator=(const T &v) {
        value_ = v;
        has_value_ = true;
        return *this;
    }

    Optional& operator=(T &&v) {
        value_ = std::move(v);
        has_value_ = true;
        return *this;
    }

    T& operator*() {
        return value();
    }
    const T& operator*() const {
        return value();
    }
};

#endif /* SRC_LIB_OPTIONAL_HPP_ */
