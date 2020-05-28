#ifndef CPPURSES_PAINTER_PALETTE_BASIC8_HPP
#define CPPURSES_PAINTER_PALETTE_BASIC8_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses::basic8 {

constexpr auto Black  = Color{0};
constexpr auto Maroon = Color{1};
constexpr auto Green  = Color{2};
constexpr auto Olive  = Color{3};
constexpr auto Navy   = Color{4};
constexpr auto Purple = Color{5};
constexpr auto Teal   = Color{6};
constexpr auto Silver = Color{7};

inline auto const palette = [] {
    auto pal = ANSI_palette{};
    for (auto i = Color::Value_t{0}; i < 8; ++i)
        pal.push_back({Color{i}, ANSI{static_cast<ANSI::Value_t>(i % 8)}});
    return pal;
}();
}  // namespace cppurses::basic8
#endif  // CPPURSES_PAINTER_PALETTE_BASIC8_HPP
