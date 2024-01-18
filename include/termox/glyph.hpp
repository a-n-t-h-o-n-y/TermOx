#pragma once

#include <esc/brush.hpp>

namespace ox {

/**
 * @brief A single terminal cell display unit.
 *
 * Contains a symbol and a Brush to describe how the symbol should be displayed.
 */
struct Glyph {
    char32_t symbol;
    esc::Brush brush;
};

}  // namespace ox