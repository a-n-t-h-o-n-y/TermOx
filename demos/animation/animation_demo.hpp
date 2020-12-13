#ifndef CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#define CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/mouse.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos::animation {

/// Box containing an 'x' character that bounces off the Widget's walls.
class Animated_box : public cppurses::Widget {
   public:
    /// Initialize with the frames-per-second the animation will happen in.
    explicit Animated_box(cppurses::FPS fps) : fps_{fps}
    {
        *this | cppurses::pipe::strong_focus();
    }

   protected:
    auto timer_event() -> bool override
    {
        flip_increment(xy_.x, x_direction_, this->width());
        flip_increment(xy_.y, y_direction_, this->height());
        this->update();
        return Widget::timer_event();
    }

    auto paint_event() -> bool override
    {
        cppurses::Painter{*this}.put(glyph_, xy_);
        return Widget::paint_event();
    }

    auto enable_event() -> bool override
    {
        this->enable_animation(fps_);
        return Widget::enable_event();
    }

    auto disable_event() -> bool override
    {
        this->disable_animation();
        return Widget::disable_event();
    }

    auto mouse_press_event(const cppurses::Mouse& m) -> bool override
    {
        xy_ = m.local;
        this->update();
        return Widget::mouse_press_event(m);
    }

    auto resize_event(cppurses::Area new_size, cppurses::Area old_size)
        -> bool override
    {
        reset(xy_.x, this->width());
        reset(xy_.y, this->height());
        return Widget::resize_event(new_size, old_size);
    }

   private:
    cppurses::FPS const fps_;

    cppurses::Glyph glyph_ = L'X' | fg(cppurses::Color::Yellow);
    cppurses::Point xy_    = cppurses::Point{0uL, 0uL};
    int x_direction_       = 1;
    int y_direction_       = 1;

   private:
    /// Increments by \p direction, if 0 or \p max, flips the direction.
    /** \p limit is one past the last viable value. */
    static void flip_increment(std::size_t& value,
                               int& direction,
                               std::size_t limit)
    {
        if (limit == 1uL)
            return;
        if ((direction == -1 && value == 0uL) ||
            (direction == +1 && value + 1 == limit)) {
            direction *= -1;
        }
        value += direction;
    }

    /// Adjust a coord if the borders have moved so that it is out of bounds.
    /** \p limit is one past the last viable value. */
    static void reset(std::size_t& value, std::size_t limit)
    {
        if (value >= limit)
            value = (limit == 0uL ? 0uL : limit - 1uL);
    }
};

/// Contains a few Animated_boxes at various animation rates.
inline auto build_demo() -> std::unique_ptr<cppurses::Widget>
{
    using namespace cppurses;
    return layout::horizontal<Animated_box>(
        std::make_unique<Animated_box>(FPS{5}),
        std::make_unique<Animated_box>(FPS{10}) | pipe::bordered(),
        std::make_unique<Animated_box>(FPS{20}),
        std::make_unique<Animated_box>(FPS{30}) | pipe::bordered(),
        std::make_unique<Animated_box>(FPS{60}));
}

}  // namespace demos::animation
#endif  // CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
