#include <termox/widget/widgets/notify_light.hpp>

#include <memory>
#include <utility>

#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

Notify_light::Notify_light(Display display, Duration_t duration, Fade fade)
    : display_{display},
      duration_{duration},
      fade_{fade},
      on_block_{initial_block | fg(display_.on) | bg(display_.off)},
      off_block_{initial_block | fg(display_.off)}
{
    *this | pipe::fixed_height(1) | pipe::fixed_width(2);
    this->set_wallpaper(off_block_);
}

Notify_light::Notify_light(Parameters p)
    : Notify_light{p.display, p.duration, p.fade}
{}

void Notify_light::emit()
{
    this->Widget::disable_animation();
    on_block_.symbol = U' ';
    this->Widget::enable_animation(duration_ / block_count);
}

void Notify_light::set_display(Display d)
{
    display_ = d;

    on_block_ |= bg(display_.off);
    on_block_ |= fg(display_.on);
    off_block_ |= fg(display_.off);
}

auto Notify_light::get_display() const -> Display { return display_; }

void Notify_light::set_duration(Duration_t d) { duration_ = d; }

auto Notify_light::get_duration() const -> Duration_t { return duration_; }

void Notify_light::set_fade(Fade f) { fade_ = f; }

auto Notify_light::get_fade() const -> Fade { return fade_; }

auto Notify_light::timer_event() -> bool
{
    auto constexpr first  = initial_block;
    auto constexpr second = U'▓';
    auto constexpr third  = U'▒';
    auto constexpr fourth = U'░';
    auto constexpr empty  = U' ';

    switch (on_block_.symbol) {
        case empty: on_block_.symbol = first; break;
        case first: on_block_.symbol = second; break;
        case second: on_block_.symbol = third; break;
        case third: on_block_.symbol = fourth; break;
        case fourth: on_block_.symbol = empty; break;
        default: break;
    }

    if (fade_ == Fade::On)
        this->set_wallpaper(on_block_);
    else if (on_block_.symbol == empty)
        this->set_wallpaper(off_block_);
    else
        this->set_wallpaper(initial_block | fg(display_.on));

    if (on_block_.symbol == empty)
        this->Widget::disable_animation();

    return Widget::timer_event();
}

auto notify_light(Notify_light::Display display,
                  Notify_light::Duration_t duration,
                  Notify_light::Fade fade) -> std::unique_ptr<Notify_light>
{
    return std::make_unique<Notify_light>(display, duration, fade);
}

auto notify_light(Notify_light::Parameters p) -> std::unique_ptr<Notify_light>
{
    return std::make_unique<Notify_light>(std::move(p));
}

}  // namespace ox
