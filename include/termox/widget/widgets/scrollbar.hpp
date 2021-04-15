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
   public:
    struct Parameters {
        std::size_t size     = 0;
        std::size_t position = invalid_position;
    };

   private:
    class Middle : public Widget {
       public:
        using Parameters = Scrollbar::Parameters;

       public:
        static auto constexpr invalid_position = -1uL;

       public:
        Middle(std::size_t size = 0, std::size_t position = 0)
            : size_{size}, position_{position}
        {
            this->set_bar_fg(Color::Foreground);
            this->set_bar_bg(Color::Dark_gray);
        }

        Middle(Parameters parameters)
            : Middle{parameters.size, parameters.position}
        {}

       public:
        void set_position(std::size_t p)
        {
            position_ = p;
            this->update_length_and_position();
            this->update();
        }

        void set_size(std::size_t s)
        {
            size_ = s;
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
                *this | pipe::wallpaper(U'▬' | fg(c));
            this->update();
        }

        /// Finds the scroll position from a physical point on the bar.
        [[nodiscard]] auto find_position_from_point(Point p) -> std::size_t
        {
            auto const length = (int)this->max_length() - (int)slider_length_;
            if (length < 1 || size_ == 0)
                return 0;
            double const at = is_vertical ? p.y : p.x;
            double ratio    = at / length;
            if (ratio > 1.)
                ratio = 1.;
            return ratio * (size_ - 1);
        }

       protected:
        auto paint_event(Painter& p) -> bool override
        {
            if (size_ == 0 || position_ == invalid_position ||
                slider_length_ == 0) {
                return Widget::paint_event(p);
            }
            auto const begin  = point(slider_position_);
            auto const length = point(slider_length_ - 1);  // bc line inclusive
            if constexpr (is_vertical)
                p.vline(bar_, begin, begin + length);
            else
                p.hline(bar_, begin, begin + length);
            return Widget::paint_event(p);
        }

        auto resize_event(Area new_size, Area old_size) -> bool override
        {
            this->update_length_and_position();
            return Widget::resize_event(new_size, old_size);
        }

       private:
        static auto constexpr is_vertical = layout::is_vertical_v<Layout_t>;

       private:
        std::size_t size_;
        std::size_t position_;

        Glyph bar_ = is_vertical ? U'█' : U'▬';

        int slider_position_;
        std::size_t slider_length_;

       private:
        void update_length_and_position()
        {
            // Order Matters, position depends on length
            auto const length = this->max_length();
            slider_length_    = slider_length(length, size_);
            slider_position_ =
                slider_position(length, size_, position_, slider_length_);
        }

        [[nodiscard]] static auto constexpr point(int position) -> Point
        {
            if constexpr (is_vertical)
                return {0, position};
            else
                return {position, 0};
        }

        [[nodiscard]] static auto constexpr slider_position(
            int max_length,
            double line_count,
            double position,
            std::size_t slider_length) -> int
        {
            if (line_count == 0. || line_count == 1.)
                return 0;
            double const ratio = position / (line_count - 1);
            auto const length  = max_length - slider_length;
            return std::round(ratio * length);
        }

        [[nodiscard]] static constexpr auto slider_length(double max_length,
                                                          double line_count)
            -> std::size_t
        {
            if (max_length == 0. || line_count == 0.)
                return 0;
            return std::ceil((1. / line_count) * max_length);
        }

        [[nodiscard]] auto max_length() const -> std::size_t
        {
            if constexpr (is_vertical)
                return this->height();
            else
                return this->width();
        }
    };

   public:
    static auto constexpr invalid_position = -1uL;

   public:
    Button& decrement_btn =
        this->template make_child<Button>(Glyph_string{top_symbol_});

    Middle& middle = this->template make_child<Middle>({0, invalid_position});

    Button& increment_btn =
        this->template make_child<Button>(Glyph_string{bottom_symbol_});

   public:
    sl::Signal<void(std::size_t)> new_position;
    sl::Signal<void()> decremented;
    sl::Signal<void()> incremented;

   public:
    explicit Scrollbar(std::size_t size     = 0,
                       std::size_t position = invalid_position)
        : size_{size}, position_{position}
    {
        using namespace pipe;
        if constexpr (is_vertical) {
            *this | fixed_width(1);
            decrement_btn | fixed_height(1);
            middle | expanding_height(0);
            increment_btn | fixed_height(1);
        }
        else {
            *this | fixed_height(1);
            decrement_btn | fixed_width(1);
            middle | expanding_width(0);
            increment_btn | fixed_width(1);
        }

        decrement_btn.pressed.connect([this] { this->decrement_position(); });
        increment_btn.pressed.connect([this] { this->increment_position(); });

        decrement_btn.install_event_filter(*this);
        increment_btn.install_event_filter(*this);
        middle.install_event_filter(*this);
    }

    explicit Scrollbar(Parameters parameters)
        : Scrollbar{parameters.size, parameters.position}
    {}

   public:
    [[nodiscard]] auto get_size() const -> std::size_t { return size_; }

    void set_size(std::size_t s)
    {
        size_ = s;
        if (position_ == invalid_position && size_ != 0)
            this->set_position(0);
        else if (position_ >= size_)
            this->set_position(size_ - 1);
        middle.set_size(s);
    }

    void increment_size() { this->set_size(this->get_size() + 1); }

    void decrement_size()
    {
        if (this->get_size() == 0)
            return;
        this->set_size(this->get_size() - 1);
    }

    [[nodiscard]] auto get_position() const -> std::size_t { return position_; }

    void set_position(std::size_t p)
    {
        if (size_ == 0)
            return;
        position_ = p < size_ ? p : size_ - 1;
        new_position.emit(position_);
        middle.set_position(p);
    }

    void decrement_position()
    {
        if (position_ == invalid_position)
            return;
        if (position_ == 0)
            return;
        this->set_position(position_ - 1);
        decremented();
    }

    void increment_position()
    {
        if (position_ + 1 == size_)
            return;
        this->set_position(position_ + 1);
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
            this->set_position(middle.find_position_from_point(m.at));
        return true;
    }

    auto mouse_move_event_filter(Widget& w, Mouse const& m) -> bool override
    {
        if (&w != &middle)
            return false;
        if (m.button == Mouse::Button::Left)
            this->set_position(middle.find_position_from_point(m.at));
        return true;
    }

   private:
    std::size_t size_;
    std::size_t position_;

   private:
    static auto constexpr is_vertical    = layout::is_vertical_v<Layout_t>;
    static auto constexpr top_symbol_    = is_vertical ? U'▴' : U'◂';
    static auto constexpr bottom_symbol_ = is_vertical ? U'▾' : U'▸';
};

/// Helper function to create a Scrollbar instance.
template <typename Layout_t>
[[nodiscard]] auto scrollbar(
    std::size_t size     = 0,
    std::size_t position = Scrollbar<Layout_t>::invalid_position)
    -> std::unique_ptr<Scrollbar<Layout_t>>
{
    return std::make_unique<Scrollbar<Layout_t>>(size, position);
}

/// Helper function to create a Scrollbar instance.
template <typename Layout_t>
[[nodiscard]] auto scrollbar(typename Scrollbar<Layout_t>::Parameters p)
    -> std::unique_ptr<Scrollbar<Layout_t>>
{
    return std::make_unique<Scrollbar<Layout_t>>(std::move(p));
}

using HScrollbar = Scrollbar<layout::Horizontal<>>;

/// Helper function to create an HScrollbar instance.
[[nodiscard]] inline auto hscrollbar(
    std::size_t size     = 0,
    std::size_t position = HScrollbar::invalid_position)
    -> std::unique_ptr<HScrollbar>
{
    return std::make_unique<HScrollbar>(size, position);
}

/// Helper function to create an HScrollbar instance.
[[nodiscard]] inline auto hscrollbar(HScrollbar::Parameters p)
    -> std::unique_ptr<HScrollbar>
{
    return std::make_unique<HScrollbar>(std::move(p));
}

using VScrollbar = Scrollbar<layout::Vertical<>>;

/// Helper function to create a VScrollbar instance.
[[nodiscard]] inline auto vscrollbar(
    std::size_t size     = 0,
    std::size_t position = VScrollbar::invalid_position)
    -> std::unique_ptr<VScrollbar>
{
    return std::make_unique<VScrollbar>(size, position);
}

/// Helper function to create a VScrollbar instance.
[[nodiscard]] inline auto vscrollbar(VScrollbar::Parameters p)
    -> std::unique_ptr<VScrollbar>
{
    return std::make_unique<VScrollbar>(std::move(p));
}

/// Link a scrollbar and any Linear_layout type.
template <typename Layout_t, typename Child, typename Parameters>
void link(Scrollbar<Layout_t>& scrollbar,
          layout::detail::Linear_layout<Child, Parameters>& layout,
          bool hijack_scroll = true)
{
    scrollbar.set_size(layout.child_count());

    scrollbar.new_position.connect([&](std::size_t p) {
        if (p < layout.child_count())
            layout.set_child_offset(p);
    });

    layout.child_added.connect([&](auto&) { scrollbar.increment_size(); });
    layout.child_removed.connect([&](auto&) { scrollbar.decrement_size(); });
    if (hijack_scroll) {
        layout.child_added.connect([&](auto& child) {
            child.install_event_filter(scrollbar);
            for (Widget* descendant : child.get_descendants())
                descendant->install_event_filter(scrollbar);
        });
        scrollbar.mouse_wheel_scrolled_filter.connect(
            [&](auto&, auto const& mouse) {
                scrollbar.handle_wheel(mouse.button);
            });
        layout.mouse_wheel_scrolled.connect(
            [&](auto const& mouse) { scrollbar.handle_wheel(mouse.button); });
    }
}

/// Link a Scrollbar and Textbox together.
template <typename Layout_t>
void link(Scrollbar<Layout_t>& scrollbar, Textbox& textbox)
{
    scrollbar.set_size(textbox.line_count());

    textbox.line_count_changed.connect(
        [&](std::size_t lines) { scrollbar.set_size(lines); });

    scrollbar.new_position.connect(
        [&](std::size_t p) { textbox.set_top_line(p); });

    textbox.scrolled_to.connect([&](int n) { scrollbar.set_position(n); });
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
