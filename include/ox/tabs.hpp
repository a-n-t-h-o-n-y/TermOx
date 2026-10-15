#pragma once
#include <cmath>
#include <ox/container.hpp>
#include <ox/routing.hpp>
#include <ox/rule.hpp>
#include <ox/selection.hpp>
namespace ox {
/** A tab strip plus a Stack of ordinary tuple/range pages. */
template <LayoutContainer Container>
class Tabs : public Widget {
   public:
    using Widget::handle;
    Stack<Container> pages;
    Style style = {};
    StylePatch selected_style = {.background = {},
                                 .foreground = {},
                                 .enabled = Trait::Inverse,
                                 .disabled = {}};
    StylePatch unselected_style = {};
    StylePatch indicator_style = {};
    StylePatch track_style = {};
    bool show_separator = true;
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    Tabs(Container children, std::vector<std::string> labels)
        : Widget{FocusPolicy::Strong}, pages{std::move(children)}
    {
        set_labels(std::move(labels));
        Connection{.signal = pages.on_select, .slot = [](auto index, Tabs& tabs) {
                       tabs.request_repaint();
                       tabs.on_select(index);
                   }}(*this);
    }
    auto labels() const -> std::span<std::string const> { return labels_; }
    void set_labels(std::vector<std::string> labels)
    {
        std::size_t count = 0;
        detail::for_each_child(pages.children, [&](Widget const&) { ++count; });
        if (labels.size() != count) {
            throw std::invalid_argument{"Tab labels/page count"};
        }
        for (auto const& text : labels) {
            validate_row_text(text);
        }
        labels_ = std::move(labels);
        refresh();
    }
    auto selected_index() const -> std::optional<std::size_t>
    { return pages.selected_index(); }
    auto select(std::optional<std::size_t> index) -> bool
    {
        auto snapshot = widget_tree(pages);
        auto had_focus = std::ranges::any_of(snapshot, [](auto const& n) {
            return n.widget.valid() && n.widget.get().is_focused();
        });
        auto lifetime = track(*this);
        auto changed = pages.select(index);
        if (auto* self = lifetime.try_get(); self && changed) self->update_indicator();
        if (auto* self = lifetime.try_get(); self && changed && had_focus) {
            auto tree = widget_tree(self->pages);
            auto candidates = focus_order(tree);
            if (!candidates.empty()) {
                self->request_focus(tree[candidates.front()].widget.get());
            }
        }
        return changed;
    }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield pages; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield pages; }
    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        validate(available);
        auto natural = pages.measure(
            {available.width, std::max(0, available.height - strip_height())}, policy);
        auto width = std::int64_t{0};
        for (auto const& text : labels_) {
            width += measure_text(text, available, TextWrap::None, policy).width + 2;
        }
        if (width > std::numeric_limits<int>::max() ||
            natural.height > std::numeric_limits<int>::max() - strip_height()) {
            throw std::length_error{"Tab extent"};
        }
        return {std::max(natural.width, (int)width), natural.height + strip_height()};
    }
    auto paint(Canvas canvas) -> Cursor override
    {
        auto strip = canvas.subcanvas({}, {size().width, std::min(1, size().height)});
        fill(strip, appearance(style));
        auto x = 0;
        auto widths = tab_widths(canvas.width_policy());
        if (auto selected = selected_index(); selected && *selected < widths.size()) {
            int start = 0;
            for (std::size_t i = 0; i < *selected; ++i) {
                start += widths[i];
            }
            scroll_ = scroll_to_show(scroll_, start, widths[*selected], total(widths),
                                     size().width);
        }
        for (std::size_t i = 0; i < labels_.size(); ++i) {
            auto normal = appearance(style);
            if (selected_index() == i) {
                normal = theme().styled ? selection_appearance(normal, selected_style)
                                        : normal | selected_style;
            }
            else
                normal = normal | unselected_style;
            if (!is_enabled()) normal = normal | disabled_style;
            paint_row_text(strip.subcanvas({x - scroll_, 0}, {widths[i], 1}),
                           " " + labels_[i] + " ", Align::Left, normal, layout_);
            x += widths[i];
        }
        if (theme().styled && show_separator && canvas.size().height > 1) {
            auto line = canvas.subcanvas({0, 1}, {canvas.size().width, 1});
            auto selected = appearance(style, Surface::Accent) | indicator_style;
            auto track = appearance(style, Surface::Border) | track_style;
            if (!is_enabled()) {
                selected = selected | disabled_style;
                track = track | disabled_style;
            }
            paint_rule(line, indicator_.value() - scroll_,
                       selected_index() ? indicator_width_.value() : 0, selected, track,
                       false, theme().ascii);
        }
        return {};
    }
    auto handle(MousePress const& e) -> EventResponse override
    {
        if (e.button != MouseButton::Left || (e.at.y < 0 || e.at.y >= strip_height()) ||
            e.at.x < 0 || e.at.x >= size().width) {
            return {};
        }
        auto x = e.at.x + scroll_;
        auto widths = tab_widths(glyph_width_policy());
        for (std::size_t i = 0; i < widths.size(); ++i) {
            if (x < widths[i]) { return {.handled = true, .repaint = select(i)}; }
            x -= widths[i];
        }
        return {};
    }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (e.action == KeyAction::Release || labels_.empty()) { return {}; }
        auto delta = 0;
        if (e.matches(KeyCode::Tab, {.ctrl = true}, e.action) ||
            e.matches(KeyCode::PageDown, {.ctrl = true}, e.action)) {
            delta = 1;
        }
        else if (e.matches(KeyCode::Tab, {.shift = true, .ctrl = true}, e.action) ||
                 e.matches(KeyCode::PageUp, {.ctrl = true}, e.action)) {
            delta = -1;
        }
        else if (is_focused() && e.matches(KeyCode::ArrowLeft, {}, e.action)) {
            delta = -1;
        }
        else if (is_focused() && e.matches(KeyCode::ArrowRight, {}, e.action)) {
            delta = 1;
        }
        else {
            return {};
        }
        auto index = (std::int64_t)selected_index().value_or(0) + delta;
        index = (index + (std::int64_t)labels_.size()) % (std::int64_t)labels_.size();
        return {.handled = true, .repaint = select((std::size_t)index)};
    }

   protected:
    void on_arrange(Rect) override
    {
        update_indicator(last_width_ != size().width);
        last_width_ = size().width;
        auto height = std::min(strip_height(), size().height);
        arrange(pages,
                {{0, height}, {size().width, std::max(0, size().height - height)}});
    }

   private:
    std::vector<std::string> labels_;
    TextLayout layout_;
    int scroll_ = 0;
    int last_width_ = -1;
    bool indicator_ready_ = false;
    Animation indicator_{*this};
    Animation indicator_width_{*this};
    auto strip_height() const -> int
    { return theme().styled && show_separator ? 2 : 1; }
    void update_indicator(bool snap = false)
    {
        if (!theme().styled || !show_separator) {
            indicator_.reset(0);
            indicator_width_.reset(0);
            indicator_ready_ = false;
            return;
        }
        auto widths = tab_widths(glyph_width_policy());
        auto index = selected_index();
        int start = 0, width = 0;
        if (index && *index < widths.size()) {
            for (std::size_t i = 0; i < *index; ++i)
                start += widths[i];
            width = widths[*index];
        }
        if (snap || !indicator_ready_) {
            indicator_.reset(start);
            indicator_width_.reset(width);
            indicator_ready_ = true;
        }
        else {
            if (indicator_.target() != start)
                indicator_.to(start, {theme().motion.selection, Ease::OutCubic});
            if (indicator_width_.target() != width)
                indicator_width_.to(width, {theme().motion.selection, Ease::OutCubic});
        }
    }
    auto tab_widths(esc::WidthPolicy policy) const -> std::vector<int>
    {
        auto widths = std::vector<int>{};
        auto sum = std::int64_t{0};
        for (auto const& text : labels_) {
            auto width = measure_text(text, size(), TextWrap::None, policy).width;
            sum += (std::int64_t)width + 2;
            if (sum > std::numeric_limits<int>::max()) {
                throw std::length_error{"Tab extent"};
            }
            widths.push_back(width + 2);
        }
        return widths;
    }
    static auto total(std::span<int const> widths) -> int
    {
        int n = 0;
        for (auto w : widths) {
            n += w;
        }
        return n;
    }
};
template <LayoutContainer Container>
Tabs(Container, std::vector<std::string>) -> Tabs<Container>;
}  // namespace ox
