/** @file
 * Padding, page stacks, and clipped scrolling around retained children.
 */
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
    /** Owned child value, arranged inside the wrapper viewport. */
    Child child;
    /** Top, right, bottom, and left cell insets; negative entries are rejected. */
    std::array<int, 4> padding;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;

    /** Retain one child and validated top/right/bottom/left insets; negative padding throws. */
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

    void paint_background(Canvas canvas) override
    { fill(canvas, U' ' | appearance(style)); }
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
    /** Page container retained by the stack; only the selected page is exposed to the
     * application tree.
     */
    Container children;
    /** Emits the optional selected page after page layout; invalidated selection is
     * reported as nullopt.
     */
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    /** Create default child storage and select its first page when nonempty. */
    explicit Stack()
        requires std::default_initializable<Container>
        : Stack{Container{}}
    {}
    /** Retain page storage and initially select index zero if it has pages. */
    explicit Stack(Container children) : children{std::move(children)}
    {
        if (count()) { selected_ = 0; }
    }
    /** Construct retained page storage from forwarded widget values/references. */
    template <WidgetDerived... Children>
        requires(sizeof...(Children) > 0)
    explicit Stack(Children&&... children)
        : Stack{Container{std::forward<Children>(children)...}}
    {}
    /** Return the selected page index, or nullopt when no page participates in the
     * widget tree.
     */
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    /** Select a page or no page, refresh child geometry, and emit on_select only on change.
     * Other pages retain their own visible flags and state.
     * @throws std::out_of_range The requested index is outside the current child storage.
     */
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
/** Deduce stack storage from an existing tuple or range. */
template <LayoutContainer Container>
Stack(Container) -> Stack<Container>;
/** Deduce tuple page storage from individual widget arguments. */
template <WidgetDerived... Children>
Stack(Children&&...) -> Stack<std::tuple<std::decay_t<Children>...>>;

/** Own one child with a clipped two-axis viewport. Natural size is measured unless
 * requested_size is supplied. Refresh after changing child content/configuration. */
template <WidgetDerived Child>
class ScrollView : public Widget {
   public:
    using Widget::handle;
    /** Owned child value, arranged inside the wrapper viewport. */
    Child child;
    /** Optional requested extent; nonnegative dimensions are required. Call refresh()
     * after changing it.
     */
    std::optional<Area> requested_size;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style = {};
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    /** Retain a child and optional nonnegative requested content size; default focus
     * policy permits Tab navigation.
     */
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
    /** Return the retained child's scrollable extent in terminal cells. */
    auto content_size() const -> Area { return content_; }
    /** Return the assigned visible scroll-view extent. */
    auto viewport_size() const -> Area { return size(); }
    /** Return the clamped offset into child content in terminal columns and rows. */
    auto scroll_offset() const -> Point { return offset_; }
    void refresh() override
    {
        request_layout();
        layout_child();
    }

    /** Clamp offset, arrange the child at the translated negative position, repaint and
     * notify on change.
     */
    auto set_scroll_offset(Point offset) -> bool
    {
        auto next = Point{clamp_scroll(offset.x, content_.width, size().width),
                          clamp_scroll(offset.y, content_.height, size().height)};
        if (next == offset_) { return false; }
        offset_ = next;
        layout_child();
        return true;
    }
    /** Scroll enough to reveal a nonnegative content-space rectangle on both axes;
     * oversized rectangles align their beginning.
     */
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
    void paint_background(Canvas canvas) override
    { fill(canvas, U' ' | appearance(style)); }
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

}  // namespace ox
