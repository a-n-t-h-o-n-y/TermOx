/** @file
 * Resizable pairs of child panes with pointer and keyboard control.
 */
#pragma once
#include <cmath>
#include <ox/put.hpp>
#include <ox/rule.hpp>
#include <ox/widget.hpp>
namespace ox {
/** Two owned panes, a captured divider, and a proportional split preserved on resize.
 */
template <WidgetDerived First, WidgetDerived Second>
class SplitPane : public Widget {
   public:
    using Widget::handle;
    /** Owned first child pane. */
    First first;
    /** Owned second child pane. */
    Second second;
    /** Stack panes top/bottom when true; otherwise place them side by side. */
    bool vertical = false;
    /** Nonnegative minimum extent of the first split pane. */
    int first_minimum = 0;
    /** Nonnegative minimum extent of the second split pane. */
    int second_minimum = 0;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style = {};
    /** Emits the normalized split ratio after set_ratio() or user dragging/keyboard
     * commands change it.
     */
    sl::Signal<void(double)> on_change;
    /** Retain two panes and a finite clamped initial ratio; size minimums are applied
     * during arrangement.
     */
    SplitPane(First first, Second second, double ratio = .5)
        : Widget{FocusPolicy::Strong},
          first{std::move(first)},
          second{std::move(second)}
    { set_ratio(ratio); }
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    SplitPane(SplitPane&& other)
        : Widget{std::move(other)},
          first{std::move(other.first)},
          second{std::move(other.second)},
          vertical{other.vertical},
          first_minimum{other.first_minimum},
          second_minimum{other.second_minimum},
          style{other.style},
          on_change{std::move(other.on_change)},
          ratio_{other.ratio_},
          divider_{other.divider_},
          dragging_{std::exchange(other.dragging_, false)}
    {}
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(SplitPane&& other) -> SplitPane&
    {
        if (this == &other) { return *this; }
        Widget::operator=(std::move(other));
        first = std::move(other.first);
        second = std::move(other.second);
        vertical = other.vertical;
        first_minimum = other.first_minimum;
        second_minimum = other.second_minimum;
        style = other.style;
        on_change = std::move(other.on_change);
        ratio_ = other.ratio_;
        divider_ = other.divider_;
        dragging_ = std::exchange(other.dragging_, false);
        return *this;
    }
    /** Return the requested split fraction, preserved across resize independently of
     * minimum-constrained pane geometry.
     */
    auto ratio() const -> double { return ratio_; }
    /** Clamp a finite ratio to [0,1], refresh pane geometry, and emit on_change only on change.
     * @throws std::invalid_argument The ratio is nonfinite.
     */
    auto set_ratio(double ratio) -> bool
    {
        if (!std::isfinite(ratio)) {
            throw std::invalid_argument{"Split ratio must be finite"};
        }
        ratio = std::clamp(ratio, 0., 1.);
        if (std::exchange(ratio_, ratio) == ratio) return false;
        auto lifetime = track(*this);
        refresh();
        if (auto* self = lifetime.try_get()) self->on_change(ratio);
        return true;
    }
    auto get_children() -> zzz::Generator<Widget&> override
    {
        co_yield first;
        co_yield second;
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        co_yield first;
        co_yield second;
    }
    void refresh() override
    {
        Widget::refresh();
        on_arrange(bounds());
    }
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    {
        validate(a);
        auto one = first.measure(a, p);
        auto two = second.measure(a, p);
        auto sum = (std::int64_t)(vertical ? one.height : one.width) +
                   (vertical ? two.height : two.width) + 1;
        if (sum > std::numeric_limits<int>::max())
            throw std::length_error{"Split extent overflow"};
        return vertical ? Area{std::max(one.width, two.width), (int)sum}
                        : Area{(int)sum, std::max(one.height, two.height)};
    }
    auto paint(Canvas c) -> Cursor override
    {
        auto s = appearance(style, Surface::Border);
        if (is_focused()) s = s | Trait::Bold;
        auto n = vertical ? size().width : size().height;
        if (theme().styled) {
            auto view = vertical ? c.subcanvas({0, divider_}, {size().width, 1})
                                 : c.subcanvas({divider_, 0}, {1, size().height});
            paint_rule(view, 0, is_focused() ? n : 0, s, s, !vertical, theme().ascii);
            return {};
        }
        for (int i = 0; i < n; ++i)
            c.write(vertical ? Point{i, divider_} : Point{divider_, i},
                    theme().styled && theme().ascii ? (vertical ? U"-" : U"|")
                                                    : (vertical ? U"─" : U"│"),
                    s);
        return {};
    }
    auto handle(MousePress const& e) -> EventResponse override
    {
        if (!is_enabled() || e.button != MouseButton::Left ||
            !contains({{}, size()}, e.at) || (vertical ? e.at.y : e.at.x) != divider_)
            return {};
        dragging_ = true;
        capture_pointer();
        return {.handled = true};
    }
    auto handle(MouseMove const& e) -> EventResponse override
    {
        if (!dragging_ || !is_enabled()) return {};
        auto n = (vertical ? size().height : size().width) - 1;
        return {
            .handled = true,
            .repaint = n > 0 && set_ratio((double)(vertical ? e.at.y : e.at.x) / n)};
    }
    auto handle(MouseRelease const& e) -> EventResponse override
    {
        if (!dragging_ || (e.button && e.button != MouseButton::Left)) return {};
        dragging_ = false;
        release_pointer();
        return {.handled = true};
    }
    auto pointer_cancel() -> EventResponse override
    {
        auto was = std::exchange(dragging_, false);
        release_pointer();
        return {.handled = was};
    }
    auto focus_out() -> EventResponse override { return pointer_cancel(); }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (!is_enabled() || !is_focused() || e.action == KeyAction::Release) return {};
        if (e.matches(vertical ? KeyCode::ArrowUp : KeyCode::ArrowLeft, {}, e.action))
            return {.handled = true, .repaint = set_ratio(ratio_ - .05)};
        if (e.matches(vertical ? KeyCode::ArrowDown : KeyCode::ArrowRight, {},
                      e.action))
            return {.handled = true, .repaint = set_ratio(ratio_ + .05)};
        if (e.matches(KeyCode::Home, {}, e.action))
            return {.handled = true, .repaint = set_ratio(0)};
        if (e.matches(KeyCode::End, {}, e.action))
            return {.handled = true, .repaint = set_ratio(1)};
        return {};
    }

   protected:
    void on_arrange(Rect) override
    {
        if (first_minimum < 0 || second_minimum < 0)
            throw std::invalid_argument{"Negative pane minimum"};
        auto n = std::max(0, (vertical ? size().height : size().width) - 1);
        auto desired = (int)std::llround(n * ratio_);
        if ((std::int64_t)first_minimum + second_minimum <= n)
            divider_ = std::clamp(desired, first_minimum, n - second_minimum);
        else
            divider_ = (int)((std::int64_t)n * first_minimum /
                             std::max<std::int64_t>(
                                 1, (std::int64_t)first_minimum + second_minimum));
        auto after = std::min(divider_ + 1, vertical ? size().height : size().width);
        auto lifetime = track(*this);
        arrange(first, {{},
                        vertical ? Area{size().width, divider_}
                                 : Area{divider_, size().height}});
        if (auto* self = lifetime.try_get())
            arrange(self->second,
                    {self->vertical ? Point{0, after} : Point{after, 0},
                     self->vertical ? Area{self->size().width, n - self->divider_}
                                    : Area{n - self->divider_, self->size().height}});
    }

   private:
    double ratio_ = .5;
    int divider_ = 0;
    bool dragging_ = false;
};
}  // namespace ox
