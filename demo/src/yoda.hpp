/**
 * @file yoda.hpp
 * @brief A showcase header for the doxyYoda Doxygen theme.
 *
 * This file demonstrates the visual styling of various Doxygen constructs
 * as rendered by the doxyYoda theme — code blocks, math, admonitions,
 * member documentation, templates, and more.
 *
 * @author Rohit Goswami
 * @version 0.1.0
 * @date 2024
 * @copyright Apache License 2.0
 */

#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <functional>

/**
 * @brief The Yoda namespace — core utilities for the doxyYoda demo.
 *
 * This namespace contains example classes, functions, and types that
 * exercise every major Doxygen documentation construct.
 *
 * @see geometry for shape hierarchy examples.
 * @see containers for template container examples.
 * @see algorithms for grouped free functions.
 */
namespace yoda {

/**
 * @brief Color palette constants from the Solarized scheme.
 *
 * These are the canonical Solarized colors as defined by
 * Ethan Schoonover. They form the basis of doxyYoda's visual identity.
 *
 * @note The hex values here match the CSS custom properties in
 *       `_variables.scss`.
 *
 * @see https://ethanschoonover.com/solarized/
 */
namespace palette {
    constexpr unsigned int base03  = 0x002b36; ///< Darkest background
    constexpr unsigned int base02  = 0x073642; ///< Dark background
    constexpr unsigned int base01  = 0x586e75; ///< Optional emphasized content
    constexpr unsigned int base00  = 0x657b83; ///< Body text (dark mode)
    constexpr unsigned int base0   = 0x839496; ///< Body text (light mode)
    constexpr unsigned int base1   = 0x93a1a1; ///< Optional de-emphasized content
    constexpr unsigned int base2   = 0xeee8d5; ///< Light background highlights
    constexpr unsigned int base3   = 0xfdf6e3; ///< Lightest background

    constexpr unsigned int yellow  = 0xb58900; ///< Accent: yellow
    constexpr unsigned int orange  = 0xcb4b16; ///< Accent: orange
    constexpr unsigned int red     = 0xdc322f; ///< Accent: red
    constexpr unsigned int magenta = 0xd33682; ///< Accent: magenta
    constexpr unsigned int violet  = 0x6c71c4; ///< Accent: violet
    constexpr unsigned int blue    = 0x268bd2; ///< Accent: blue
    constexpr unsigned int cyan    = 0x2aa198; ///< Accent: cyan
    constexpr unsigned int green   = 0x859900; ///< Accent: green
} // namespace palette

/**
 * @brief A 2D vector with common geometric operations.
 *
 * Demonstrates template classes, operator overloading, and mathematical
 * documentation with MathJax.
 *
 * The magnitude of a vector \f$(x, y)\f$ is computed as:
 * \f[
 *   \| \mathbf{v} \| = \sqrt{x^2 + y^2}
 * \f]
 *
 * @tparam T Numeric type (must support arithmetic operators).
 *
 * @warning Normalization of a zero-length vector is undefined behavior.
 */
template <typename T>
class Vec2 {
public:
    T x; ///< X component
    T y; ///< Y component

    /**
     * @brief Construct a new Vec2.
     * @param x The x component (default: 0).
     * @param y The y component (default: 0).
     */
    Vec2(T x = T{}, T y = T{}) : x(x), y(y) {}

    /**
     * @brief Compute the Euclidean magnitude.
     * @return The length of the vector.
     *
     * @pre The type T must be convertible to `double`.
     */
    [[nodiscard]] double magnitude() const {
        return std::sqrt(static_cast<double>(x * x + y * y));
    }

    /**
     * @brief Return a normalized (unit-length) copy of this vector.
     *
     * @attention The result is undefined if `magnitude() == 0`.
     *
     * @return A new Vec2 with unit length.
     */
    [[nodiscard]] Vec2<double> normalized() const {
        double m = magnitude();
        return {static_cast<double>(x) / m, static_cast<double>(y) / m};
    }

    /**
     * @brief Compute the dot product of two vectors.
     *
     * The dot product is defined as:
     * \f[
     *   \mathbf{a} \cdot \mathbf{b} = a_x b_x + a_y b_y
     * \f]
     *
     * @param other The other vector.
     * @return The scalar dot product.
     */
    [[nodiscard]] T dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    /**
     * @brief Vector addition.
     * @param rhs Right-hand side operand.
     * @return The component-wise sum.
     */
    Vec2 operator+(const Vec2& rhs) const {
        return {x + rhs.x, y + rhs.y};
    }

    /**
     * @brief Scalar multiplication.
     * @param scalar The scalar multiplier.
     * @return The scaled vector.
     */
    Vec2 operator*(T scalar) const {
        return {x * scalar, y * scalar};
    }
};

/// @brief Convenience alias for a double-precision 2D vector.
using Vec2d = Vec2<double>;

/// @brief Convenience alias for an integer 2D vector.
using Vec2i = Vec2<int>;

/**
 * @brief Interpolation methods available for smoothing operations.
 *
 * Demonstrates enum documentation with per-enumerator descriptions.
 */
enum class Interpolation {
    Linear,      ///< Simple linear interpolation: \f$ f(t) = a + t(b - a) \f$
    Cosine,      ///< Cosine-smoothed interpolation
    Cubic,       ///< Cubic Hermite interpolation
    CatmullRom   ///< Catmull-Rom spline interpolation
};

/**
 * @brief Linearly interpolate between two values.
 *
 * Computes:
 * \f[
 *   \text{lerp}(a, b, t) = (1 - t) \cdot a + t \cdot b
 * \f]
 *
 * @param a Start value.
 * @param b End value.
 * @param t Interpolation factor in \f$[0, 1]\f$.
 * @return The interpolated value.
 *
 * @invariant The result lies in \f$[a, b]\f$ when \f$t \in [0, 1]\f$.
 *
 * @todo Add clamped variant that enforces \f$t \in [0, 1]\f$.
 */
inline double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

/**
 * @brief Apply a function element-wise to a vector of values.
 *
 * @tparam T Element type.
 * @param values Input values.
 * @param fn The transformation function.
 * @return A new vector with `fn` applied to each element.
 *
 * Example usage:
 * @code
 * auto squared = map({1.0, 2.0, 3.0}, [](double x) { return x * x; });
 * // squared == {1.0, 4.0, 9.0}
 * @endcode
 *
 * @deprecated Use `std::ranges::transform` with C++20 instead.
 */
template <typename T>
std::vector<T> map(const std::vector<T>& values,
                   std::function<T(T)> fn) {
    std::vector<T> result;
    result.reserve(values.size());
    for (const auto& v : values) {
        result.push_back(fn(v));
    }
    return result;
}

/**
 * @brief Find the first element satisfying a predicate.
 *
 * @tparam T Element type.
 * @param values The input container.
 * @param pred A unary predicate.
 * @return The first matching element, or `std::nullopt`.
 *
 * @bug Does not short-circuit on the first match if compiled
 *      without optimizations on some compilers.
 */
template <typename T>
std::optional<T> find_if(const std::vector<T>& values,
                         std::function<bool(const T&)> pred) {
    for (const auto& v : values) {
        if (pred(v)) return v;
    }
    return std::nullopt;
}

/**
 * @brief A simple RAII-based timer for benchmarking.
 *
 * Demonstrates nested types, constructors/destructors, and
 * the `@test` directive.
 *
 * @test Verify that elapsed time is non-negative after construction.
 */
class ScopedTimer {
public:
    /// @brief Timer resolution units.
    enum class Unit {
        Seconds,      ///< Report in seconds
        Milliseconds, ///< Report in milliseconds
        Microseconds  ///< Report in microseconds
    };

    /**
     * @brief Start the timer with a label.
     * @param label A human-readable name for this timing region.
     * @param unit  The resolution to report (default: milliseconds).
     */
    explicit ScopedTimer(std::string label, Unit unit = Unit::Milliseconds);

    /**
     * @brief Stop the timer and print elapsed time.
     *
     * @post The elapsed time has been printed to `stdout`.
     */
    ~ScopedTimer();

    /// @brief Get the label for this timer.
    [[nodiscard]] const std::string& label() const { return label_; }

private:
    std::string label_; ///< Timer label
    Unit unit_;         ///< Reporting unit
};

} // namespace yoda
