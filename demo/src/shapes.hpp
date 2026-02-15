/**
 * @file shapes.hpp
 * @brief Geometry shapes demonstrating class inheritance and Graphviz diagrams.
 *
 * This file exercises Doxygen's class hierarchy features: inheritance graphs,
 * collaboration diagrams, pure virtual methods, and the `@defgroup` system.
 *
 * @author Rohit Goswami
 * @copyright Apache License 2.0
 */

#pragma once

#include <cmath>
#include <string>
#include <memory>
#include <vector>

/**
 * @defgroup shapes Geometry Shapes
 * @brief A hierarchy of 2D shapes with area and perimeter calculations.
 *
 * This module demonstrates doxyYoda's rendering of:
 * - Class inheritance diagrams (Graphviz)
 * - Collaboration graphs
 * - Pure virtual functions
 * - Math formulas per shape
 * - Group pages with nested compounds
 *
 * @{
 */

namespace geometry {

/**
 * @brief Abstract base class for all 2D shapes.
 *
 * Every shape must provide its area, perimeter, and a human-readable name.
 * The Shape class also tracks the total number of shape instances created
 * (demonstrating static member documentation).
 *
 * @note Shapes are non-copyable by design. Use `std::unique_ptr<Shape>`
 *       for polymorphic ownership.
 *
 * @see Circle, Rectangle, Triangle, Square
 */
class Shape {
public:
    /// @brief Virtual destructor for safe polymorphic deletion.
    virtual ~Shape() = default;

    /**
     * @brief Compute the area of the shape.
     * @return The area in square units.
     */
    [[nodiscard]] virtual double area() const = 0;

    /**
     * @brief Compute the perimeter of the shape.
     * @return The perimeter in linear units.
     */
    [[nodiscard]] virtual double perimeter() const = 0;

    /**
     * @brief Return the human-readable name of the shape.
     * @return A string like `"Circle"`, `"Rectangle"`, etc.
     */
    [[nodiscard]] virtual std::string name() const = 0;

    /**
     * @brief Pretty-print shape information.
     * @return A formatted string: `"ShapeName(area=..., perimeter=...)"`.
     */
    [[nodiscard]] std::string describe() const {
        return name() + "(area=" + std::to_string(area())
               + ", perimeter=" + std::to_string(perimeter()) + ")";
    }

    /**
     * @brief Get the total count of Shape instances created.
     * @return The running total.
     */
    static int instance_count() { return instance_count_; }

protected:
    /// @brief Increment the instance counter.
    Shape() { ++instance_count_; }

private:
    static inline int instance_count_ = 0; ///< Running total of instances.
};

/**
 * @brief A circle defined by its radius.
 *
 * Area and perimeter are computed as:
 * \f[
 *   A = \pi r^2, \qquad P = 2\pi r
 * \f]
 *
 * @see Shape
 */
class Circle : public Shape {
public:
    /**
     * @brief Construct a circle with the given radius.
     * @param radius The radius (must be positive).
     * @pre \f$ r > 0 \f$
     */
    explicit Circle(double radius) : radius_(radius) {}

    [[nodiscard]] double area() const override {
        return M_PI * radius_ * radius_;
    }

    [[nodiscard]] double perimeter() const override {
        return 2.0 * M_PI * radius_;
    }

    [[nodiscard]] std::string name() const override { return "Circle"; }

    /// @brief Get the radius.
    [[nodiscard]] double radius() const { return radius_; }

private:
    double radius_; ///< The circle's radius.
};

/**
 * @brief A rectangle defined by width and height.
 *
 * \f[
 *   A = w \times h, \qquad P = 2(w + h)
 * \f]
 *
 * @see Square (a special case where \f$w = h\f$)
 */
class Rectangle : public Shape {
public:
    /**
     * @brief Construct a rectangle.
     * @param width  The width (must be positive).
     * @param height The height (must be positive).
     */
    Rectangle(double width, double height) : width_(width), height_(height) {}

    [[nodiscard]] double area() const override {
        return width_ * height_;
    }

    [[nodiscard]] double perimeter() const override {
        return 2.0 * (width_ + height_);
    }

    [[nodiscard]] std::string name() const override { return "Rectangle"; }

    /// @brief Get the width.
    [[nodiscard]] double width() const { return width_; }
    /// @brief Get the height.
    [[nodiscard]] double height() const { return height_; }

protected:
    double width_;  ///< The rectangle's width.
    double height_; ///< The rectangle's height.
};

/**
 * @brief A square — a special case of Rectangle where width equals height.
 *
 * \f[
 *   A = s^2, \qquad P = 4s
 * \f]
 *
 * @note This class inherits from Rectangle and overrides `name()`.
 *       It demonstrates a deeper inheritance chain for the class diagram.
 */
class Square : public Rectangle {
public:
    /**
     * @brief Construct a square with the given side length.
     * @param side The side length (must be positive).
     */
    explicit Square(double side) : Rectangle(side, side) {}

    [[nodiscard]] std::string name() const override { return "Square"; }

    /// @brief Get the side length.
    [[nodiscard]] double side() const { return width_; }
};

/**
 * @brief A triangle defined by its three side lengths.
 *
 * The area is computed via Heron's formula:
 * \f[
 *   s = \frac{a + b + c}{2}, \qquad A = \sqrt{s(s-a)(s-b)(s-c)}
 * \f]
 *
 * @warning The triangle inequality must hold: each side must be less than
 *          the sum of the other two.
 */
class Triangle : public Shape {
public:
    /**
     * @brief Construct a triangle from three side lengths.
     * @param a First side.
     * @param b Second side.
     * @param c Third side.
     * @pre \f$ a + b > c \f$ and \f$ b + c > a \f$ and \f$ a + c > b \f$
     */
    Triangle(double a, double b, double c) : a_(a), b_(b), c_(c) {}

    [[nodiscard]] double area() const override {
        double s = (a_ + b_ + c_) / 2.0;
        return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
    }

    [[nodiscard]] double perimeter() const override {
        return a_ + b_ + c_;
    }

    [[nodiscard]] std::string name() const override { return "Triangle"; }

private:
    double a_; ///< First side length.
    double b_; ///< Second side length.
    double c_; ///< Third side length.
};

/**
 * @brief Create a container of example shapes for demonstration.
 *
 * @return A vector of unique pointers to various shapes.
 *
 * Example usage:
 * @code
 * auto shapes = geometry::make_demo_shapes();
 * for (const auto& s : shapes) {
 *     std::cout << s->describe() << "\n";
 * }
 * @endcode
 */
inline std::vector<std::unique_ptr<Shape>> make_demo_shapes() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Square>(3.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));
    return shapes;
}

} // namespace geometry

/** @} */ // end of shapes group
