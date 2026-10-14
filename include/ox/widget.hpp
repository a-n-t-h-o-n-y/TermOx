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
enum class FocusPolicy : std::uint8_t { None, Tab, Click, Strong };

/**
 * Base class for interactive UI elements.
 */
class Widget {
   public:
    FocusPolicy focus_policy;
    SizePolicy width_policy;
    SizePolicy height_policy;
    bool visible = true;
    bool enabled = true;
    /** Root-layer child: bounds are global and inherited parent clipping is bypassed.
     */
    bool overlay = false;
    /** While visible, input/focus are confined to this subtree. */
    bool modal = false;
    std::size_t overlay_order = 0;
    /** Optional application-wide bindings. Current input/modal eligibility applies. */
    std::function<EventResponse(KeyEvent const&)> shortcuts = {};
    auto is_enabled() const -> bool
    { return enabled && (!application_ || input_enabled_); }
    StylePatch disabled_style = {.background = {},
                                 .foreground = {},
                                 .enabled = Trait::Dim,
                                 .disabled = {}};
    auto theme() const -> Theme const&;
    auto appearance(Style normal, Surface role = Surface::Canvas) const -> Style
    {
        normal = theme_style(normal, theme(), role);
        return is_enabled() ? normal : normal | disabled_style;
    }
    auto selection_appearance(Style normal, StylePatch const& patch) const -> Style
    { return theme_selection(normal, patch, theme()); }

    [[nodiscard]] auto bounds() const -> Rect { return bounds_; }
    [[nodiscard]] auto at() const -> Point { return bounds_.at; }
    [[nodiscard]] auto size() const -> Area { return bounds_.size; }
    [[nodiscard]] auto is_focused() const -> bool { return focused_; }

   public:
    Widget(FocusPolicy fp = FocusPolicy::None,
           SizePolicy width = SizePolicy::flex(),
           SizePolicy height = SizePolicy::flex());

    Widget(Widget const&) = delete;
    Widget(Widget&& other);

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&;

    virtual ~Widget();

   public:
    virtual auto handle(MousePress const&) -> EventResponse { return {}; }

    virtual auto handle(MouseRelease const&) -> EventResponse { return {}; }

    virtual auto handle(MouseWheel const&) -> EventResponse { return {}; }

    virtual auto handle(MouseMove const&) -> EventResponse { return {}; }

    virtual auto mouse_enter() -> EventResponse { return {}; }

    virtual auto mouse_leave() -> EventResponse { return {}; }

    virtual auto focus_in() -> EventResponse { return {}; }

    virtual auto focus_out() -> EventResponse { return {}; }

    virtual auto handle(KeyEvent const&) -> EventResponse { return {}; }

    virtual auto handle(esc::PasteChunk const&) -> EventResponse { return {}; }

    /** Pure natural-size query. Measurement must not mutate the tree or emit signals.
     */
    virtual auto measure(Area available, esc::WidthPolicy = {}) const -> Area
    {
        validate(available);
        return {};
    }
    virtual void paint_background(Canvas) {}
    virtual auto paint(Canvas) -> Cursor { return {}; }
    virtual void paint_overlay(Canvas) {}
    virtual void refresh()
    {
        request_layout();
        request_repaint();
    }
    /** Called after attachment and application theme changes. Do not emit model
     * changes. */
    virtual void theme_changed() {}
    virtual auto pointer_cancel() -> EventResponse { return {}; }
    auto capture_pointer(MouseButton button = MouseButton::Left) -> bool;
    void release_pointer();
    auto is_pointer_captured() const -> bool;
    auto clipboard() -> Clipboard&;
    auto glyph_width_policy() const -> esc::WidthPolicy;
    auto screen_size() const -> Area;
    auto global_bounds() const -> Rect;
    auto request_focus() -> bool;
    auto request_focus(Widget& target) -> bool;
    void bring_to_front();

    virtual auto get_children() -> zzz::Generator<Widget&>
    { return zzz::Generator<Widget&>{nullptr}; }

    virtual auto get_children() const -> zzz::Generator<Widget const&>
    { return zzz::Generator<Widget const&>{nullptr}; }

   protected:
    /** Geometry is already updated; use arrange() on children. */
    virtual void on_arrange(Rect /* previous */) {}

    /** Schedule painting after a direct widget mutation, if attached. */
    void request_repaint();
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
    LifetimeView() = default;

   public:
    [[nodiscard]] auto try_get() const noexcept -> ConcreteType*
    {
        auto pointer = widget_ptr_.lock();
        if (!pointer || !*pointer) { return nullptr; }
        if constexpr (std::is_same_v<ConcreteType, Widget>) { return *pointer; }
        else {
            return dynamic_cast<ConcreteType*>(*pointer);
        }
    }
    [[nodiscard]] auto valid() const noexcept -> bool { return try_get() != nullptr; }
    template <typename Other>
    auto operator==(LifetimeView<Other> const& other) const -> bool
    {
        return !widget_ptr_.owner_before(other.widget_ptr_) &&
               !other.widget_ptr_.owner_before(widget_ptr_);
    }

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

template <typename T>
concept WidgetDerived = std::derived_from<std::remove_cv_t<T>, Widget>;

template <WidgetDerived WidgetType>
auto operator|(WidgetType& w, SizePolicy sp) -> WidgetType&
{
    w.width_policy = sp;
    w.height_policy = sp;
    return w;
}

template <WidgetDerived WidgetType>
auto operator|(WidgetType&& w, SizePolicy sp) -> WidgetType
{
    w.width_policy = sp;
    w.height_policy = sp;
    return w;
}

template <WidgetDerived WidgetType>
auto operator|(WidgetType& w, FocusPolicy fp) -> WidgetType&
{
    w.focus_policy = fp;
    return w;
}

template <WidgetDerived WidgetType>
auto operator|(WidgetType&& w, FocusPolicy fp) -> WidgetType
{
    w.focus_policy = fp;
    return w;
}

namespace filter {

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
 * signal.disconnect(id) removes it. Tracked targets are borrowed, not owned. */
template <typename SignalType, typename SlotType>
struct [[nodiscard("Use operator() to make the connection.")]] Connection {
    SignalType& signal;
    SlotType slot;

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
