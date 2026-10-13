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
    Cursor cursor = std::nullopt;
    bool active = true;
    bool enabled = true;
    bool focused = false;

    Point at = {.x = 0, .y = 0};
    Area size = {.width = 0, .height = 0};
    std::shared_ptr<Widget*> lifetime = std::make_shared<Widget*>(this);

   public:
    Widget(FocusPolicy fp = FocusPolicy::None, SizePolicy sp = SizePolicy::flex());

    Widget(Widget const&) = delete;
    Widget(Widget&& other);

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&;

    virtual ~Widget();

   public:
    virtual auto mouse_press(MousePress) -> EventResponse { return {}; }

    virtual auto mouse_release(MouseRelease) -> EventResponse { return {}; }

    virtual auto mouse_wheel(MouseWheel) -> EventResponse { return {}; }

    virtual auto mouse_move(MouseMove) -> EventResponse { return {}; }

    virtual auto mouse_enter() -> EventResponse { return {}; }

    virtual auto mouse_leave() -> EventResponse { return {}; }

    virtual auto focus_in() -> EventResponse { return {}; }

    virtual auto focus_out() -> EventResponse { return {}; }

    virtual auto key_press(KeyEvent) -> EventResponse { return {}; }

    virtual auto key_release(KeyEvent) -> EventResponse { return {}; }

    virtual auto paste(esc::PasteChunk const&) -> EventResponse { return {}; }

    /**
     * this->size is updated by the time this is invoked. The previous size is passed in
     * as a parameter.
     */
    virtual void resize(Area /* old_size */) {}

    virtual auto timer() -> EventResponse { return {}; }

    virtual void paint(Canvas) {}

    virtual auto get_children() -> zzz::Generator<Widget&> { co_return; }

    virtual auto get_children() const -> zzz::Generator<Widget const&> { co_return; }

   protected:
    /** Schedule painting after a direct widget mutation, if attached. */
    void request_repaint();

   private:
    friend class Timer;
    friend class Application;
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
    LifetimeView(std::shared_ptr<Widget*> wp) : widget_ptr_{wp} {}

   public:
    [[nodiscard]] auto valid() const -> bool
    {
        auto pointer = widget_ptr_.lock();
        return pointer && *pointer;
    }
    template <typename Other>
    auto operator==(LifetimeView<Other> const& other) const -> bool
    {
        return !widget_ptr_.owner_before(other.widget_ptr_) &&
               !other.widget_ptr_.owner_before(widget_ptr_);
    }

    [[nodiscard]] auto get() const -> ConcreteType&
    {
        auto pointer = widget_ptr_.lock();
        if (!pointer || !*pointer) {
            throw std::logic_error{"Expired widget lifetime"};
        }
        return static_cast<ConcreteType&>(**pointer);
    }

   private:
    template <typename>
    friend class LifetimeView;
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

inline constexpr auto is_active =
    std::views::filter([](Widget const& w) { return w.active; });

}  // namespace filter

/**
 * Create a handle to the lifetime of the given Widget.
 *
 * @details Provides template type deducation convinience.
 */
template <WidgetDerived WidgetType>
[[nodiscard]] auto track(WidgetType& w) -> LifetimeView<WidgetType>
{ return {w.lifetime}; }

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
            fn(std::forward<decltype(args)>(args)..., lifetimes.get()...);
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