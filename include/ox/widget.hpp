#pragma once

#include <memory>
#include <ranges>
#include <stdexcept>
#include <type_traits>

#include <zzz/coro.hpp>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/core/layout.hpp>

namespace ox {
class Application;
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
    SizePolicy size_policy;
    bool visible = true;
    bool enabled = true;

    [[nodiscard]] auto bounds() const -> Rect { return bounds_; }
    [[nodiscard]] auto at() const -> Point { return bounds_.at; }
    [[nodiscard]] auto size() const -> Area { return bounds_.size; }
    [[nodiscard]] auto is_focused() const -> bool { return focused_; }

   public:
    Widget(FocusPolicy fp = FocusPolicy::None, SizePolicy sp = SizePolicy::flex());

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

    virtual auto paint(Canvas) -> Cursor { return {}; }

    virtual auto get_children() -> zzz::Generator<Widget&>
    { return zzz::Generator<Widget&>{nullptr}; }

    virtual auto get_children() const -> zzz::Generator<Widget const&>
    { return zzz::Generator<Widget const&>{nullptr}; }

   protected:
    /** Geometry is already updated; use arrange() on children. */
    virtual void on_arrange(Rect /* previous */) {}

    /** Schedule painting after a direct widget mutation, if attached. */
    void request_repaint();

   private:
    friend class Timer;
    friend class Application;
    friend class Focus;
    friend void arrange(Widget&, Rect);
    template <typename T>
    friend auto track(T&) -> LifetimeView<T>;
    Rect bounds_;
    bool focused_ = false;
    std::shared_ptr<Widget*> lifetime_ = std::make_shared<Widget*>(this);
    Application* application_ = nullptr;
    std::vector<std::weak_ptr<Timer::State>> timers_;
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
concept WidgetDerived = std::is_base_of_v<Widget, T>;

template <WidgetDerived WidgetType>
auto operator|(WidgetType& w, SizePolicy sp) -> WidgetType&
{
    w.size_policy = sp;
    return w;
}

template <WidgetDerived WidgetType>
auto operator|(WidgetType&& w, SizePolicy sp) -> WidgetType
{
    w.size_policy = sp;
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
 * @details Provides template type deducation convinience.
 */
template <typename WidgetType>
[[nodiscard]] auto track(WidgetType& w) -> LifetimeView<WidgetType>
{
    static_assert(WidgetDerived<WidgetType>);
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

/**
 * Tracks each of the passed in Widget's lifetimes, if they are all valid when this slot
 * is called, it will call the given function with the Widgets as arguments.
 */
template <typename SlotFn, WidgetDerived... TrackedWidgets>
[[nodiscard]] auto tracked(SlotFn&& fn, TrackedWidgets&... widgets)
{
    // TODO constraint?
    return [fn = std::forward<SlotFn>(fn),
            ... lifetimes = track(widgets)](auto&&... args) mutable {
        if ((lifetimes.valid() && ...)) {
            std::invoke(fn, std::forward<decltype(args)>(args)..., lifetimes.get()...);
        }
    };
}

/**
 * Enables designated initializers and Lifetime tracking for Signal/Slot connections.
 * @details Use operator() immediately to connect and track Widgets if needed.
 */
template <typename SignalType, typename SlotType>
struct [[nodiscard("Use operator() to make actual connection.")]] Connection {
    SignalType& signal;
    SlotType slot;

    /**
     * This must be called in order to make the connection.
     */
    template <typename... TrackedWidgets>
    auto operator()(TrackedWidgets&... to_track) && -> sl::Identifier
    { return signal.connect(tracked(slot, to_track...)); }
};

}  // namespace ox