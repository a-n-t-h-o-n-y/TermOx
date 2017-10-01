#include "painter/palette.hpp"
#include "painter/color.hpp"
#include "painter/detail/paint_buffer.hpp"
#include "system/system.hpp"
#include <cstddef>
#include <cstdint>

namespace {

std::size_t translate(cppurses::Color c) {
    return static_cast<std::int16_t>(c) - 240;
}

}  // namespace

namespace cppurses {

void Palette::initialize() {
    Paint_buffer* pb = System::paint_buffer();
    pb->set_color(Color::Black, this->get_values(Color::Black));
    pb->set_color(Color::Dark_red, this->get_values(Color::Dark_red));
    pb->set_color(Color::Dark_blue, this->get_values(Color::Dark_blue));
    pb->set_color(Color::Dark_gray, this->get_values(Color::Dark_gray));
    pb->set_color(Color::Brown, this->get_values(Color::Brown));
    pb->set_color(Color::Green, this->get_values(Color::Green));
    pb->set_color(Color::Red, this->get_values(Color::Red));
    pb->set_color(Color::Gray, this->get_values(Color::Gray));
    pb->set_color(Color::Blue, this->get_values(Color::Blue));
    pb->set_color(Color::Orange, this->get_values(Color::Orange));
    pb->set_color(Color::Light_gray, this->get_values(Color::Light_gray));
    pb->set_color(Color::Light_green, this->get_values(Color::Light_green));
    pb->set_color(Color::Violet, this->get_values(Color::Violet));
    pb->set_color(Color::Light_blue, this->get_values(Color::Light_blue));
    pb->set_color(Color::Yellow, this->get_values(Color::Yellow));
    pb->set_color(Color::White, this->get_values(Color::White));
    pb->flush(false);
}

void Palette::set_color(Color c,
                        std::int16_t r,
                        std::int16_t g,
                        std::int16_t b) {
    auto& rgb = definitions_.at(translate(c));
    rgb.red = r;
    rgb.green = g;
    rgb.blue = b;
}

void Palette::set_color(Color c, RGB values) {
    this->set_color(c, values.red, values.green, values.blue);
}

RGB Palette::get_values(Color c) const {
    return RGB{this->red_value(c), this->green_value(c), this->blue_value(c)};
}

std::int16_t Palette::red_value(Color c) const {
    return definitions_.at(translate(c)).red;
}

std::int16_t Palette::green_value(Color c) const {
    return definitions_.at(translate(c)).green;
}

std::int16_t Palette::blue_value(Color c) const {
    return definitions_.at(translate(c)).blue;
}

Standard_palette::Standard_palette() {
    this->set_color(Color::Black, 0, 0, 0);
    this->set_color(Color::Dark_red, 128, 0, 0);
    this->set_color(Color::Dark_blue, 0, 0, 128);
    this->set_color(Color::Dark_gray, 105, 105, 105);
    this->set_color(Color::Brown, 128, 128, 0);
    this->set_color(Color::Green, 0, 128, 0);
    this->set_color(Color::Red, 255, 0, 0);
    this->set_color(Color::Gray, 128, 128, 128);
    this->set_color(Color::Blue, 0, 0, 255);
    this->set_color(Color::Orange, 255, 165, 0);
    this->set_color(Color::Light_gray, 192, 192, 192);
    this->set_color(Color::Light_green, 0, 255, 0);
    this->set_color(Color::Violet, 255, 0, 255);
    this->set_color(Color::Light_blue, 0, 255, 255);
    this->set_color(Color::Yellow, 255, 255, 0);
    this->set_color(Color::White, 255, 255, 255);
}

DawnBringer_palette::DawnBringer_palette() {
    this->set_color(Color::Black, 20, 12, 28);
    this->set_color(Color::Dark_red, 68, 36, 52);
    this->set_color(Color::Dark_blue, 48, 52, 109);
    this->set_color(Color::Dark_gray, 78, 74, 78);
    this->set_color(Color::Brown, 133, 76, 48);
    this->set_color(Color::Green, 52, 101, 36);
    this->set_color(Color::Red, 208, 70, 72);
    this->set_color(Color::Gray, 117, 113, 97);
    this->set_color(Color::Blue, 89, 125, 206);
    this->set_color(Color::Orange, 210, 125, 44);
    this->set_color(Color::Light_gray, 133, 149, 161);
    this->set_color(Color::Light_green, 109, 170, 44);
    this->set_color(Color::Violet, 210, 170, 153);
    this->set_color(Color::Light_blue, 109, 194, 202);
    this->set_color(Color::Yellow, 218, 212, 94);
    this->set_color(Color::White, 222, 238, 214);
}

}  // namespace cppurses
