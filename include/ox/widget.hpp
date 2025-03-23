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
 * Policy for how a widget should be sized by its parent layout.
 */
struct SizePolicy {
    int minimum = 0;
    int maximum = std::numeric_limits<int>::max();
    float flexibility = 1.f;

    [[nodiscard]] static auto fixed(int size) -> SizePolicy;
    [[nodiscard]] static auto flex(float flex = 1.f) -> SizePolicy;
    [[nodiscard]] static auto bounded(int min, int max) -> SizePolicy;
    [[nodiscard]] static auto min(int min) -> SizePolicy;
    [[nodiscard]] static auto max(int max) -> SizePolicy;
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
    Widget(FocusPolicy fp, SizePolicy sp) : focus_policy{fp}, size_policy{sp} {}

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

    virtual void timer(int /* id */) {}

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
            fn(lifetimes.get()..., std::forward<decltype(args)>(args)...);
        }
    };
}

}  // namespace ox