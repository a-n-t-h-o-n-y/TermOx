#pragma once
#include <chrono>
#include <ox/value_range.hpp>
#include <ox/checkbox.hpp>
#include <ox/lineedit.hpp>
#include <ox/timer.hpp>
namespace ox {
/** Fractional block fill, clipped to the supplied canvas. Fractions clamp to [0,1]. */
void paint_meter(Canvas,
                 double fraction,
                 Style fill_style,
                 Style track_style,
                 bool vertical = false,
                 bool ascii = false);
void validate(ValueRange);
auto clamp_value(double, ValueRange) -> double;
auto value_fraction(double, ValueRange) -> double;
class Slider : public Widget {
   public:
    using Widget::handle;
    struct Options {
        ValueRange range = {};
        double value = 0;
        bool vertical = false;
        Style style = {};
    };
    bool vertical;
    Style style;
    sl::Signal<void(double)> on_change;
    Slider();
    explicit Slider(Options);
    Slider(Slider&&);
    auto operator=(Slider&&) -> Slider&;
    auto range() const -> ValueRange { return range_; }
    auto value() const -> double { return value_; }
    auto set_value(double) -> bool;
    auto set_range(ValueRange) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto handle(MouseRelease const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;
    auto focus_out() -> EventResponse override { return pointer_cancel(); }

   private:
    ValueRange range_;
    double value_;
    bool dragging_ = false;
    auto point_value(Point) const -> double;
};
class ProgressBar : public Widget {
   public:
    struct Options {
        ValueRange range = {};
        double value = 0;
        Style style = {};
        bool show_percentage = true;
    };
    Style style;
    bool show_percentage;
    sl::Signal<void(double)> on_change;
    ProgressBar();
    explicit ProgressBar(Options);
    auto range() const -> ValueRange { return range_; }
    auto value() const -> double { return value_; }
    auto set_value(double) -> bool;
    auto set_range(ValueRange) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    ValueRange range_;
    double value_;
    Animation display_{*this};
};
/** Animated status indicator. No timer runs while stopped; timers follow widget moves.
 */
class Spinner : public Widget {
   public:
    struct Options {
        std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                           "⠴", "⠦", "⠧", "⠇", "⠏"};
        std::chrono::milliseconds interval{80};
        bool running = true;
        Style style = {};
    };
    Style style;
    sl::Signal<void(bool)> on_change;
    Spinner();
    explicit Spinner(Options);
    auto is_running() const -> bool;
    auto set_running(bool) -> bool;
    void set_frames(std::vector<std::string>);
    auto frames() const -> std::span<std::string const> { return frames_; }
    auto tick() -> EventResponse;
    void theme_changed() override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    bool running_ = true;
    std::vector<std::string> frames_;
    std::size_t frame_ = 0;
    Timer timer_;
    TextLayout layout_;
};
/** Same boolean contract and activation bindings as CheckBox, with switch presentation.
 */
class Switch : public CheckBox {
   public:
    Switch();
    explicit Switch(CheckBox::Options);
};
}  // namespace ox

#include <ox/numeric_edit.hpp>
