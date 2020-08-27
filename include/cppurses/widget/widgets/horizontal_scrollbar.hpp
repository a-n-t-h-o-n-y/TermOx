#ifndef CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#define CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
#include <cstddef>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/opposite.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>

namespace cppurses {

/// Scrollbar to display progress through some structure, and control progress.
/** Provides an increment, decrement, and new_position Signals to notify of
    changes. The size parameter tells the scrollbar how many units of scroll
    there are to the scructure it is linked with, and the current unit the
    scrollbar is at is emitted with the new_position signal. */
template <typename Layout_t>
class Scrollbar : public layout::Opposite_t<Layout_t> {
   public:
    struct Parameters {
        std::size_t size;
        std::size_t position = 0uL;
    };

   public:
    Button& top_btn = this->template make_child<Button>(top_symbol_);

    // TODO make this a middle bar widget that displays progress and can be
    // clicked to return a new ratio 0..1
    // Replace with a Slider, make Slider take a layout type as well.
    Widget& middle = this->make_child();

    Button& bottom_btn = this->template make_child<Button>(bottom_symbol_);

   public:
    sig::Signal<void(std::size_t)> new_position;
    sig::Signal<void()>& increment = top_btn.pressed;
    sig::Signal<void()>& decrement = bottom_btn.pressed;

   public:
    Scrollbar(Parameters p) : parameters_{std::move(p)}
    {
        // using namespace pipe;
        // *this | fixed_height(1) | expanding_width(0);

        // left_btn | fixed_width(1);
        // middle | expanding_width(0) | bg(Color::Light_gray);
        // right_btn | fixed_width(1);
    }

   public:
    auto get_size() const -> std::size_t { return parameters_.size; }

    void set_size(std::size_t s)
    {
        parameters_.size = s;
        // TODO tell middle section about the change
        // if new size is smaller than current position, then set position to
        // the max value possible(size - 1). emit signal.
    }

    auto get_position() const -> std::size_t { return parameters_.position; }

    void set_position(std::size_t p)
    {
        parameters_.position = p;
        // TODO tell middle section about the change
        // emit signal. can maybe hook up signal to slider's signal if you make
        // slider work in a similar way instead of with floating point.
        // bounds check first, clamp it to max if it is over
    }

   private:
    Parameters parameters_;

   private:
    static auto constexpr is_vertical    = layout::is_vertical_v<Layout_t>;
    static auto constexpr top_symbol_    = is_vertical ? L'▴' : L'<';
    static auto constexpr bottom_symbol_ = is_vertical ? L'▾' : L'<';
};

using HScrollbar = Scrollbar<layout::Horizontal<>>;
using VScrollbar = Scrollbar<layout::Vertical<>>;

class Horizontal_scrollbar : public layout::Horizontal<> {
   public:
    Button& left_btn  = this->make_child<Button>("<");
    Widget& middle    = this->make_child<Widget>();
    Button& right_btn = this->make_child<Button>(">");

    sig::Signal<void()>& left  = left_btn.pressed;
    sig::Signal<void()>& right = right_btn.pressed;

   public:
    Horizontal_scrollbar()
    {
        using namespace pipe;
        *this | fixed_height(1) | expanding_width(0);

        left_btn | fixed_width(1);
        middle | expanding_width(0) | bg(Color::Light_gray);
        right_btn | fixed_width(1);
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto horizontal_scrollbar(Args&&... args)
    -> std::unique_ptr<Horizontal_scrollbar>
{
    return std::make_unique<Horizontal_scrollbar>(std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HORIZONTAL_SCROLLBAR
