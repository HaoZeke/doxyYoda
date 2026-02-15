/**
 * @file containers.hpp
 * @brief Template containers demonstrating generic programming documentation.
 *
 * Exercises Doxygen's template parameter docs, exception classes,
 * cross-file references, and non-type template parameters.
 *
 * @author Rohit Goswami
 * @copyright Apache License 2.0
 */

#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

/**
 * @defgroup containers Container Utilities
 * @brief Reusable generic containers and result types.
 *
 * This module showcases doxyYoda's rendering of:
 * - Template classes with type and non-type parameters
 * - Exception hierarchies
 * - `std::variant`-based sum types
 * - Cross-file `@see` references
 *
 * @{
 */

namespace containers {

// ─── Exceptions ──────────────────────────────────────────────────────────────

/**
 * @brief Base exception for container errors.
 *
 * All container-specific exceptions derive from this class,
 * which in turn derives from `std::runtime_error`.
 *
 * @see BufferOverflow, BufferUnderflow
 */
class ContainerError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Thrown when a fixed-capacity container is full.
 *
 * @see RingBuffer
 */
class BufferOverflow : public ContainerError {
public:
    BufferOverflow() : ContainerError("buffer overflow: container is full") {}
};

/**
 * @brief Thrown when popping from an empty container.
 *
 * @see Stack, RingBuffer
 */
class BufferUnderflow : public ContainerError {
public:
    BufferUnderflow() : ContainerError("buffer underflow: container is empty") {}
};

// ─── Stack ───────────────────────────────────────────────────────────────────

/**
 * @brief A simple LIFO stack backed by `std::vector`.
 *
 * @tparam T Element type.
 *
 * @note This is intentionally simple — it demonstrates how doxyYoda
 *       renders template class documentation, not a production container.
 *
 * @see RingBuffer for a fixed-capacity alternative.
 * @see yoda::Vec2 for another template class example.
 */
template <typename T>
class Stack {
public:
    /**
     * @brief Push a value onto the top of the stack.
     * @param value The value to push.
     * @post `size()` is incremented by 1.
     */
    void push(const T& value) {
        data_.push_back(value);
    }

    /**
     * @brief Remove and return the top element.
     * @return The top element.
     * @throws BufferUnderflow if the stack is empty.
     * @pre `!empty()`
     */
    T pop() {
        if (data_.empty()) throw BufferUnderflow();
        T val = data_.back();
        data_.pop_back();
        return val;
    }

    /**
     * @brief Peek at the top element without removing it.
     * @return A const reference to the top element.
     * @throws BufferUnderflow if the stack is empty.
     */
    [[nodiscard]] const T& top() const {
        if (data_.empty()) throw BufferUnderflow();
        return data_.back();
    }

    /// @brief Check if the stack is empty.
    [[nodiscard]] bool empty() const { return data_.empty(); }

    /// @brief Get the number of elements.
    [[nodiscard]] std::size_t size() const { return data_.size(); }

    /// @brief Remove all elements.
    void clear() { data_.clear(); }

private:
    std::vector<T> data_; ///< Underlying storage.
};

// ─── RingBuffer ──────────────────────────────────────────────────────────────

/**
 * @brief A fixed-capacity circular buffer.
 *
 * @tparam T Element type.
 * @tparam N Maximum capacity (compile-time constant).
 *
 * Uses a head/tail index scheme:
 * \f[
 *   \text{next}(i) = (i + 1) \bmod N
 * \f]
 *
 * @warning Writing to a full buffer throws BufferOverflow.
 *          Use `full()` to check before writing.
 *
 * @todo Add an overwrite-on-full policy as a template parameter.
 */
template <typename T, std::size_t N>
class RingBuffer {
public:
    RingBuffer() = default;

    /**
     * @brief Write a value to the buffer.
     * @param value The value to enqueue.
     * @throws BufferOverflow if the buffer is full.
     */
    void write(const T& value) {
        if (full()) throw BufferOverflow();
        buf_[tail_] = value;
        tail_ = (tail_ + 1) % N;
        ++size_;
    }

    /**
     * @brief Read and remove the oldest value.
     * @return The oldest value.
     * @throws BufferUnderflow if the buffer is empty.
     */
    T read() {
        if (empty()) throw BufferUnderflow();
        T val = buf_[head_];
        head_ = (head_ + 1) % N;
        --size_;
        return val;
    }

    /// @brief Check if the buffer is empty.
    [[nodiscard]] bool empty() const { return size_ == 0; }

    /// @brief Check if the buffer is at capacity.
    [[nodiscard]] bool full() const { return size_ == N; }

    /// @brief Get the number of elements currently stored.
    [[nodiscard]] std::size_t size() const { return size_; }

    /// @brief Get the maximum capacity.
    [[nodiscard]] constexpr std::size_t capacity() const { return N; }

private:
    std::array<T, N> buf_{}; ///< Fixed-size backing array.
    std::size_t head_ = 0;   ///< Read index.
    std::size_t tail_ = 0;   ///< Write index.
    std::size_t size_ = 0;   ///< Current element count.
};

// ─── Result ──────────────────────────────────────────────────────────────────

/**
 * @brief A sum type representing either a success value or an error.
 *
 * Inspired by Rust's `Result<T, E>`. Uses `std::variant` internally.
 *
 * @tparam T The success type.
 * @tparam E The error type (default: `std::string`).
 *
 * Example:
 * @code
 * Result<int, std::string> divide(int a, int b) {
 *     if (b == 0) return Result<int, std::string>::err("division by zero");
 *     return Result<int, std::string>::ok(a / b);
 * }
 * @endcode
 *
 * @see Stack, RingBuffer
 */
template <typename T, typename E = std::string>
class Result {
public:
    /**
     * @brief Create a success result.
     * @param value The success value.
     * @return A Result containing the value.
     */
    static Result ok(T value) { return Result(std::move(value)); }

    /**
     * @brief Create an error result.
     * @param error The error value.
     * @return A Result containing the error.
     */
    static Result err(E error) { return Result(ErrTag{}, std::move(error)); }

    /// @brief Check if this result is a success.
    [[nodiscard]] bool is_ok() const {
        return std::holds_alternative<T>(data_);
    }

    /// @brief Check if this result is an error.
    [[nodiscard]] bool is_err() const { return !is_ok(); }

    /**
     * @brief Unwrap the success value.
     * @return The contained value.
     * @throws std::bad_variant_access if this is an error.
     */
    [[nodiscard]] const T& value() const { return std::get<T>(data_); }

    /**
     * @brief Unwrap the error.
     * @return The contained error.
     * @throws std::bad_variant_access if this is a success.
     */
    [[nodiscard]] const E& error() const { return std::get<E>(data_); }

private:
    struct ErrTag {};
    explicit Result(T value) : data_(std::move(value)) {}
    Result(ErrTag, E error) : data_(std::move(error)) {}

    std::variant<T, E> data_; ///< Variant storage for value or error.
};

} // namespace containers

/** @} */ // end of containers group
