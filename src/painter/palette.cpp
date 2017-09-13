#include "painter/palette.hpp"
#include "painter/color.hpp"
#include <cstddef>
#include <cstdint>

namespace {

std::size_t translate(cppurses::Color c) {
    return static_cast<std::int16_t>(c) - 240;
}

}  // namespace

namespace cppurses {

void Palette::set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b) {
    auto& rgb = definitions_.at(translate(c));
    rgb.at(0) = r;
    rgb.at(1) = g;
    rgb.at(2) = b;
}

int Palette::red_value(Color c) const {
    return definitions_.at(translate(c)).at(0);
}

int Palette::green_value(Color c) const {
    return definitions_.at(translate(c)).at(1);
}

int Palette::blue_value(Color c) const {
    return definitions_.at(translate(c)).at(2);
}

Standard_palette::Standard_palette() {
    this->set_rgb(Color::Black, 0, 0, 0);
    this->set_rgb(Color::Dark_red, 128, 0, 0);
    this->set_rgb(Color::Dark_blue, 0, 0, 128);
    this->set_rgb(Color::Dark_gray, 105, 105, 105);
    this->set_rgb(Color::Brown, 128, 128, 0);
    this->set_rgb(Color::Green, 0, 128, 0);
    this->set_rgb(Color::Red, 255, 0, 0);
    this->set_rgb(Color::Gray, 128, 128, 128);
    this->set_rgb(Color::Blue, 0, 0, 255);
    this->set_rgb(Color::Orange, 255, 165, 0);
    this->set_rgb(Color::Light_gray, 192, 192, 192);
    this->set_rgb(Color::Light_green, 0, 255, 0);
    this->set_rgb(Color::Violet, 255, 0, 255);
    this->set_rgb(Color::Light_blue, 0, 255, 255);
    this->set_rgb(Color::Yellow, 255, 255, 0);
    this->set_rgb(Color::White, 255, 255, 255);
}

DawnBringer_palette::DawnBringer_palette() {
    this->set_rgb(Color::Black, 20, 12, 28);
    this->set_rgb(Color::Dark_red, 68, 36, 52);
    this->set_rgb(Color::Dark_blue, 48, 52, 109);
    this->set_rgb(Color::Dark_gray, 78, 74, 78);
    this->set_rgb(Color::Brown, 133, 76, 48);
    this->set_rgb(Color::Green, 52, 101, 36);
    this->set_rgb(Color::Red, 208, 70, 72);
    this->set_rgb(Color::Gray, 117, 113, 97);
    this->set_rgb(Color::Blue, 89, 125, 206);
    this->set_rgb(Color::Orange, 210, 125, 44);
    this->set_rgb(Color::Light_gray, 133, 149, 161);
    this->set_rgb(Color::Light_green, 109, 170, 44);
    this->set_rgb(Color::Violet, 210, 170, 153);
    this->set_rgb(Color::Light_blue, 109, 194, 202);
    this->set_rgb(Color::Yellow, 218, 212, 94);
    this->set_rgb(Color::White, 222, 238, 214);
}

}  // namespace cppurses
