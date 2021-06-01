#ifndef TERMOX_DEMOS_COLOR_SLIDERS_HPP
#define TERMOX_DEMOS_COLOR_SLIDERS_HPP
#include <array>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include <termox/termox.hpp>

/// Linear interpolation over Hue values from \p first to \p last.
template <std::size_t Count>
auto constexpr make_color_defs(ox::HSL first, ox::HSL last)
    -> std::array<ox::HSL, Count>
{
    static_assert(Count > 1);
    auto result          = std::array<ox::HSL, Count>{};
    auto const increment = (double)(last.hue - first.hue) / (double)(Count - 1);
    for (auto i = 0uL; i < Count; ++i) {
        std::uint16_t const hue = first.hue + (increment * i);
        result[i] = ox::HSL{hue, first.saturation, first.lightness};
    }
    return result;
}

template <std::size_t Count>
auto make_palette(ox::HSL first, ox::HSL last) -> ox::Palette
{
    auto result    = ox::Palette{};
    std::uint8_t c = 16;
    for (auto const hsl : make_color_defs<Count>(first, last)) {
        result.push_back({ox::Color{c}, hsl});
        ++c;
    }
    return result;
}

class Color_slider : public ox::Slider_bottom {
   public:
    struct Parameters {
        ox::Color color;
        int initial;
    };

   public:
    Color_slider(ox::Color color, int initial)
        : ox::Slider_bottom{{0, 255}, initial}
    {
        *this | fg(color);
    }

    Color_slider(Parameters parameters)
        : Color_slider{parameters.color, parameters.initial}
    {}
};

template <std::size_t Count>
class Color_sliders : public ox::HArray<Color_slider, Count> {
   public:
    Color_sliders(ox::Palette palette)
        : ox::HArray<Color_slider, Count>{make_slider_constructors(palette)},
          palette_{std::move(palette)}
    {
        using namespace ox::pipe;
        *this | strong_focus() |
            on_focus_in([this] { ox::Terminal::set_palette(palette_); });
    }

   private:
    template <std::size_t... Is>
    static auto make_parameters(std::index_sequence<Is...>,
                                ox::Palette const& palette)
        -> std::array<Color_slider::Parameters, Count>
    {
        return {Color_slider::Parameters{palette[Is].color, 0}...};
    }

    static auto make_slider_constructors(ox::Palette const& palette)
        -> std::array<Color_slider::Parameters, Count>
    {
        if (palette.size() < Count)
            throw std::runtime_error{"Palette is too small for Color_sliders."};
        return make_parameters(std::make_index_sequence<Count>(), palette);
    }

   private:
    ox::Palette palette_;
};

#endif  // TERMOX_DEMOS_COLOR_SLIDERS_HPP
