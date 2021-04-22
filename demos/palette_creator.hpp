#ifndef TERMOX_DEMOS_PALETTE_CREATOR_HPP
#define TERMOX_DEMOS_PALETTE_CREATOR_HPP
#include <memory>

#include <termox/termox.hpp>
#include <type_traits>
#include <variant>

namespace demo {

[[nodiscard]] inline auto slider(ox::Slider_bottom::Range range,
                                 ox::Glyph label)
{
    using namespace ox::pipe;
    // clang-format off
    auto x =
        ox::htuple(
            ox::widget() | fixed_width(1),
            ox::vtuple(
                ox::slider_bottom(range, 0),
                ox::int_view(),
                ox::hlabel(ox::Glyph_string{label}, ox::Align::Center)
            ),
            ox::widget() | fixed_width(1)
        ) | fixed_width(5);
    // clang-format on

    auto& slider   = x->get<1>().get<0>();
    auto& int_view = x->get<1>().get<1>();

    slider.value_changed.connect([&](int v) { int_view.set_value(v); });

    return x;
}

[[nodiscard]] inline auto rgb_sliders()
{
    // clang-format off
    auto sliders =
        ox::htuple(
            slider({0, 255}, U'R'),
            slider({0, 255}, U'G'),
            slider({0, 255}, U'B')
        ) | ox::pipe::fixed_width(15);
    // clang-format on
    return sliders;
}

[[nodiscard]] inline auto hsl_sliders()
{
    // clang-format off
    auto sliders =
        ox::htuple(
            slider({0, 360}, U'H'),
            slider({0, 100}, U'S'),
            slider({0, 100}, U'L')
        );
    // clang-format on
    return sliders;
}

[[nodiscard]] inline auto slider_stack()
{
    using namespace ox::pipe;

    // clang-format off
    auto x =
        ox::vpair(
            ox::spair(
                rgb_sliders(),
                hsl_sliders()
            ),
            ox::button(U"Use HSL") | fixed_height(1)
                | fg(ox::Color::Background) | bg(ox::Color::Foreground)
        ) | ox::pipe::fixed_width(15);
    // clang-format on

    auto& stack = x->first;
    auto& btn   = x->second;

    stack.set_active_page(0);

    btn | on_press([&] {
        auto const ap = stack.active_page_index();
        if (ap == 0) {
            stack.set_active_page(1);
            btn.set_text(U"Use RGB");
        }
        else {
            stack.set_active_page(0);
            btn.set_text(U"Use HSL");
        }
    });

    return x;
}

struct Color_control : ox::layout::Horizontal<> {
    using Slider_stack = std::remove_reference_t<decltype(
        *std::declval<decltype(std::function{slider_stack})::result_type>())>;

    Slider_stack& ss = this->append_child(slider_stack());

    Color_control()
    {
        using namespace ox::pipe;

        *this | fixed_width(15);

        auto& rgb       = ss.first.first;
        auto& r_changed = rgb.get<0>().get<1>().get<0>().value_changed;
        auto& g_changed = rgb.get<1>().get<1>().get<0>().value_changed;
        auto& b_changed = rgb.get<2>().get<1>().get<0>().value_changed;
        auto& r_value   = rgb.get<0>().get<1>().get<1>();
        auto& g_value   = rgb.get<1>().get<1>().get<1>();
        auto& b_value   = rgb.get<2>().get<1>().get<1>();

        auto& hsl       = ss.first.second;
        auto& h_changed = hsl.get<0>().get<1>().get<0>().value_changed;
        auto& s_changed = hsl.get<1>().get<1>().get<0>().value_changed;
        auto& l_changed = hsl.get<2>().get<1>().get<0>().value_changed;
        auto& h_value   = hsl.get<0>().get<1>().get<1>();
        auto& s_value   = hsl.get<1>().get<1>().get<1>();
        auto& l_value   = hsl.get<2>().get<1>().get<1>();

        auto& btn = ss.second;

        auto const signal_rgb = [&] {
            auto const rgb = ox::RGB{(std::uint8_t)r_value.value(),
                                     (std::uint8_t)g_value.value(),
                                     (std::uint8_t)b_value.value()};
            new_color.emit({rgb});
        };

        auto const signal_hsl = [&] {
            auto const hsl = ox::HSL{(std::uint16_t)h_value.value(),
                                     (std::uint8_t)s_value.value(),
                                     (std::uint8_t)l_value.value()};
            new_color.emit({hsl});
        };

        r_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 0)
                signal_rgb();
        });
        g_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 0)
                signal_rgb();
        });
        b_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 0)
                signal_rgb();
        });
        h_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 1)
                signal_hsl();
        });
        s_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 1)
                signal_hsl();
        });
        l_changed.connect([=](auto) {
            if (ss.first.active_page_index() == 1)
                signal_hsl();
        });

        btn | on_press([&, signal_hsl, signal_rgb] {
            if (ss.first.active_page_index() == 0)
                signal_rgb();
            else
                signal_hsl();
        });
    }

    sl::Signal<void(ox::True_color)> new_color;
};

class Palette_creator : public ox::HPair<Color_control, ox::Color_select> {
   public:
    Color_control& control   = this->first;
    ox::Color_select& select = this->second;

   public:
    // TODO export current palette as hpp file
    // TODO add/remove color buttons
    // TODO set slider values on color selected
    Palette_creator()
    {
        *this | ox::pipe::strong_focus();
        control.new_color.connect(
            [this](ox::True_color c) { this->update_selected(c); });
        select.color_selected.connect([this](ox::Color c) { selected_ = c; });
        ox::Terminal::palette_changed.connect(
            [this](auto const& pal) { palette_ = pal; });
    }

   protected:
    auto focus_in_event() -> bool override
    {
        ox::Terminal::set_palette(ox::dawn_bringer32::palette);
        return ox::HPair<Color_control, ox::Color_select>::focus_in_event();
    }

   private:
    ox::Color selected_  = ox::Color::Background;
    ox::Palette palette_ = ox::Terminal::current_palette();

   private:
    void update_selected(ox::True_color c)
    {
        auto const at = std::find_if(
            std::begin(palette_), std::end(palette_),
            [this](auto const& def) { return def.color == selected_; });
        if (at != std::end(palette_)) {
            at->value = c;
            ox::Terminal::set_palette(palette_);
        }
    }
};

[[nodiscard]] inline auto make_palette_creator()
{
    return std::make_unique<Palette_creator>();
}

}  // namespace demo
#endif  // TERMOX_DEMOS_PALETTE_CREATOR_HPP
