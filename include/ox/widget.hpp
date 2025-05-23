#pragma once

#include <memory>
#include <ranges>
#include <type_traits>

#include <zzz/coro.hpp>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>

namespace ox {

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
 * Policy for how a Widget should be sized by its parent layout.
 */
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;

    [[nodiscard]] static constexpr auto fixed(int size) -> SizePolicy
    {
        return {
            .minimum = size,
            .maximum = size,
            .flexibility = 0.f,
        };
    }

    [[nodiscard]] static constexpr auto flex(float flex = 1.f) -> SizePolicy
    {
        return {
            .minimum = 0,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = flex,
        };
    }

    [[nodiscard]] static constexpr auto bounded(int min, int max) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = max,
            .flexibility = 1.f,
        };
    }

    [[nodiscard]] static constexpr auto min(int min) -> SizePolicy
    {
        return {
            .minimum = min,
            .maximum = std::numeric_limits<int>::max(),
            .flexibility = 1.f,
        };
    }

    [[nodiscard]] static constexpr auto max(int max) -> SizePolicy
    {
        return {
            .minimum = 0,
            .maximum = max,
            .flexibility = 1.f,
        };
    }

    [[nodiscard]] static constexpr auto suspended(Area size) -> SizePolicy
    {
        // FIXME: This is a hack at the moment, it works, but each Layout type should
        // ideally define its own SizePolicy type.
        return {
            .minimum = size.width,
            .maximum = size.height,
            .flexibility = 0.f,
        };
    }
};

/**
 * Base class for interactive UI elements.
 */
class Widget {
   public:
    FocusPolicy focus_policy;
    SizePolicy size_policy;
    Terminal::Cursor cursor = std::nullopt;
    bool active = true;

    Point at = {.x = 0, .y = 0};
    Area size = {.width = 0, .height = 0};
    std::shared_ptr<Widget*> lifetime = std::make_shared<Widget*>(this);

   public:
    Widget(FocusPolicy fp = FocusPolicy::None, SizePolicy sp = SizePolicy::flex());

    Widget(Widget const&) = delete;
    Widget(Widget&& other);

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&;

    virtual ~Widget() = default;

   public:
    virtual void mouse_press(Mouse) {}

    virtual void mouse_release(Mouse) {}

    virtual void mouse_wheel(Mouse) {}

    virtual void mouse_move(Mouse) {}

    virtual void mouse_enter() {}

    virtual void mouse_leave() {}

    virtual void focus_in() {}

    virtual void focus_out() {}

    virtual void key_press(Key) {}

    virtual void key_release(Key) {}

    /**
     * this->size is updated by the time this is invoked. The previous size is passed in
     * as a parameter.
     */
    virtual void resize(Area /* old_size */) {}

    virtual void timer() {}

    virtual void paint(Canvas) {}

    virtual auto get_children() -> zzz::Generator<Widget&> { co_return; }

    virtual auto get_children() const -> zzz::Generator<Widget const&> { co_return; }
};

/**
 * Handle to a Widget Lifetime, can check if the Widget is still alive and get a ref.
 *
 * @details Use the track() function to create objects of this type.
 */
template <typename ConcreteType>
class LifetimeView {
   public:
    LifetimeView(std::shared_ptr<Widget*> wp) : widget_ptr_{wp} {}

   public:
    [[nodiscard]] auto valid() const -> bool { return !widget_ptr_.expired(); }

    [[nodiscard]] auto get() const -> ConcreteType&
    {
        return static_cast<ConcreteType&>(**(widget_ptr_.lock()));
    }

   private:
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
{
    return {w.lifetime};
}

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
    {
        return signal.connect(tracked(slot, to_track...));
    }
};

}  // namespace ox