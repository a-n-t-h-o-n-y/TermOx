#ifndef TERMOX_DEMOS_FRACTAL_JULIA_HPP
#define TERMOX_DEMOS_FRACTAL_JULIA_HPP
#include <complex>
#include <utility>

#include "float_t.hpp"
#include "square_sum.hpp"

namespace fractal {

/// Returns the number of iterations required for a particular point.
[[nodiscard]] inline auto julia(Float_t x,
                                Float_t y,
                                std::complex<Float_t> c,
                                unsigned int max_iterations) -> unsigned int
{
    auto iterations = 0u;
    auto z          = std::complex{x, y};
    while (square_sum(z.real(), z.imag()) < 4. && iterations < max_iterations) {
        z = (z * z) + c;
        ++iterations;
    }
    return iterations;
}

}  // namespace fractal
#endif  // TERMOX_DEMOS_FRACTAL_JULIA_HPP
