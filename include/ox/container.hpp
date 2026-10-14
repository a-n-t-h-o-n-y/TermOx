#pragma once

#include <limits>
#include <optional>
#include <tuple>

#include <ox/layout.hpp>
#include <ox/scroll.hpp>
#include <ox/scrollbar.hpp>

namespace ox {

/** Own one child and paint its background before the child. Insets are T/R/B/L. */
template <WidgetDerived Child>
class Padding : public Widget {
   public:
    using Widget::handle;
    Child child;
    std::array<int, 4> padding;
    Style style;

    explicit Padding(Child child, std::array<int, 4> padding = {}, Style style = {})
        : child{std::move(child)}, padding{padding}, style{style}
    { (void)padding_rect({}, padding); }
    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        auto const interior = padding_rect(available, padding);
        auto const natural = child.measure(interior.size, policy);
        validate(natural);
        auto const width = (std::int64_t)natural.width + padding[1] + padding[3];
        auto const height = (std::int64_t)natural.height + padding[0] + padding[2];
        if (width > std::numeric_limits<int>::max() ||
            height > std::numeric_limits<int>::max()) {
            throw std::length_error{"Padding extent overflow"};
        }
        return {(int)width, (int)height};
    }
    void refresh() override
    {
        request_layout();
        layout_child();
    }

    void paint_background(Canvas canvas) override { fill(canvas, U' ' | style); }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }

   protected:
    void on_arrange(Rect) override { layout_child(); }

   private:
    void layout_child()
    {
        auto const bounds = padding_rect(size(), padding);
        request_repaint();
        arrange(child, bounds);
    }
};

/** Enumerate only the selected page; other pages keep their own visibility state. */
template <LayoutContainer Container>
class Stack : public Widget {
   public:
    using Widget::handle;
    Container children;
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    explicit Stack()
        requires std::default_initializable<Container>
        : Stack{Container{}}
    {}
    explicit Stack(Container children) : children{std::move(children)}
    {
        if (count()) { selected_ = 0; }
    }
    template <WidgetDerived... Children>
        requires(sizeof...(Children) > 0)
    explicit Stack(Children&&... children)
        : Stack{Container{std::forward<Children>(children)...}}
    {}
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    auto select(std::optional<std::size_t> index) -> bool
    {
        if (index && *index >= count()) { throw std::out_of_range{"Stack page"}; }
        if (selected_ == index) { return false; }
        selected_ = index;
        request_repaint();
        auto lifetime = track(*this);
        refresh();
        if (auto* stack = lifetime.try_get(); stack && stack->selected_ == index) {
            stack->on_select(index);
        }
        return true;
    }
    void refresh() override
    {
        request_layout();
        layout_page();
    }

    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        validate(available);
        auto const* page = selected_child();
        return page && page->visible ? page->measure(available, policy) : Area{};
    }
    auto get_children() -> zzz::Generator<Widget&> override
    {
        if (auto* page = selected_child()) { co_yield *page; }
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        if (auto const* page = selected_child()) { co_yield *page; }
    }

   protected:
    void on_arrange(Rect) override { layout_page(); }

   private:
    void layout_page()
    {
        if (selected_ && *selected_ >= count()) {
            selected_.reset();
            request_repaint();
            on_select(std::nullopt);
            return;
        }
        auto* page = selected_child();
        request_repaint();
        if (page && page->visible) { arrange(*page, {{}, size()}); }
    }
    std::optional<std::size_t> selected_;
    auto count() const -> std::size_t
    {
        auto result = std::size_t{0};
        detail::for_each_child(children, [&](Widget const&) { ++result; });
        return result;
    }
    auto selected_child() -> Widget*
    {
        Widget* result = nullptr;
        auto index = std::size_t{0};
        detail::for_each_child(children, [&](Widget& child) {
            if (selected_ == index++) { result = &child; }
        });
        return result;
    }
    auto selected_child() const -> Widget const*
    {
        Widget const* result = nullptr;
        auto index = std::size_t{0};
        detail::for_each_child(children, [&](Widget const& child) {
            if (selected_ == index++) { result = &child; }
        });
        return result;
    }
};
template <LayoutContainer Container>
Stack(Container) -> Stack<Container>;
template <WidgetDerived... Children>
Stack(Children&&...) -> Stack<std::tuple<std::decay_t<Children>...>>;

/** Own one child with a clipped two-axis viewport. Natural size is measured unless
 * requested_size is supplied. Refresh after changing child content/configuration. */
template <WidgetDerived Child>
class ScrollView : public Widget {
   public:
    using Widget::handle;
    Child child;
    std::optional<Area> requested_size;
    Style style = {};
    sl::Signal<void(Point, Area, Area)> on_scroll;
    explicit ScrollView(Child child, std::optional<Area> requested = {})
        : Widget{FocusPolicy::Tab}, child{std::move(child)}, requested_size{requested}
    {
        if (requested) { validate(*requested); }
    }
    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        validate(available);
        auto natural =
            (requested_size ? *requested_size : child.measure(available, policy));
        validate(natural);
        return {std::min(natural.width, available.width),
                std::min(natural.height, available.height)};
    }
    auto content_size() const -> Area { return content_; }
    auto viewport_size() const -> Area { return size(); }
    auto scroll_offset() const -> Point { return offset_; }
    void refresh() override
    {
        request_layout();
        layout_child();
    }

    auto set_scroll_offset(Point offset) -> bool
    {
        auto next = Point{clamp_scroll(offset.x, content_.width, size().width),
                          clamp_scroll(offset.y, content_.height, size().height)};
        if (next == offset_) { return false; }
        offset_ = next;
        layout_child();
        return true;
    }
    auto reveal(Rect content) -> bool
    {
        validate(content.size);
        return set_scroll_offset(
            {scroll_to_show(offset_.x, content.at.x, content.size.width, content_.width,
                            size().width),
             scroll_to_show(offset_.y, content.at.y, content.size.height,
                            content_.height, size().height)});
    }
    auto handle(MouseWheel const& event) -> EventResponse override
    {
        auto const changed = set_scroll_offset(
            {scroll_by(offset_.x, event.dx, content_.width, size().width),
             scroll_by(offset_.y, event.dy, content_.height, size().height)});
        return {.handled = changed, .repaint = changed};
    }
    auto handle(KeyEvent const& event) -> EventResponse override
    {
        if (event.action == KeyAction::Release) { return {}; }
        auto next = offset_;
        auto matches = [&](KeyCode key) {
            return event.matches(key, {}, event.action);
        };
        if (matches(KeyCode::ArrowLeft)) {
            next.x = scroll_by(next.x, -1, content_.width, size().width);
        }
        else if (matches(KeyCode::ArrowRight)) {
            next.x = scroll_by(next.x, 1, content_.width, size().width);
        }
        else if (matches(KeyCode::ArrowUp)) {
            next.y = scroll_by(next.y, -1, content_.height, size().height);
        }
        else if (matches(KeyCode::ArrowDown)) {
            next.y = scroll_by(next.y, 1, content_.height, size().height);
        }
        else if (matches(KeyCode::PageUp)) {
            next.y = scroll_by(next.y, -std::max(size().height, 1), content_.height,
                               size().height);
        }
        else if (matches(KeyCode::PageDown)) {
            next.y = scroll_by(next.y, std::max(size().height, 1), content_.height,
                               size().height);
        }
        else if (matches(KeyCode::Home)) {
            next = {};
        }
        else if (matches(KeyCode::End)) {
            next.y = scroll_limit(content_.height, size().height);
        }
        else {
            return {};
        }
        auto const changed = set_scroll_offset(next);
        return {.handled = true, .repaint = changed};
    }
    void paint_background(Canvas canvas) override { fill(canvas, U' ' | style); }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }

   protected:
    void on_arrange(Rect) override { layout_child(); }

   private:
    void layout_child()
    {
        auto natural = requested_size ? *requested_size
                                      : child.measure(size(), glyph_width_policy());
        validate(natural);
        content_ = requested_size ? natural
                                  : Area{std::max(natural.width, size().width),
                                         std::max(natural.height, size().height)};
        offset_ = {clamp_scroll(offset_.x, content_.width, size().width),
                   clamp_scroll(offset_.y, content_.height, size().height)};
        auto lifetime = track(*this);
        request_repaint();
        arrange(child, {{-offset_.x, -offset_.y}, content_});
        if (auto* view = lifetime.try_get()) { view->notify(); }
    }
    Point offset_ = {};
    Area content_ = {};
    std::optional<std::tuple<Point, Area, Area>> notified_;
    void notify()
    {
        auto value = std::tuple{offset_, content_, size()};
        if (notified_ == value) { return; }
        notified_ = value;
        on_scroll(std::get<0>(value), std::get<1>(value), std::get<2>(value));
    }
};

template <WidgetDerived Child>
auto link(ScrollView<Child>& view, ScrollBar& bar)
    -> std::pair<sl::Identifier, sl::Identifier>
{
    auto synchronize = [](auto& view, ScrollBar& bar) {
        auto const content = view.content_size();
        auto const viewport = view.viewport_size();
        auto const at = view.scroll_offset();
        bar.set_range(bar.vertical ? content.height : content.width,
                      bar.vertical ? viewport.height : viewport.width,
                      bar.vertical ? at.y : at.x);
    };
    synchronize(view, bar);
    auto first = Connection{
        .signal = view.on_scroll,
        .slot = [synchronize](Point, Area, Area, auto& view, ScrollBar& bar) {
            synchronize(view, bar);
        }}(view, bar);
    try {
        return {first, Connection{.signal = bar.on_scroll,
                                  .slot = [](int, auto& view, ScrollBar& bar) {
                                      auto at = view.scroll_offset();
                                      if (bar.vertical) { at.y = bar.position(); }
                                      else {
                                          at.x = bar.position();
                                      }
                                      view.set_scroll_offset(at);
                                  }}(view, bar)};
    }
    catch (...) {
        view.on_scroll.disconnect(first);
        throw;
    }
}
}  // namespace ox
