#ifndef TERMOX_DEMOS_PALETTE_VALUE_CONTROL_HPP
#define TERMOX_DEMOS_PALETTE_VALUE_CONTROL_HPP
#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/vertical_slider.hpp>

namespace palette::detail {

/// Vertical_slider with extra space around the sides.
struct Buffered_slider : ox::layout::Horizontal<> {
    using Slider_t = ox::Vertical_slider;

    Widget& left     = this->make_child() | ox::pipe::fixed_width(1);
    Slider_t& slider = this->make_child<Slider_t>(0, 255);
    Widget& right    = this->make_child() | ox::pipe::fixed_width(1);
};

/// Line_edit with extra space on the left side.
class Buffered_edit_box : public ox::layout::Horizontal<> {
   public:
    Buffered_edit_box();

   public:
    Widget& left       = this->make_child() | ox::pipe::fixed_width(1);
    ox::Line_edit& box = this->make_child<ox::Line_edit>("0");
};
}  // namespace palette::detail

namespace palette {

/// Provides user interface to change a color value from [0, 255].
/** Emits signal when changed. */
class Value_control : public ox::layout::Vertical<> {
    detail::Buffered_slider& slider_{
        this->make_child<detail::Buffered_slider>()};

    detail::Buffered_edit_box& value_edit_{
        this->make_child<detail::Buffered_edit_box>()};

    ox::HLabel& label_;

   public:
    explicit Value_control(const ox::Glyph_string& label);

    /// Set the current value being held.
    void set_value(int value) { slider_.slider.set_value(value); }

    sl::Signal<void(int)>& value_changed{slider_.slider.value_changed};
};

}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_VALUE_CONTROL_HPP
