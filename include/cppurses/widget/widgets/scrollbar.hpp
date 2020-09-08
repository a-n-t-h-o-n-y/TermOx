#ifndef CPPURSES_WIDGET_WIDGETS_SCROLLBAR_HPP
#define CPPURSES_WIDGET_WIDGETS_SCROLLBAR_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <iostream>//temp

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/detail/linear_layout.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

/// Scrollbar to display progress through some structure, and control progress.
/** Provides an increment, decrement, and new_position Signals to notify of
    changes. The size parameter tells the scrollbar how many units of scroll
    there are to the scructure it is linked with, and the current position the
    scrollbar is at is emitted with the new_position signal. */
template <typename Layout_t>
class Scrollbar : public Layout_t {
   private:
    class Middle : public Widget {
       public:
        struct Parameters {
            std::size_t position;
            std::size_t length;
        };

       public:
        Middle(Parameters p = {0uL, 0uL}) : parameters_{p}
        {
            *this | pipe::bg(Color::Light_gray);
        }

       public:
        void set_position(std::size_t p)
        {
            parameters_.position = p;
            this->update();
        }

        void set_length(std::size_t l)
        {
            parameters_.length = l;
            this->update();
        }

       protected:
        auto paint_event() -> bool override
        {
            if (parameters_.length == 0uL)
                return Widget::paint_event();

            Painter{*this}.put(L'🬰', {0,0});
            // std::wcerr << bar << L'\n';
            // Painter{*this}.line(
            //     bar, point(parameters_.position),
            //     point(parameters_.position + parameters_.length));
            return Widget::paint_event();
        }

       private:
        static auto constexpr is_vertical = layout::is_vertical_v<Layout_t>;
        static auto constexpr bar =
            is_vertical
                ? L'🬰' | Trait::Inverse | foreground(Color::Foreground)
                : L' ' | background(Color::Foreground);
        // set wallpaper to skinny too

       private:
        Parameters parameters_;

       private:
        static auto constexpr point(std::size_t position) -> Point
        {
            if constexpr (is_vertical)
                return {0uL, position};
            else
                return {position, 0uL};
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

    Middle& middle = this->template make_child<Middle>({5uL, 2uL});

    Button& increment_btn =
        this->template make_child<Button>(Glyph_string{bottom_symbol_});

   public:
    sig::Signal<void(std::size_t)> new_position;
    sig::Signal<void()> decremented;
    sig::Signal<void()> incremented;

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
        // TODO tell middle section about the change
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
        parameters_.position =
            p < parameters_.size ? p : parameters_.size - 1uL;
        new_position(parameters_.position);
        // TODO tell middle section about the change
        // can maybe hook up signal to slider's signal if you make slider work
        // in a similar way instead of with floating point.
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
    virtual auto mouse_wheel_event_filter(Widget&, Mouse::State const& mouse)
        -> bool override
    {
        return this->handle_wheel(mouse.button);
    }

   private:
    Parameters parameters_;

   private:
    static auto constexpr is_vertical    = layout::is_vertical_v<Layout_t>;
    static auto constexpr top_symbol_    = is_vertical ? L'▴' : L'<';
    static auto constexpr bottom_symbol_ = is_vertical ? L'▾' : L'>';
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
        [&](std::size_t p) { layout.set_offset(p); });

    layout.child_added.connect([&](auto&) { scrollbar.increment_size(); });
    layout.child_removed.connect([&](auto&) { scrollbar.decrement_size(); });
    if (hijack_scroll) {
        layout.child_added.connect(
            [&](auto& child) { child.install_event_filter(scrollbar); });
        scrollbar.mouse_wheel_filter.connect([&](auto&, auto const& mouse) {
            scrollbar.handle_wheel(mouse.button);
        });
        layout.mouse_wheel_scrolled.connect(
            [&](auto const& mouse) { scrollbar.handle_wheel(mouse.button); });
    }
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_SCROLLBAR_HPP
