/**
 * @file algorithms.hpp
 * @brief Numeric and sorting algorithms demonstrating function group docs.
 *
 * Exercises Doxygen's `@defgroup` for free functions, complexity
 * annotations with MathJax, cross-references, and inline source browsing.
 *
 * @author Rohit Goswami
 * @copyright Apache License 2.0
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

/**
 * @defgroup algorithms Algorithms
 * @brief Sorting and numeric algorithms with complexity analysis.
 *
 * This module demonstrates doxyYoda's rendering of:
 * - Grouped free functions
 * - Complexity analysis with MathJax
 * - `@see` cross-references between groups
 * - Inline source code browsing
 *
 * @{
 */

namespace algorithms {

// ─── Sorting ─────────────────────────────────────────────────────────────────

/**
 * @brief Sort a vector in-place using bubble sort.
 *
 * Repeatedly steps through the list, swaps adjacent elements that are
 * out of order, and repeats until no swaps are needed.
 *
 * **Complexity:**
 * \f[
 *   \text{Time: } O(n^2), \qquad \text{Space: } O(1)
 * \f]
 *
 * @tparam T Element type (must support `operator<`).
 * @param data The vector to sort.
 *
 * @note This is for demonstration only. Use `std::sort` in production.
 *
 * @see insertion_sort for a slightly better quadratic sort.
 * @see containers::Stack for a container that could hold sorted results.
 */
template <typename T>
void bubble_sort(std::vector<T>& data) {
    bool swapped;
    for (std::size_t i = 0; i < data.size(); ++i) {
        swapped = false;
        for (std::size_t j = 0; j + 1 < data.size() - i; ++j) {
            if (data[j + 1] < data[j]) {
                std::swap(data[j], data[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

/**
 * @brief Sort a vector in-place using insertion sort.
 *
 * Builds the sorted portion one element at a time by inserting each
 * new element into its correct position.
 *
 * **Complexity:**
 * \f[
 *   \text{Best: } O(n), \quad \text{Average/Worst: } O(n^2), \quad \text{Space: } O(1)
 * \f]
 *
 * @tparam T Element type (must support `operator<`).
 * @param data The vector to sort.
 *
 * @attention Insertion sort is efficient for small or nearly-sorted data.
 *            For large datasets, prefer `std::sort` (\f$O(n \log n)\f$).
 */
template <typename T>
void insertion_sort(std::vector<T>& data) {
    for (std::size_t i = 1; i < data.size(); ++i) {
        T key = data[i];
        std::size_t j = i;
        while (j > 0 && key < data[j - 1]) {
            data[j] = data[j - 1];
            --j;
        }
        data[j] = key;
    }
}

// ─── Numeric ─────────────────────────────────────────────────────────────────

/**
 * @brief Compute the \f$n\f$-th Fibonacci number.
 *
 * Uses the recurrence:
 * \f[
 *   F(n) = F(n-1) + F(n-2), \quad F(0) = 0, \; F(1) = 1
 * \f]
 *
 * Implemented iteratively for \f$O(n)\f$ time and \f$O(1)\f$ space.
 *
 * @param n The index (0-based).
 * @return \f$F(n)\f$.
 *
 * @invariant The result is always non-negative.
 *
 * @bug Overflows for `n > 93` when using `uint64_t`.
 */
inline std::uint64_t fibonacci(unsigned n) {
    if (n <= 1) return n;
    std::uint64_t a = 0, b = 1;
    for (unsigned i = 2; i <= n; ++i) {
        std::uint64_t c = a + b;
        a = b;
        b = c;
    }
    return b;
}

/**
 * @brief Compute the greatest common divisor of two integers.
 *
 * Uses the Euclidean algorithm:
 * \f[
 *   \gcd(a, 0) = a, \qquad \gcd(a, b) = \gcd(b, \; a \bmod b)
 * \f]
 *
 * @param a First integer.
 * @param b Second integer.
 * @return \f$\gcd(a, b)\f$.
 *
 * @pre Both arguments should be non-negative.
 * @post The return value divides both `a` and `b`.
 *
 * @deprecated Use `std::gcd` from `<numeric>` (C++17) instead.
 */
inline unsigned gcd(unsigned a, unsigned b) {
    while (b != 0) {
        unsigned t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/**
 * @brief Test whether a number is prime.
 *
 * Uses trial division up to \f$\sqrt{n}\f$:
 * \f[
 *   \text{Time: } O(\sqrt{n})
 * \f]
 *
 * @param n The number to test.
 * @return `true` if `n` is prime, `false` otherwise.
 *
 * @test Verify edge cases: `is_prime(0) == false`, `is_prime(1) == false`,
 *       `is_prime(2) == true`, `is_prime(97) == true`.
 */
inline bool is_prime(unsigned n) {
    if (n < 2) return false;
    if (n < 4) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (unsigned i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

/**
 * @brief Compute the sum of all elements in a range.
 *
 * @tparam T Numeric type.
 * @param data The input vector.
 * @return The sum \f$\sum_{i=0}^{n-1} \text{data}[i]\f$.
 *
 * @see yoda::Vec2::dot for a related inner-product operation.
 */
template <typename T>
T sum(const std::vector<T>& data) {
    return std::accumulate(data.begin(), data.end(), T{});
}

} // namespace algorithms

/** @} */ // end of algorithms group
