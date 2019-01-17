#ifndef CPPURSES_DEMOS_PALETTE_VALUE_CONTROL_HPP
#define CPPURSES_DEMOS_PALETTE_VALUE_CONTROL_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_width.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/vertical_slider.hpp>

namespace palette {
namespace detail {
/// Vertical_slider with extra space around the sides.
struct Buffered_slider : cppurses::Horizontal_layout {
    using Bar_t = cppurses::Blank_width;
    using Slider_t = cppurses::Vertical_slider;

    Bar_t& left{this->make_child<Bar_t>(1)};
    Slider_t& slider{this->make_child<Slider_t>(0, 255)};
    Bar_t& right{this->make_child<Bar_t>(1)};
};

/// Line_edit with extra space on the left side.
struct Buffered_edit_box : cppurses::Horizontal_layout {
    Buffered_edit_box();
    cppurses::Blank_width& left{this->make_child<cppurses::Blank_width>(1)};
    cppurses::Line_edit& box{this->make_child<cppurses::Line_edit>("0")};
};
}  // namespace detail

/// Provides user interface to change a color value from [0, 255].
/** Emits signal when changed. */
class Value_control : public cppurses::Vertical_layout {
    detail::Buffered_slider& slider_{
        this->make_child<detail::Buffered_slider>()};

    detail::Buffered_edit_box& value_edit_{
        this->make_child<detail::Buffered_edit_box>()};

    cppurses::Label& label_;

   public:
    explicit Value_control(const cppurses::Glyph_string& label);

    /// Set the current value being held.
    void set_value(int value) { slider_.slider.set_value(value); }

    sig::Signal<void(int)>& value_changed{slider_.slider.value_changed};
};
}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_VALUE_CONTROL_HPP
