/** @file
 * Widget lifecycle, geometry, effects, and tracked connections.
 */
#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <zzz/coro.hpp>

#include <signals_light/signal.hpp>

#include <ox/core/canvas.hpp>
#include <ox/core/events.hpp>
#include <ox/core/layout.hpp>
#include <ox/motion.hpp>
#include <ox/timer.hpp>

namespace ox {
namespace detail {
template <typename, bool>
class LinearLayout;
}
class Application;
struct Clipboard;
template <typename T>
class LifetimeView;
/** Borrow typed widget identity; the view follows moves and expires on destruction or
 * incompatible slicing.
 */
template <typename T>
auto track(T&) -> LifetimeView<T>;

/**
 * Policy for how a widget should handle focus.
 *
 * @details
 * None: The widget cannot be focused.
 * Tab: The widget can be focused with the tab key.
 * Click: The widget can be focused by clicking on it.
 * Strong: The widget can be focused by either tab or click.
 */
enum class FocusPolicy : std::uint8_t {
    None, ///< Exclude from keyboard and pointer focus.
    Tab, ///< Include in Tab traversal; pointer presses do not select focus.
    Click, ///< Allow pointer focus; exclude from Tab traversal.
    Strong ///< Allow both Tab traversal and pointer focus.
};

/**
 * Base class for interactive UI elements.
 */
class Widget {
   public:
    /** How this widget can acquire focus; changing it is observed during tree
     * synchronization.
     */
    FocusPolicy focus_policy;
    /** Horizontal allocation bounds and flexibility; call refresh() after direct
     * layout-policy changes.
     */
    SizePolicy width_policy;
    /** Vertical allocation bounds and flexibility; call refresh() after direct
     * layout-policy changes.
     */
    SizePolicy height_policy;
    /** False hides this widget and its descendants from painting and input. */
    bool visible = true;
    /** False excludes this subtree from input, focus, and widget-timer callbacks; it
     * remains visible.
     */
    bool enabled = true;
    /** Root-layer child: bounds are global and inherited parent clipping is bypassed.
     */
    bool overlay = false;
    /** While visible, input/focus are confined to this subtree. */
    bool modal = false;
    /** Overlay stacking key; bring_to_front() raises an attached overlay without manual
     * bookkeeping.
     */
    std::size_t overlay_order = 0;
    /** Optional application-wide bindings. Current input/modal eligibility applies. */
    std::function<EventResponse(KeyEvent const&)> shortcuts = {};
    /** Return effective input eligibility when attached, including disabled ancestors
     * and modal restrictions.
     */
    auto is_enabled() const -> bool
    { return enabled && (!application_ || input_enabled_); }
    /** Patch applied by appearance() when this widget is effectively disabled. */
    StylePatch disabled_style = {.background = {},
                                 .foreground = {},
                                 .enabled = Trait::Dim,
                                 .disabled = {}};
    /** Borrow the application theme, or a static plain theme while detached. */
    auto theme() const -> Theme const&;
    /** Resolve semantic theme defaults, preserve explicit colors, then apply
     * disabled_style if needed.
     */
    auto appearance(Style normal, Surface role = Surface::Canvas) const -> Style
    {
        normal = theme_style(normal, theme(), role);
        return is_enabled() ? normal : normal | disabled_style;
    }
    /** Resolve a themed selected style with the supplied explicit patch. */
    auto selection_appearance(Style normal, StylePatch const& patch) const -> Style
    { return theme_selection(normal, patch, theme()); }

    /** Return parent-local geometry; overlay bounds are interpreted in screen coordinates. */
    [[nodiscard]] auto bounds() const -> Rect { return bounds_; }
    /** Return bounds().at in the parent's coordinate system. */
    [[nodiscard]] auto at() const -> Point { return bounds_.at; }
    /** Return the assigned terminal-cell extent, which may be zero in either dimension. */
    [[nodiscard]] auto size() const -> Area { return bounds_.size; }
    /** Return the focus flag maintained by Focus; do not infer focus from its policy. */
    [[nodiscard]] auto is_focused() const -> bool { return focused_; }

   public:
    /** Create an unattached, visible, enabled widget with the given focus and allocation
     * policies.
     */
    Widget(FocusPolicy fp = FocusPolicy::None,
           SizePolicy width = SizePolicy::flex(),
           SizePolicy height = SizePolicy::flex());

    /** Widget identities cannot be copied. */
    Widget(Widget const&) = delete;
    /** Transfer identity, attachments, timers, and animations so existing tracked
     * references follow the move.
     */
    Widget(Widget&& other);

    /** Widget identities cannot be copy-assigned. */
    auto operator=(Widget const&) -> Widget& = delete;
    /** Invalidate this identity and its work, then take other's identity and attached work. */
    auto operator=(Widget&& other) -> Widget&;

    /** Expire lifetime views, stop attached timers, and detach animations without
     * destroying child widgets.
     */
    virtual ~Widget();

   public:
    /** Handle a local-coordinate mouse press; the default leaves propagation and effects
     * unchanged.
     */
    virtual auto handle(MousePress const&) -> EventResponse { return {}; }

    /** Handle a local-coordinate mouse release; capture can deliver it outside the
     * widget bounds.
     */
    virtual auto handle(MouseRelease const&) -> EventResponse { return {}; }

    /** Handle local wheel input; return handled to stop ancestor propagation. */
    virtual auto handle(MouseWheel const&) -> EventResponse { return {}; }

    /** Handle local pointer motion, including captured drags outside the widget bounds. */
    virtual auto handle(MouseMove const&) -> EventResponse { return {}; }

    /** Notify entry into the hover ancestry; default has no effects. */
    virtual auto mouse_enter() -> EventResponse { return {}; }

    /** Notify departure from the hover ancestry, including geometry or eligibility changes. */
    virtual auto mouse_leave() -> EventResponse { return {}; }

    /** Notify acquisition of focus; return repaint if focus decoration changes. */
    virtual auto focus_in() -> EventResponse { return {}; }

    /** Notify loss of focus; default has no effects. */
    virtual auto focus_out() -> EventResponse { return {}; }

    /** Handle keyboard input from the focused ancestry; raw handlers return effects
     * without applying them.
     */
    virtual auto handle(KeyEvent const&) -> EventResponse { return {}; }

    /** Handle a bounded paste-stream chunk; editors commit only complete accepted paste
     * content.
     */
    virtual auto handle(esc::PasteChunk const&) -> EventResponse { return {}; }

    /** Pure natural-size query. Measurement must not mutate the tree or emit signals.
     */
    virtual auto measure(Area available, esc::WidthPolicy = {}) const -> Area
    {
        validate(available);
        return {};
    }
    /** Paint behind this widget and its children, clipped to the inherited visible region. */
    virtual void paint_background(Canvas) {}
    /** Paint local content and return an optional local caret; the application uses it
     * only for the focused widget.
     */
    virtual auto paint(Canvas) -> Cursor { return {}; }
    /** Paint after children using the same inherited clip, for borders or other
     * foreground decorations.
     */
    virtual void paint_overlay(Canvas) {}
    /** Request layout and repaint after direct public configuration changes; subclasses
     * may rebuild cached data.
     */
    virtual void refresh()
    {
        request_layout();
        request_repaint();
    }
    /** Called after attachment and application theme changes. Do not emit model
     * changes. */
    virtual void theme_changed() {}
    /** Cancel an armed pointer gesture when capture is lost; no release activation
     * should follow.
     */
    virtual auto pointer_cancel() -> EventResponse { return {}; }
    /** Request explicit drag capture while attached and eligible; return false if
     * capture is unavailable.
     */
    auto capture_pointer(MouseButton button = MouseButton::Left) -> bool;
    /** Release this widget's capture, if any; detached widgets do nothing. */
    void release_pointer();
    /** Return whether this widget currently owns application pointer capture. */
    auto is_pointer_captured() const -> bool;
    /** Borrow the application clipboard. @throws std::logic_error The widget is not
     * attached.
     */
    auto clipboard() -> Clipboard&;
    /** Return the attached frame's width policy, or the default policy while detached. */
    auto glyph_width_policy() const -> esc::WidthPolicy;
    /** Return the attached screen size, or this widget's assigned size while detached. */
    auto screen_size() const -> Area;
    /** Return attached tree geometry in screen coordinates, or bounds() while detached. */
    auto global_bounds() const -> Rect;
    /** Request focus for this attached, eligible widget; return false when detached or
     * ineligible.
     */
    auto request_focus() -> bool;
    /** Request focus for an eligible target in the same application; return whether the
     * request was accepted for delivery.
     */
    auto request_focus(Widget& target) -> bool;
    /** Raise this widget in its attached application's overlay order; detached widgets
     * do nothing.
     */
    void bring_to_front();

    /** Enumerate borrowed immediate children in paint order; the default yields none. */
    virtual auto get_children() -> zzz::Generator<Widget&>
    { return zzz::Generator<Widget&>{nullptr}; }

    /** Enumerate the same immediate children read-only; children must survive traversal. */
    virtual auto get_children() const -> zzz::Generator<Widget const&>
    { return zzz::Generator<Widget const&>{nullptr}; }

   protected:
    /** Return a callable that weakly applies effects to the attached loop, or an empty
     * function while detached.
     */
    auto effect_handler() const -> std::function<void(EventResponse)>;
    /** Geometry is already updated; use arrange() on children. */
    virtual void on_arrange(Rect /* previous */) {}

    /** Schedule painting after a direct widget mutation, if attached. */
    void request_repaint();
    /** Schedule layout and repaint through the attached application; detached widgets do
     * nothing.
     */
    void request_layout();

   private:
    template <typename, bool>
    friend class detail::LinearLayout;
    friend class Timer;
    friend class Animation;
    friend class Application;
    friend class Focus;
    friend void arrange(Widget&, Rect);
    template <typename T>
    friend auto track(T&) -> LifetimeView<T>;
    Rect bounds_;
    bool focused_ = false;
    bool input_enabled_ = true;
    mutable std::shared_ptr<Widget*> lifetime_ = std::make_shared<Widget*>(this);
    Application* application_ = nullptr;
    std::vector<std::weak_ptr<Timer::State>> timers_;
    std::vector<std::weak_ptr<Animation::State>> animations_;
};

/**
 * Handle to a Widget Lifetime, can check if the Widget is still alive and get a ref.
 *
 * @details Use the track() function to create objects of this type.
 */
template <typename ConcreteType>
class LifetimeView {
   public:
    /** Create an empty, invalid view without extending any widget lifetime. */
    LifetimeView() = default;

   public:
    /** Return the current typed widget address, or null after destruction or
     * incompatible slicing.
     */
    [[nodiscard]] auto try_get() const noexcept -> ConcreteType*
    {
        auto pointer = widget_ptr_.lock();
        if (!pointer || !*pointer) { return nullptr; }
        if constexpr (std::is_same_v<ConcreteType, Widget>) { return *pointer; }
        else {
            return dynamic_cast<ConcreteType*>(*pointer);
        }
    }
    /** Return whether try_get() currently resolves a compatible live widget. */
    [[nodiscard]] auto valid() const noexcept -> bool { return try_get() != nullptr; }
    /** Compare tracked identity rather than the widget's current address or dynamic type. */
    template <typename Other>
    auto operator==(LifetimeView<Other> const& other) const -> bool
    {
        return !widget_ptr_.owner_before(other.widget_ptr_) &&
               !other.widget_ptr_.owner_before(widget_ptr_);
    }

    /** Borrow the current widget. @throws std::logic_error The identity has expired or
     * no longer has the requested type.
     */
    [[nodiscard]] auto get() const -> ConcreteType&
    {
        auto* pointer = try_get();
        if (!pointer) { throw std::logic_error{"Expired widget lifetime"}; }
        return *pointer;
    }

   private:
    template <typename>
    friend class LifetimeView;
    template <typename T>
    friend auto track(T&) -> LifetimeView<T>;
    explicit LifetimeView(std::shared_ptr<Widget*> const& pointer)
        : widget_ptr_{pointer}
    {}
    std::weak_ptr<Widget*> widget_ptr_;
};

/** A widget type, allowing const qualification while retaining inheritance from Widget. */
template <typename T>
concept WidgetDerived = std::derived_from<std::remove_cv_t<T>, Widget>;

/** Assign both width and height policies and return the same widget; use individual
 * fields for one axis.
 */
template <WidgetDerived WidgetType>
auto operator|(WidgetType& w, SizePolicy sp) -> WidgetType&
{
    w.width_policy = sp;
    w.height_policy = sp;
    return w;
}

/** Assign both allocation policies and return the moved widget for composition. */
template <WidgetDerived WidgetType>
auto operator|(WidgetType&& w, SizePolicy sp) -> WidgetType
{
    w.width_policy = sp;
    w.height_policy = sp;
    return w;
}

/** Assign focus eligibility and return the same widget for composition. */
template <WidgetDerived WidgetType>
auto operator|(WidgetType& w, FocusPolicy fp) -> WidgetType&
{
    w.focus_policy = fp;
    return w;
}

/** Assign focus eligibility and return the moved widget for composition. */
template <WidgetDerived WidgetType>
auto operator|(WidgetType&& w, FocusPolicy fp) -> WidgetType
{
    w.focus_policy = fp;
    return w;
}

namespace filter {

/** Range adaptor filtering widgets by their own visible flag; ancestor visibility is not
 * evaluated.
 */
inline constexpr auto is_visible =
    std::views::filter([](Widget const& w) { return w.visible; });

}  // namespace filter

/**
 * Create a handle to the lifetime of the given Widget.
 *
 * Views follow moves, reject slicing, and borrow the widget without extending its life.
 */
template <typename WidgetType>
[[nodiscard]] auto track(WidgetType& w) -> LifetimeView<WidgetType>
{
    static_assert(WidgetDerived<WidgetType>);
    if (!w.lifetime_) {
        w.lifetime_ = std::make_shared<Widget*>(
            const_cast<Widget*>(static_cast<Widget const*>(&w)));
    }
    return LifetimeView<WidgetType>{w.lifetime_};
}

/** Assign parent-local bounds and notify layout, even when bounds are unchanged.
 * Validation precedes mutation; a throwing layout hook leaves the new bounds. */
void arrange(Widget& widget, Rect bounds);

template <typename W, typename Callback>
    requires std::derived_from<W, Widget> &&
             std::is_invocable_r_v<EventResponse, Callback&, W&>
Timer::Timer(W& widget,
             std::chrono::milliseconds interval,
             Callback action,
             bool launch)
    : Timer{interval,
            widget_action(widget,
                          [action = checked_callback(std::move(action))](
                              Widget& base) mutable -> EventResponse {
                              auto* target = dynamic_cast<W*>(&base);
                              return target ? std::invoke(action, *target)
                                            : EventResponse{};
                          }),
            launch}
{ attach(widget); }

/** Borrow widget identities and append live references to the callback arguments.
 * Expired/sliced targets suppress invocation. Callbacks run on the dispatch thread.
 * Captures owned by the callback remain alive until the callback is removed. */
template <typename SlotFn, WidgetDerived... TrackedWidgets>
[[nodiscard]] auto tracked(SlotFn&& fn, TrackedWidgets&... widgets)
{
    if constexpr (requires { static_cast<bool>(fn); }) {
        if (!static_cast<bool>(fn)) {
            throw std::invalid_argument{"Tracked callback is empty"};
        }
    }
    return [fn = std::forward<SlotFn>(fn),
            ... lifetimes = track(widgets)](auto&&... args) mutable
        requires std::invocable<SlotFn&, decltype(args)..., TrackedWidgets&...>
    {
        auto pointers = std::tuple{lifetimes.try_get()...};
        std::apply(
            [&](auto*... pointer) {
                if (((pointer != nullptr) && ...)) {
                    std::invoke(fn, std::forward<decltype(args)>(args)..., *pointer...);
                }
            },
            pointers);
    };
}

/** Named signal/slot configuration; invoke immediately to make the connection.
 * The signal owns its slot. Discarding the identifier keeps it connected;
 * signal.disconnect(id) removes it. Tracked targets are borrowed, not owned.
 *
 * Callbacks must be copyable. A disconnected slot returned by signal.disconnect()
 * can still be called and retains its captures while held. SignalsLight snapshots
 * each emission: new slots wait until the next emission, disconnected pending slots
 * are skipped, and destroying the signal cancels pending callbacks. Mutable callbacks
 * retain state between emissions; copies own independent callback state.
 *
 * The label argument below is resolved at invocation, so it follows moves and
 * suppresses the callback after the label is destroyed:
 * @code{.cpp}
 * auto count = ox::IntegerLabel{{.value = 0}};
 * auto button = ox::Button{{.label = {.text = "Increment"}}};
 * ox::Connection{
 *     .signal = button.on_activate,
 *     .slot = [](ox::IntegerLabel& count) { count.set_value(count.value + 1); },
 * }(count);
 * @endcode
 */
template <typename SignalType, typename SlotType>
struct [[nodiscard("Use operator() to make the connection.")]] Connection {
    /** Borrowed signal that owns the subscription after operator() is invoked. */
    SignalType& signal;
    /** Callback moved into the signal; tracked widgets are appended to its emitted
     * arguments.
     */
    SlotType slot;

    /** Subscribe once and return an explicit-disconnection identifier; discarding it
     * keeps the slot connected.
     */
    template <WidgetDerived... TrackedWidgets>
        requires std::copy_constructible<SlotType>
    auto operator()(TrackedWidgets&... widgets) && -> sl::Identifier
    {
        if constexpr (sizeof...(widgets) == 0) {
            return signal.connect(std::move(slot));
        }
        else {
            return signal.connect(tracked(std::move(slot), widgets...));
        }
    }
};

}  // namespace ox
