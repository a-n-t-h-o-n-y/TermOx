#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <locale>
#include <ox/put.hpp>
#include <ox/selection.hpp>
#include <ox/value.hpp>
#include <sstream>
namespace ox {
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
auto value_string(double v) -> std::string
{
    char buffer[128];
    auto [end, error] = std::to_chars(buffer, buffer + sizeof(buffer), v);
    if (error != std::errc{}) { throw std::runtime_error{"Number formatting failed"}; }
    return {buffer, end};
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
    auto s = appearance(style);
    if (is_focused()) { s = s | Trait::Bold; }
    fill(c, s);
    auto n = vertical ? c.size().height : c.size().width;
    if (!n) { return {}; }
    auto at = (int)std::llround(value_fraction(value_, range_) * (n - 1));
    if (vertical) { at = n - 1 - at; }
    for (int i = 0; i < n; ++i) {
        c.write(vertical ? Point{0, i} : Point{i, 0},
                i == at ? U"●" : (vertical ? U"│" : U"─"), s);
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
NumericEdit::NumericEdit() : NumericEdit{Options{}} {}
NumericEdit::NumericEdit(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
      editor{std::move(x.editor)},
      range_{x.range},
      value_{clamp_value(x.value, x.range)}
{
    sync();
    Connection{.signal = editor.on_submit,
               .slot = [](auto const&, NumericEdit& self) { self.commit(); }}(*this);
    // Focus is owned by the editor. Scoped bindings leave editing keys to LineEdit.
    auto lifetime = track(*this);
    editor.shortcuts = [lifetime](KeyEvent const& e) {
        auto* self = lifetime.try_get();
        return self && self->editor.is_focused() ? self->handle(e) : EventResponse{};
    };
}
void NumericEdit::sync() { editor.set_text(value_string(value_)); }
auto NumericEdit::set_value(double v) -> bool
{
    v = clamp_value(v, range_);
    auto changed = std::exchange(value_, v) != v;
    auto lifetime = track(*this);
    sync();
    if (auto* self = lifetime.try_get(); self && changed && self->value_ == v) {
        self->on_change(v);
    }
    return changed;
}
auto NumericEdit::set_range(ValueRange r) -> bool
{
    validate(r);
    if (r == range_) return false;
    range_ = r;
    set_value(value_);
    return true;
}
auto NumericEdit::commit() -> bool
{
    auto const& text = editor.text();
    double v = 0;
    auto input = std::istringstream{text};
    input.imbue(std::locale::classic());
    input >> std::noskipws >> v;
    if (!input || !input.eof() || !std::isfinite(v) || v < range_.minimum ||
        v > range_.maximum) {
        auto copy = text;
        on_invalid(copy);
        return false;
    }
    auto lifetime = track(*this);
    set_value(v);
    return lifetime.valid();
}
void NumericEdit::revert() { sync(); }
auto NumericEdit::measure(Area a, esc::WidthPolicy p) const -> Area
{ return editor.measure(a, p); }
void NumericEdit::on_arrange(Rect) { arrange(editor, {{}, size()}); }
auto NumericEdit::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled() || editor.read_only || e.action == KeyAction::Release) return {};
    if (e.matches(KeyCode::Escape, {}, KeyAction::Press)) {
        revert();
        return {.handled = true, .repaint = true};
    }
    if (e.matches(KeyCode::ArrowUp, {}, e.action) ||
        e.matches(KeyCode::ArrowDown, {}, e.action)) {
        return {
            .handled = true,
            .repaint = set_value(increment(
                value_, range_, e.matches(KeyCode::ArrowUp, {}, e.action) ? 1 : -1))};
    }
    return {};
}
ProgressBar::ProgressBar() : ProgressBar{Options{}} {}
ProgressBar::ProgressBar(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
      style{x.style},
      show_percentage{x.show_percentage},
      range_{x.range},
      value_{clamp_value(x.value, x.range)}
{}
auto ProgressBar::set_value(double v) -> bool
{
    v = clamp_value(v, range_);
    if (std::exchange(value_, v) == v) return false;
    request_repaint();
    on_change(v);
    return true;
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
    auto s = appearance(style);
    fill(c, s);
    auto f = value_fraction(value_, range_);
    auto filled = (int)std::llround(f * c.size().width);
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
      timer_{*this, x.interval, [](Spinner& s) { return s.tick(); }, x.running}
{ set_frames(std::move(x.frames)); }
auto Spinner::is_running() const -> bool { return timer_.is_running(); }
auto Spinner::set_running(bool run) -> bool
{
    if (run == is_running()) return false;
    if (run)
        timer_.start();
    else
        timer_.stop();
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
    if (!is_running() || frames_.empty()) return {};
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
        paint_row_text(c, frames_[frame_], Align::Left, appearance(style), layout_);
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
