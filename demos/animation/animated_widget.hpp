#ifndef CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#define CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos::animation {

class Animated_box : public cppurses::Widget {
   public:
    /// Initialize with the frames-per-second the animation will happen in.
    explicit Animated_box(cppurses::FPS fps) : fps_{fps}
    {
        *this | cppurses::pipe::strong_focus();
    }

   protected:
    auto timer_event() -> bool override;

    auto paint_event() -> bool override
    {
        cppurses::Painter{*this}.put(glyph_, coords_);
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

    auto mouse_press_event(const cppurses::Mouse::State& mouse) -> bool override
    {
        coords_ = mouse.local;
        this->update();
        return Widget::mouse_press_event(mouse);
    }

   private:
    cppurses::Glyph glyph_{'X', cppurses::foreground(cppurses::Color::Yellow)};
    cppurses::Point coords_{0, 0};
    int x_direction = 1;
    int y_direction = 1;

    cppurses::FPS const fps_;
};

class Animated_widget : public cppurses::layout::Horizontal<Animated_box> {
   public:
    Animated_widget()
    {
        box_2_.border.enable();
        box_4_.border.enable();
    }

   private:
    Animated_box& box_1_ = this->make_child(cppurses::FPS{5});
    Animated_box& box_2_ = this->make_child(cppurses::FPS{10});
    Animated_box& box_3_ = this->make_child(cppurses::FPS{20});
    Animated_box& box_4_ = this->make_child(cppurses::FPS{30});
    Animated_box& box_5_ = this->make_child(cppurses::FPS{60});
};

}  // namespace demos::animation
#endif  // CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
