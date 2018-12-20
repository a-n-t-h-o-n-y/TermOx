#include <cppurses/system/terminal_properties.hpp>

#include <cstdint>

#include <ncurses.h>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/palettes.hpp>

namespace {

std::int16_t scale(std::int16_t value) {
    return static_cast<std::int16_t>((static_cast<double>(value) / 255) * 999);
}

}  // namespace

namespace cppurses {

void Terminal_properties::initialize() {
    // TODO Integrate this with Paint_engine constructor into an init function.
    this->update_dimensions();
    this->set_color_palette(Palettes::DawnBringer());
    this->handle_control_characters(false);
}

void Terminal_properties::set_background_tile(const Glyph& tile) {
    default_background_ = tile;
    // TODO should notify the paint system to repaint every widget.
}

void Terminal_properties::set_color_palette(const Palette& palette) {
    for (const Color_definition& def : palette) {
        std::int16_t ncurses_color_number{static_cast<std::int16_t>(def.color)};
        ::init_color(ncurses_color_number, scale(def.values.red),
                     scale(def.values.green), scale(def.values.blue));
    }
}

void Terminal_properties::handle_control_characters(bool handle) {
    if (handle) {
        ::raw();
    } else {
        ::noraw();
        ::cbreak();
    }
}

void Terminal_properties::update_dimensions() {
    width_ = getmaxx(::stdscr);
    height_ = getmaxy(::stdscr);
}

}  // namespace cppurses
