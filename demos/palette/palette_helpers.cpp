#include "palette_helpers.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/color_definition.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/system/terminal_properties.hpp>
using namespace cppurses;

namespace palette {
namespace detail {

Color_definition& extract_definition(Palette& palette, Color color) {
    auto at = std::find_if(
        std::begin(palette), std::end(palette),
        [color](const Color_definition& def) { return def.color == color; });
    if (at != std::end(palette)) {
        return *at;
    }
    return palette[0];
}
}  // namespace detail
}  // namespace palette

namespace {
using namespace palette::detail;

/// Set a color definition for the terminal.
void set_definition(Color color, RGB values) {
    auto palette = cppurses::System::terminal.current_palette();
    auto& definition = extract_definition(palette, color);
    definition.values = values;
    cppurses::System::terminal.set_color_palette(palette);
}
}  // namespace

namespace palette {
namespace detail {

void set_red(Color color, std::int16_t red_value) {
    auto palette = System::terminal.current_palette();
    const auto& def = extract_definition(palette, color);
    set_definition(color, {red_value, def.values.green, def.values.blue});
}

void set_green(Color color, std::int16_t green_value) {
    auto palette = System::terminal.current_palette();
    const auto& def = extract_definition(palette, color);
    set_definition(color, {def.values.red, green_value, def.values.blue});
}

void set_blue(Color color, std::int16_t blue_value) {
    auto palette = System::terminal.current_palette();
    const auto& def = extract_definition(palette, color);
    set_definition(color, {def.values.red, def.values.green, blue_value});
}
}  // namespace detail
}  // namespace palette
