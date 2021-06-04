#ifndef TERMOX_DEMOS_FRACTAL_SQUARE_SUM_HPP
#define TERMOX_DEMOS_FRACTAL_SQUARE_SUM_HPP
#include "float_t.hpp"

namespace fractal {

/// Sum of Squares, for two values.
/** Optimization b/c std::abs of complex has unnecessary square root for these
 *  calculations. */
[[nodiscard]] inline auto square_sum(Float_t a, Float_t b) -> Float_t
{
    return (a * a) + (b * b);
}

}  // namespace fractal
#endif  // TERMOX_DEMOS_FRACTAL_SQUARE_SUM_HPP
