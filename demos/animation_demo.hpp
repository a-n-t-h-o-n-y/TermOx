#ifndef TERMOX_DEMOS_ANIMATED_WIDGET_HPP
#define TERMOX_DEMOS_ANIMATED_WIDGET_HPP
#include <termox/termox.hpp>
#include "termox/widget/bordered.hpp"

namespace demo {

/// Box containing a circle character that bounces off the Widget's walls.
class Animated_box : public ox::Widget {
   public:
    /// Initialize with the frames-per-second the animation will happen in.
    explicit Animated_box(ox::FPS fps) : fps_{fps}
    {
        *this | ox::pipe::strong_focus();
    }

   protected:
    auto timer_event() -> bool override
    {
        flip_increment(xy_.x, x_direction_, this->width());
        flip_increment(xy_.y, y_direction_, this->height());
        this->update();
        return Widget::timer_event();
    }

    auto paint_event(ox::Painter& p) -> bool override
    {
        p.put(glyph_, xy_);
        return Widget::paint_event(p);
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

    auto mouse_press_event(const ox::Mouse& m) -> bool override
    {
        xy_ = m.at;
        this->update();
        return Widget::mouse_press_event(m);
    }

    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override
    {
        reset(xy_.x, this->width());
        reset(xy_.y, this->height());
        return Widget::resize_event(new_size, old_size);
    }

   private:
    ox::FPS const fps_;

    ox::Glyph glyph_ = U'○' | fg(ox::Color::Yellow);
    ox::Point xy_    = ox::Point{0, 0};
    int x_direction_ = 1;
    int y_direction_ = 1;

   private:
    /// Increments by \p direction, if 0 or \p max, flips the direction.
    /** \p limit is one past the last viable value. */
    static void flip_increment(int& value, int& direction, int limit)
    {
        if (limit == 1)
            return;
        if ((direction == -1 && value == 0) ||
            (direction == +1 && value + 1 == limit)) {
            direction *= -1;
        }
        value += direction;
    }

    /// Adjust a coord if the borders have moved so that it is out of bounds.
    /** \p limit is one past the last viable value. */
    static void reset(int& value, int limit)
    {
        if (value >= limit)
            value = (limit == 0 ? 0 : limit - 1);
    }
};

/// Contains a few Animated_boxes at various animation rates.
inline auto make_animation_demo() -> std::unique_ptr<ox::Widget>
{
    using namespace ox;
    using namespace ox::pipe;
    auto app = layout::horizontal<>(std::make_unique<Animated_box>(FPS{5}),
                                    std::make_unique<Bordered<Animated_box>>(
                                        ox::border::squared(), FPS{10}),
                                    std::make_unique<Animated_box>(FPS{20}),
                                    std::make_unique<Bordered<Animated_box>>(
                                        ox::border::squared(), FPS{30}),
                                    std::make_unique<Animated_box>(FPS{60}));
    app | direct_focus() | forward_focus(app->get_children().front());
    return app;
}

}  // namespace demo
#endif  // TERMOX_DEMOS_ANIMATED_WIDGET_HPP
