#ifndef TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#define TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#include <chrono>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// A 'light' that can be emitted for a given duration to visually notify.
class Notify_light : public Widget {
   public:
    using Duration_t = std::chrono::milliseconds;

    struct Display {
        Color on;
        Color off;
    };

    enum class Fade : bool { On, Off };

    struct Parameters {
        Display display     = {Color::Foreground, Color::Background};
        Duration_t duration = default_duration;
        Fade fade           = Fade::On;
    };

   public:
    explicit Notify_light(Display display     = Display{Color::Foreground,
                                                    Color::Background},
                          Duration_t duration = default_duration,
                          Fade fade           = Fade::On)
        : Notify_light{Parameters{display, duration, fade}}
    {}

    Notify_light(Parameters p)
        : parameters_{std::move(p)},
          on_block_{initial_block | fg(parameters_.display.on) |
                    bg(parameters_.display.off)},
          off_block_{initial_block | fg(parameters_.display.off)}
    {
        *this | pipe::fixed_height(1) | pipe::fixed_width(2);
        this->set_wallpaper(off_block_);
    }

   public:
    /// Start emitting the on color for the set duration.
    void emit()
    {
        this->Widget::disable_animation();
        on_block_.symbol = L' ';
        this->Widget::enable_animation(parameters_.duration / block_count);
    }

    /// Set the on and off Colors of the light.
    void set_display(Display d)
    {
        parameters_.display = d;

        on_block_ | fg(parameters_.display.on) | bg(parameters_.display.off);
        off_block_ | fg(parameters_.display.off);
    }

    /// Return the on and off Colors currently used.
    auto get_display() const -> Display { return parameters_.display; }

    /// Set the duration from the time emit is called to when the light is off.
    void set_duration(Duration_t d) { parameters_.duration = d; }

    /// Return the currently set duration.
    auto get_duration() const -> Duration_t { return parameters_.duration; }

    /// Set the light to Fade or not.
    void set_fade(Fade f) { parameters_.fade = f; }

    /// Return the currently set Fade type.
    auto get_fade() const -> Fade { return parameters_.fade; }

   protected:
    auto timer_event() -> bool override
    {
        auto constexpr first  = initial_block;
        auto constexpr second = L'▓';
        auto constexpr third  = L'▒';
        auto constexpr fourth = L'░';
        auto constexpr empty  = L' ';

        switch (on_block_.symbol) {
            case empty: on_block_.symbol = first; break;
            case first: on_block_.symbol = second; break;
            case second: on_block_.symbol = third; break;
            case third: on_block_.symbol = fourth; break;
            case fourth: on_block_.symbol = empty; break;
            default: break;
        }

        if (parameters_.fade == Fade::On)
            this->set_wallpaper(on_block_);
        else if (on_block_.symbol == empty)
            this->set_wallpaper(off_block_);
        else
            this->set_wallpaper(initial_block | fg(parameters_.display.on));

        if (on_block_.symbol == empty)
            this->Widget::disable_animation();

        return Widget::timer_event();
    }

   private:
    Parameters parameters_;
    Glyph on_block_;
    Glyph off_block_;

    static auto constexpr initial_block    = L'█';
    static auto constexpr block_count      = 4;
    static auto constexpr default_duration = Duration_t{450};
};

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
