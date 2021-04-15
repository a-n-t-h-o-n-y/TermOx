#ifndef TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#define TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#include <chrono>
#include <memory>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// A 'light' that can be emitted for a given duration to visually notify.
class Notify_light : public Widget {
   public:
    struct Display {
        Color on;
        Color off;
    };

    using Duration_t = std::chrono::milliseconds;

    enum class Fade : bool { On, Off };

    struct Parameters {
        Display display     = {Color::Foreground, Color::Background};
        Duration_t duration = default_duration;
        Fade fade           = Fade::On;
    };

    static auto constexpr default_duration = Duration_t{450};

   public:
    explicit Notify_light(Display display     = Display{Color::Foreground,
                                                    Color::Background},
                          Duration_t duration = default_duration,
                          Fade fade           = Fade::On)
        : display_{display},
          duration_{duration},
          fade_{fade},
          on_block_{initial_block | fg(display_.on) | bg(display_.off)},
          off_block_{initial_block | fg(display_.off)}
    {
        *this | pipe::fixed_height(1) | pipe::fixed_width(2);
        this->set_wallpaper(off_block_);
    }

    Notify_light(Parameters parameters)
        : Notify_light{parameters.display, parameters.duration, parameters.fade}
    {}

   public:
    /// Start emitting the on color for the set duration.
    void emit()
    {
        this->Widget::disable_animation();
        on_block_.symbol = U' ';
        this->Widget::enable_animation(duration_ / block_count);
    }

    /// Set the on and off Colors of the light.
    void set_display(Display d)
    {
        display_ = d;

        on_block_ | fg(display_.on) | bg(display_.off);
        off_block_ | fg(display_.off);
    }

    /// Return the on and off Colors currently used.
    [[nodiscard]] auto get_display() const -> Display { return display_; }

    /// Set the duration from the time emit is called to when the light is off.
    void set_duration(Duration_t d) { duration_ = d; }

    /// Return the currently set duration.
    [[nodiscard]] auto get_duration() const -> Duration_t { return duration_; }

    /// Set the light to Fade or not.
    void set_fade(Fade f) { fade_ = f; }

    /// Return the currently set Fade type.
    [[nodiscard]] auto get_fade() const -> Fade { return fade_; }

   protected:
    auto timer_event() -> bool override
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

   private:
    Display display_;
    Duration_t duration_;
    Fade fade_;

    Glyph on_block_;
    Glyph off_block_;

    static auto constexpr initial_block = U'█';
    static auto constexpr block_count   = 4;
};

/// Helper function to create a Notify_light instance.
[[nodiscard]] inline auto notify_light(
    Notify_light::Display display     = Notify_light::Display{Color::Foreground,
                                                          Color::Background},
    Notify_light::Duration_t duration = Notify_light::default_duration,
    Notify_light::Fade fade           = Notify_light::Fade::On)
    -> std::unique_ptr<Notify_light>
{
    return std::make_unique<Notify_light>(display, duration, fade);
}

/// Helper function to create a Notify_light instance.
[[nodiscard]] inline auto notify_light(Notify_light::Parameters parameters)
    -> std::unique_ptr<Notify_light>
{
    return std::make_unique<Notify_light>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
