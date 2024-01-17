#ifndef TERMOX_DEMOS_FRACTAL_MANDELBROT_HPP
#define TERMOX_DEMOS_FRACTAL_MANDELBROT_HPP
#include <complex>

#include "float_t.hpp"
#include "square_sum.hpp"

namespace fractal {

/// Returns the number of iterations required for a particular point.
[[nodiscard]] inline auto mandelbrot(Float_t x,
                                     Float_t y,
                                     unsigned int max_iterations)
    -> unsigned int
{
    auto iterations = 0u;
    auto const c    = std::complex{x, y};
    auto z          = std::complex{Float_t{0}, Float_t{0}};
    while (square_sum(z.real(), z.imag()) < 4. && iterations < max_iterations) {
        z = (z * z) + c;
        ++iterations;
    }
    return iterations;
}

}  // namespace fractal
#endif  // TERMOX_DEMOS_FRACTAL_MANDELBROT_HPP
