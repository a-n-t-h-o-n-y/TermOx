#ifndef TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
#define TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/layouts/detail/linear_layout.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

/// Scrollbar to display progress through some structure, and control progress.
/** Provides an increment, decrement, and new_position Signals to notify of
 *  changes. The size parameter tells the scrollbar how many units of scroll
 *  there are to the scructure it is linked with, and the current position the
 *  scrollbar is at is emitted with the new_position signal. */
template <typename Layout_t>
class Scrollbar : public Layout_t {
   private:
    class Middle : public Widget {
       public:
        static auto constexpr invalid_position = -1uL;

       public:
        struct Parameters {
            std::size_t size;
            std::size_t position;
        };

       public:
        Middle(Parameters p = {0uL, 0uL}) : parameters_{p}
        {
            this->set_bar_fg(Color::Foreground);
            this->set_bar_bg(Color::Dark_gray);
        }

       public:
        void set_position(std::size_t p)
        {
            parameters_.position = p;
            this->update_length_and_position();
            this->update();
        }

        void set_size(std::size_t s)
        {
            parameters_.size = s;
            this->update_length_and_position();
            this->update();
        }

        void set_bar_fg(Color c)
        {
            bar_ | fg(c);
            this->update();
        }

        void set_bar_bg(Color c)
        {
            if constexpr (is_vertical)
                *this | bg(c);
            else
                *this | pipe::wallpaper(L'▬' | fg(c));
            this->update();
        }

        /// Finds the scroll position from a physical point on the bar.
        auto find_position_from_point(Point p) -> std::size_t
        {
            auto const length = (int)this->max_length() - (int)slider_length_;
            if (length < 1 || parameters_.size == 0)
                return 0;
            double const at = is_vertical ? p.y : p.x;
            double ratio    = at / length;
            if (ratio > 1.)
                ratio = 1.;
            return ratio * (parameters_.size - 1);
        }

       protected:
        auto paint_event() -> bool override
        {
            if (parameters_.size == 0uL ||
                parameters_.position == invalid_position ||
                slider_length_ == 0uL) {
                return Widget::paint_event();
            }
            auto const begin  = point(slider_position_);
            auto const length = point(slider_length_ - 1);  // bc line inclusive
            Painter{*this}.line(bar_, begin, begin + length);
            return Widget::paint_event();
        }

        auto resize_event(Area new_size, Area old_size) -> bool override
        {
            this->update_length_and_position();
            return Widget::resize_event(new_size, old_size);
        }

       private:
        static auto constexpr is_vertical = layout::is_vertical_v<Layout_t>;

       private:
        Parameters parameters_;
        Glyph bar_ = is_vertical ? L'█' : L'▬';

        std::size_t slider_position_;
        std::size_t slider_length_;

       private:
        void update_length_and_position()
        {
            // Order Matters, position depends on length
            auto const length = this->max_length();
            slider_length_    = slider_length(length, parameters_.size);
            slider_position_  = slider_position(
                length, parameters_.size, parameters_.position, slider_length_);
        }

        static auto constexpr point(std::size_t position) -> Point
        {
            if constexpr (is_vertical)
                return {0uL, position};
            else
                return {position, 0uL};
        }

        static auto constexpr slider_position(std::size_t max_length,
                                              double line_count,
                                              double position,
                                              std::size_t slider_length)
            -> std::size_t
        {
            if (line_count == 0. || line_count == 1.)
                return 0uL;
            double const ratio = position / (line_count - 1);
            auto const length  = max_length - slider_length;
            return std::round(ratio * length);
        }

        static auto constexpr slider_length(double max_length,
                                            double line_count) -> std::size_t
        {
            if (max_length == 0. || line_count == 0.)
                return 0;
            return std::ceil((1. / line_count) * max_length);
        }

        auto max_length() const -> std::size_t
        {
            if constexpr (is_vertical)
                return this->height();
            else
                return this->width();
        }
    };

   public:
    struct Parameters {
        std::size_t size     = 0uL;
        std::size_t position = invalid_position;
    };

   public:
    static auto constexpr invalid_position = -1uL;

   public:
    Button& decrement_btn =
        this->template make_child<Button>(Glyph_string{top_symbol_});

    Middle& middle = this->template make_child<Middle>({0uL, invalid_position});

    Button& increment_btn =
        this->template make_child<Button>(Glyph_string{bottom_symbol_});

   public:
    sl::Signal<void(std::size_t)> new_position;
    sl::Signal<void()> decremented;
    sl::Signal<void()> incremented;

   public:
    Scrollbar(Parameters p = {}) : parameters_{std::move(p)}
    {
        using namespace pipe;
        if constexpr (is_vertical) {
            *this | fixed_width(1uL);
            decrement_btn | fixed_height(1uL);
            middle | expanding_height(0uL);
            increment_btn | fixed_height(1uL);
        }
        else {
            *this | fixed_height(1uL);
            decrement_btn | fixed_width(1uL);
            middle | expanding_width(0uL);
            increment_btn | fixed_width(1uL);
        }

        decrement_btn.pressed.connect([this] { this->decrement_position(); });
        increment_btn.pressed.connect([this] { this->increment_position(); });

        decrement_btn.install_event_filter(*this);
        increment_btn.install_event_filter(*this);
        middle.install_event_filter(*this);
    }

   public:
    auto get_size() const -> std::size_t { return parameters_.size; }

    void set_size(std::size_t s)
    {
        parameters_.size = s;
        if (parameters_.position == invalid_position && parameters_.size != 0uL)
            this->set_position(0uL);
        else if (parameters_.position >= parameters_.size)
            this->set_position(parameters_.size - 1uL);
        middle.set_size(s);
    }

    void increment_size() { this->set_size(this->get_size() + 1uL); }

    void decrement_size()
    {
        if (this->get_size() == 0uL)
            return;
        this->set_size(this->get_size() - 1uL);
    }

    auto get_position() const -> std::size_t { return parameters_.position; }

    void set_position(std::size_t p)
    {
        if (parameters_.size == 0uL)
            return;
        parameters_.position =
            p < parameters_.size ? p : parameters_.size - 1uL;
        new_position(parameters_.position);
        middle.set_position(p);
    }

    void decrement_position()
    {
        if (parameters_.position == invalid_position)
            return;
        if (parameters_.position == 0uL)
            return;
        this->set_position(parameters_.position - 1uL);
        decremented();
    }

    void increment_position()
    {
        if (parameters_.position + 1uL == parameters_.size)
            return;
        this->set_position(parameters_.position + 1uL);
        incremented();
    }

    /// Returns true if button was a scoll wheel button
    auto handle_wheel(Mouse::Button button) -> bool
    {
        switch (button) {
            case Mouse::Button::ScrollDown: this->increment_position(); break;
            case Mouse::Button::ScrollUp: this->decrement_position(); break;
            default: return false;
        }
        return true;
    }

   protected:
    auto mouse_wheel_event_filter(Widget&, Mouse const& m) -> bool override
    {
        return this->handle_wheel(m.button);
    }

    auto mouse_press_event_filter(Widget& w, Mouse const& m) -> bool override
    {
        if (&w != &middle)
            return false;
        if (m.button == Mouse::Button::Left)
            this->set_position(middle.find_position_from_point(m.local));
        return true;
    }

   private:
    Parameters parameters_;

   private:
    static auto constexpr is_vertical    = layout::is_vertical_v<Layout_t>;
    static auto constexpr top_symbol_    = is_vertical ? L'▴' : L'◂';
    static auto constexpr bottom_symbol_ = is_vertical ? L'▾' : L'▸';
};

using HScrollbar = Scrollbar<layout::Horizontal<>>;
using VScrollbar = Scrollbar<layout::Vertical<>>;

/// Helper function to create an instance.
template <typename Layout_t>
inline auto scrollbar(typename Scrollbar<Layout_t>::Parameters p)
    -> std::unique_ptr<Scrollbar<Layout_t>>
{
    return std::make_unique<Scrollbar<Layout_t>>(std::move(p));
}

/// Helper function to create an instance.
inline auto h_scrollbar(HScrollbar::Parameters p) -> std::unique_ptr<HScrollbar>
{
    return std::make_unique<HScrollbar>(std::move(p));
}

/// Helper function to create an instance.
inline auto v_scrollbar(VScrollbar::Parameters p) -> std::unique_ptr<VScrollbar>
{
    return std::make_unique<VScrollbar>(std::move(p));
}

template <typename Layout_t, typename Child, typename Parameters>
void link(Scrollbar<Layout_t>& scrollbar,
          layout::detail::Linear_layout<Child, Parameters>& layout,
          bool hijack_scroll = true)
{
    scrollbar.set_size(layout.child_count());

    scrollbar.new_position.connect(
        [&](std::size_t p) { layout.set_child_offset(p); });

    layout.child_added.connect([&](auto&) { scrollbar.increment_size(); });
    layout.child_removed.connect([&](auto&) { scrollbar.decrement_size(); });
    if (hijack_scroll) {
        layout.child_added.connect(
            [&](auto& child) { child.install_event_filter(scrollbar); });
        scrollbar.mouse_wheel_scrolled_filter.connect(
            [&](auto&, auto const& mouse) {
                scrollbar.handle_wheel(mouse.button);
            });
        layout.mouse_wheel_scrolled.connect(
            [&](auto const& mouse) { scrollbar.handle_wheel(mouse.button); });
    }
}

template <typename Layout_t>
void link(Scrollbar<Layout_t>& scrollbar, Textbox& textbox)
{
    scrollbar.set_size(textbox.line_count());

    textbox.line_count_changed.connect(
        [&](std::size_t lines) { scrollbar.set_size(lines); });

    scrollbar.new_position.connect(
        [&](std::size_t p) { textbox.set_top_line(p); });

    textbox.scrolled_to.connect(
        [&](std::size_t n) { scrollbar.set_position(n); });
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
