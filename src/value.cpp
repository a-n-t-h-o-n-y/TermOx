#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <locale>
#include <ox/put.hpp>
#include <ox/rule.hpp>
#include <ox/selection.hpp>
#include <ox/value.hpp>
#include <sstream>
namespace ox {
void paint_meter(Canvas canvas,
                 double fraction,
                 Style filled,
                 Style track,
                 bool vertical,
                 bool ascii)
{
    if (!std::isfinite(fraction)) throw std::invalid_argument{"Nonfinite meter value"};
    fraction = std::clamp(fraction, 0., 1.);
    fill(canvas, U' ' | track);
    auto n = vertical ? canvas.size().height : canvas.size().width;
    auto levels = std::u32string_view{vertical ? U" ▁▂▃▄▅▆▇█" : U" ▏▎▍▌▋▊▉█"};
    auto amount = fraction * n;
    for (int i = 0; i < n; ++i) {
        auto coverage = std::clamp(amount - i, 0., 1.);
        auto glyph = ascii ? (coverage >= .5 ? U'#' : U'-')
                           : levels[(std::size_t)std::lround(coverage * 8)];
        auto at = vertical ? Point{0, n - 1 - i} : Point{i, 0};
        auto level = (int)std::lround(coverage * 8);
        if (ascii ? coverage >= .5 : level > 0)
            canvas.write(at, std::u32string_view{&glyph, 1}, filled);
        else {
            auto stroke = ascii ? (vertical ? U'|' : U'-') : (vertical ? U'│' : U'─');
            if (!ascii && n > 1) {
                if (vertical && at.y == 0)
                    stroke = U'╷';
                else if (vertical && at.y == n - 1)
                    stroke = U'╵';
                else if (!vertical && at.x == 0)
                    stroke = U'╶';
                else if (!vertical && at.x == n - 1)
                    stroke = U'╴';
            }
            canvas.write(at, std::u32string_view{&stroke, 1}, track);
        }
    }
}
void validate(ValueRange r)
{
    if (!std::isfinite(r.minimum) || !std::isfinite(r.maximum) ||
        !std::isfinite(r.step) || r.minimum > r.maximum || r.step <= 0) {
        throw std::invalid_argument{"Invalid value range"};
    }
}
auto clamp_value(double v, ValueRange r) -> double
{
    validate(r);
    if (!std::isfinite(v)) { throw std::invalid_argument{"Value must be finite"}; }
    return std::clamp(v, r.minimum, r.maximum);
}
auto value_fraction(double v, ValueRange r) -> double
{
    v = clamp_value(v, r);
    if (r.minimum == r.maximum) { return 1; }
    if (r.minimum >= 0 || r.maximum <= 0) {
        return (v - r.minimum) / (r.maximum - r.minimum);
    }
    // Divide first to avoid overflow for opposite finite extremes.
    auto scale = std::max(std::abs(r.minimum), std::abs(r.maximum));
    return (v / scale - r.minimum / scale) / (r.maximum / scale - r.minimum / scale);
}
namespace {
auto increment(double v, ValueRange r, double steps) -> double
{
    auto next = (long double)v + (long double)r.step * steps;
    return (double)std::clamp(next, (long double)r.minimum, (long double)r.maximum);
}
}  // namespace
Slider::Slider() : Slider{Options{}} {}
Slider::Slider(Options x)
    : Widget{FocusPolicy::Strong,
             x.vertical ? SizePolicy::fixed(1) : SizePolicy::flex(),
             x.vertical ? SizePolicy::flex() : SizePolicy::fixed(1)},
      vertical{x.vertical},
      style{x.style},
      range_{x.range},
      value_{clamp_value(x.value, x.range)}
{}
Slider::Slider(Slider&& other)
    : Widget{std::move(other)},
      vertical{other.vertical},
      style{other.style},
      on_change{std::move(other.on_change)},
      range_{other.range_},
      value_{other.value_},
      dragging_{std::exchange(other.dragging_, false)}
{}
auto Slider::operator=(Slider&& other) -> Slider&
{
    if (this == &other) { return *this; }
    Widget::operator=(std::move(other));
    vertical = other.vertical;
    style = other.style;
    on_change = std::move(other.on_change);
    range_ = other.range_;
    value_ = other.value_;
    dragging_ = std::exchange(other.dragging_, false);
    return *this;
}
auto Slider::set_value(double v) -> bool
{
    v = clamp_value(v, range_);
    if (std::exchange(value_, v) == v) { return false; }
    request_repaint();
    on_change(v);
    return true;
}
auto Slider::set_range(ValueRange r) -> bool
{
    validate(r);
    if (r == range_) { return false; }
    range_ = r;
    request_repaint();
    set_value(value_);
    return true;
}
auto Slider::measure(Area a, esc::WidthPolicy) const -> Area
{
    validate(a);
    return vertical ? Area{1, 5} : Area{10, 1};
}
auto Slider::paint(Canvas c) -> Cursor
{
    auto s = appearance(style, Surface::Accent);
    if (is_focused()) { s = s | Trait::Bold; }
    fill(c, s);
    auto n = vertical ? c.size().height : c.size().width;
    if (!n) { return {}; }
    auto at = (int)std::llround(value_fraction(value_, range_) * (n - 1));
    if (vertical) { at = n - 1 - at; }
    if (theme().styled) {
        paint_rule(c, vertical ? at : 0, vertical ? n - at : at + 1, s,
                   appearance({}, Surface::Border), vertical, theme().ascii);
        c.write(vertical ? Point{0, at} : Point{at, 0}, theme().ascii ? U"o" : U"●", s);
        return {};
    }
    for (int i = 0; i < n; ++i) {
        c.write(vertical ? Point{0, i} : Point{i, 0},
                theme().styled && theme().ascii
                    ? (i == at ? U"o" : (vertical ? U"|" : U"-"))
                    : (i == at ? U"●" : (vertical ? U"│" : U"─")),
                s);
    }
    return {};
}
auto Slider::point_value(Point p) const -> double
{
    auto n = vertical ? size().height : size().width;
    if (n <= 1) { return range_.minimum; }
    auto at = std::clamp(vertical ? p.y : p.x, 0, n - 1);
    auto fraction = (long double)(vertical ? n - 1 - at : at) / (n - 1);
    return (double)((1 - fraction) * range_.minimum + fraction * range_.maximum);
}
auto Slider::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled() || e.action == KeyAction::Release || e.modifiers.ctrl ||
        e.modifiers.alt || e.modifiers.meta || e.modifiers.super || e.modifiers.hyper) {
        return {};
    }
    auto steps = 0;
    if (e.matches(KeyCode::ArrowRight, {}, e.action) ||
        e.matches(KeyCode::ArrowUp, {}, e.action)) {
        steps = 1;
    }
    if (e.matches(KeyCode::ArrowLeft, {}, e.action) ||
        e.matches(KeyCode::ArrowDown, {}, e.action)) {
        steps = -1;
    }
    if (e.matches(KeyCode::PageUp, {}, e.action)) { steps = 10; }
    if (e.matches(KeyCode::PageDown, {}, e.action)) { steps = -10; }
    if (e.matches(KeyCode::Home, {}, e.action)) {
        return {.handled = true, .repaint = set_value(range_.minimum)};
    }
    if (e.matches(KeyCode::End, {}, e.action)) {
        return {.handled = true, .repaint = set_value(range_.maximum)};
    }
    return steps ? EventResponse{.handled = true,
                                 .repaint = set_value(increment(value_, range_, steps))}
                 : EventResponse{};
}
auto Slider::handle(MousePress const& e) -> EventResponse
{
    if (!is_enabled() || e.button != MouseButton::Left ||
        !contains({{}, size()}, e.at)) {
        return {};
    }
    dragging_ = true;
    capture_pointer();
    return {.handled = true, .repaint = set_value(point_value(e.at))};
}
auto Slider::handle(MouseMove const& e) -> EventResponse
{
    return dragging_ && is_enabled()
               ? EventResponse{.handled = true, .repaint = set_value(point_value(e.at))}
               : EventResponse{};
}
auto Slider::handle(MouseRelease const& e) -> EventResponse
{
    if (!dragging_ || (e.button && e.button != MouseButton::Left)) { return {}; }
    dragging_ = false;
    release_pointer();
    return {.handled = true, .repaint = is_enabled() && set_value(point_value(e.at))};
}
auto Slider::pointer_cancel() -> EventResponse
{
    auto was = std::exchange(dragging_, false);
    release_pointer();
    return {.handled = was};
}
ProgressBar::ProgressBar() : ProgressBar{Options{}} {}
ProgressBar::ProgressBar(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
      style{x.style},
      show_percentage{x.show_percentage},
      range_{x.range},
      value_{clamp_value(x.value, x.range)}
{ display_.reset(value_fraction(value_, range_)); }
auto ProgressBar::set_value(double v) -> bool
{
    v = clamp_value(v, range_);
    auto changed = std::exchange(value_, v) != v;
    display_.to(
        value_fraction(v, range_),
        {theme().styled ? theme().motion.progress : std::chrono::milliseconds{0},
         Ease::Spring});
    if (changed) {
        request_repaint();
        on_change(v);
    }
    return changed;
}
auto ProgressBar::set_range(ValueRange r) -> bool
{
    validate(r);
    if (r == range_) return false;
    range_ = r;
    request_repaint();
    set_value(value_);
    return true;
}
auto ProgressBar::measure(Area a, esc::WidthPolicy) const -> Area
{
    validate(a);
    return {10, 1};
}
auto ProgressBar::paint(Canvas c) -> Cursor
{
    auto s = appearance(style, Surface::Accent);
    fill(c, s);
    auto f = value_fraction(value_, range_);
    if (theme().styled) {
        auto visual = display_.active() ? display_.value() : f;
        paint_meter(c, visual, s, appearance({}, Surface::Border), false,
                    theme().ascii);
    }
    auto filled = (int)std::llround(f * c.size().width);
    if (!theme().styled)
        for (int x = 0; x < c.size().width; ++x)
            c.write({x, 0}, x < filled ? U"█" : U"░", s);
    if (show_percentage) {
        auto label = std::to_string((int)std::llround(f * 100)) + "%";
        put(c, {std::max(0, (c.size().width - (int)label.size()) / 2), 0},
            std::string_view{label}, s);
    }
    return {};
}
Spinner::Spinner() : Spinner{Options{}} {}
Spinner::Spinner(Options x)
    : Widget{FocusPolicy::None, SizePolicy{.flexibility = 0}, SizePolicy::fixed(1)},
      style{x.style},
      running_{x.running},
      timer_{*this, x.interval, [](Spinner& s) { return s.tick(); }, x.running}
{ set_frames(std::move(x.frames)); }
auto Spinner::is_running() const -> bool { return running_; }
void Spinner::theme_changed()
{
    if (running_ && theme().motion.enabled) {
        if (!timer_.is_running()) timer_.start();
    }
    else
        timer_.stop();
}
auto Spinner::set_running(bool run) -> bool
{
    if (run == running_) return false;
    running_ = run;
    theme_changed();
    request_repaint();
    on_change(run);
    return true;
}
void Spinner::set_frames(std::vector<std::string> f)
{
    if (f.empty()) throw std::invalid_argument{"Spinner needs frames"};
    for (auto const& v : f)
        validate_row_text(v);
    frames_ = std::move(f);
    frame_ = 0;
    refresh();
}
auto Spinner::tick() -> EventResponse
{
    if (!running_ || !theme().motion.enabled || frames_.empty()) return {};
    frame_ = (frame_ + 1) % frames_.size();
    return {.repaint = true};
}
auto Spinner::measure(Area a, esc::WidthPolicy p) const -> Area
{
    validate(a);
    auto width = 0;
    for (auto const& f : frames_)
        width = std::max(width, measure_text(f, a, TextWrap::None, p).width);
    return {width, 1};
}
auto Spinner::paint(Canvas c) -> Cursor
{
    if (!frames_.empty())
        paint_row_text(c,
                       theme().styled && theme().ascii
                           ? std::string_view{"*"}
                           : std::string_view{frames_[frame_]},
                       Align::Left, appearance(style, Surface::Accent), layout_);
    return {};
}
Switch::Switch() : Switch{CheckBox::Options{}} {}
Switch::Switch(CheckBox::Options x)
    : CheckBox{[&] {
          x.checked_glyphs = U"[━●]"_gs;
          x.unchecked_glyphs = U"[●━]"_gs;
          return std::move(x);
      }()}
{}
}  // namespace ox
