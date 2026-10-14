#pragma once
#include <ox/container.hpp>
#include <ox/routing.hpp>
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
        auto natural =
            pages.measure({available.width, std::max(0, available.height - 1)}, policy);
        auto width = std::int64_t{0};
        for (auto const& text : labels_) {
            width += measure_text(text, available, TextWrap::None, policy).width + 2;
        }
        if (width > std::numeric_limits<int>::max() ||
            natural.height == std::numeric_limits<int>::max()) {
            throw std::length_error{"Tab extent"};
        }
        return {std::max(natural.width, (int)width), natural.height + 1};
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
            if (selected_index() == i) { normal = normal | selected_style; }
            paint_row_text(strip.subcanvas({x - scroll_, 0}, {widths[i], 1}),
                           " " + labels_[i] + " ", Align::Left, normal, layout_);
            x += widths[i];
        }
        return {};
    }
    auto handle(MousePress const& e) -> EventResponse override
    {
        if (e.button != MouseButton::Left || e.at.y != 0 || e.at.x < 0 ||
            e.at.x >= size().width) {
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
        arrange(pages, {{0, std::min(1, size().height)},
                        {size().width, std::max(0, size().height - 1)}});
    }

   private:
    std::vector<std::string> labels_;
    TextLayout layout_;
    int scroll_ = 0;
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
