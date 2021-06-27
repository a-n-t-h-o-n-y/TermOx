#ifndef TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#define TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
#include <chrono>
#include <memory>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
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
                          Fade fade           = Fade::On);

    explicit Notify_light(Parameters p);

   public:
    /// Start emitting the on color for the set duration.
    void emit();

    /// Set the on and off Colors of the light.
    void set_display(Display d);

    /// Return the on and off Colors currently used.
    [[nodiscard]] auto get_display() const -> Display;

    /// Set the duration from the time emit is called to when the light is off.
    void set_duration(Duration_t d);

    /// Return the currently set duration.
    [[nodiscard]] auto get_duration() const -> Duration_t;

    /// Set the light to Fade or not.
    void set_fade(Fade f);

    /// Return the currently set Fade type.
    [[nodiscard]] auto get_fade() const -> Fade;

   protected:
    auto timer_event() -> bool override;

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
[[nodiscard]] auto notify_light(
    Notify_light::Display display     = Notify_light::Display{Color::Foreground,
                                                          Color::Background},
    Notify_light::Duration_t duration = Notify_light::default_duration,
    Notify_light::Fade fade           = Notify_light::Fade::On)
    -> std::unique_ptr<Notify_light>;

/// Helper function to create a Notify_light instance.
[[nodiscard]] auto notify_light(Notify_light::Parameters p)
    -> std::unique_ptr<Notify_light>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NOTIFY_LIGHT_HPP
