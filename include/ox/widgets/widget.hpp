#pragma once

#include <cassert>
#include <climits>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <vector>

#include <ox/core.hpp>
#include <ox/widgets/focus.hpp>

namespace ox::widgets {

/**
 * Policy for how a widget should handle focus.
 * @details
 * None: The widget cannot be focused.
 * Tab: The widget can be focused with the tab key.
 * Click: The widget can be focused by clicking on it.
 * Strong: The widget can be focused by either tab or click.
 */
enum class FocusPolicy : std::uint8_t { None, Tab, Click, Strong };

/**
 * Runtime Concept Idiom based Widget class.
 *
 * @details This models a Widget concept that can be used to store any type that can act
 * like a widget. At a minimum a widget implements the paint free function:
 * `void paint(YourWidget const&, ox::Canvas);`
 * Looking at the friend functions in the implementation will give you an event handler
 * blueprint.
 */
class Widget {
   public:
    FocusPolicy focus_policy;

    bool enabled = true;  // TODO - use this somewhere.
    Point at = {0, 0};
    Area size = {0, 0};

   public:
    template <typename T>
    Widget(T&& t, FocusPolicy fp = FocusPolicy::None)
        : focus_policy{fp},
          self_{std::make_unique<Model<T>>(Model<T>{std::forward<T>(t)})}
    {}

    Widget(Widget const&) = delete;
    Widget(Widget&& other)
        : focus_policy{other.focus_policy},
          enabled{other.enabled},
          at{other.at},
          size{other.size},
          self_{std::move(other.self_)}
    {
        if (&other == Focus::in_focus_) {
            Focus::in_focus_ = this;
        }
    }

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&
    {
        focus_policy = other.focus_policy;
        enabled = other.enabled;
        at = other.at;
        size = other.size;
        self_ = std::move(other.self_);
        if (&other == Focus::in_focus_) {
            Focus::in_focus_ = this;
        }
        return *this;
    }

    ~Widget()
    {
        if (this == Focus::in_focus_) {
            Focus::in_focus_ = nullptr;
        }
    }

   public:
    /**
     * Retrieve the underlying concrete widget object.
     *
     * @details You must know the type of the underlying widget to use this, otherwise
     * this results in undefined behavior
     * @tparam T The type of the underlying data.
     * @return A reference to the underlying data.
     */
    template <typename T>
    [[nodiscard]] auto data() -> T&
    {
        assert(dynamic_cast<Model<T>*>(self_.get()) != nullptr);
        return static_cast<Model<T>*>(self_.get())->data();
    }

   public:
    friend void paint(Widget const& w, Canvas c) { w.self_->paint_(c); }
    friend void mouse_press(Widget& w, Mouse m) { w.self_->mouse_press_(m); }
    friend void mouse_release(Widget& w, Mouse m) { w.self_->mouse_release_(m); }
    friend void mouse_wheel(Widget& w, Mouse m) { w.self_->mouse_wheel_(m); }
    friend void mouse_move(Widget& w, Mouse m) { w.self_->mouse_move_(m); }
    friend void mouse_enter(Widget& w) { w.self_->mouse_enter_(); }
    friend void mouse_leave(Widget& w) { w.self_->mouse_leave_(); }
    friend void key_press(Widget& w, Key k) { w.self_->key_press_(k); }
    friend void key_release(Widget& w, Key k) { w.self_->key_release_(k); }
    friend void focus_in(Widget& w) { w.self_->focus_in_(); }
    friend void focus_out(Widget& w) { w.self_->focus_out_(); }
    friend void resize(Widget& w, Area a) { w.self_->resize_(a); }
    friend void timer(Widget& w, int id) { w.self_->timer_(id); }

    [[nodiscard]] friend auto cursor(Widget const& w) -> ox::Terminal::Cursor
    {
        return w.self_->cursor_();
    }

    [[nodiscard]] friend auto children(Widget& w) -> std::span<Widget>
    {
        return w.self_.get()->children_();
    }

    [[nodiscard]] friend auto children(Widget const& w) -> std::span<Widget const>
    {
        return static_cast<Concept const*>(w.self_.get())->children_();
    }

   private:
    class Concept {
       public:
        virtual ~Concept() = default;

        virtual void paint_(Canvas) const = 0;
        virtual void mouse_press_(Mouse) = 0;
        virtual void mouse_release_(Mouse) = 0;
        virtual void mouse_wheel_(Mouse) = 0;
        virtual void mouse_move_(Mouse) = 0;
        virtual void mouse_enter_() = 0;
        virtual void mouse_leave_() = 0;
        virtual void key_press_(Key) = 0;
        virtual void key_release_(Key) = 0;
        virtual void focus_in_() = 0;
        virtual void focus_out_() = 0;
        virtual void resize_(Area) = 0;
        virtual void timer_(int) = 0;

        virtual auto cursor_() const -> ox::Terminal::Cursor = 0;

        virtual auto children_() -> std::span<Widget> = 0;
        virtual auto children_() const -> std::span<Widget const> = 0;
    };

    template <typename T>
    class Model final : public Concept {
       public:
        Model(T t) : data_{std::move(t)} {}

       public:
        void paint_(Canvas c) const override
        {
            if constexpr (requires(T const& w, Canvas c) { paint(w, c); }) {
                paint(data_, c);
            }
        }

        void mouse_press_(Mouse m) override
        {
            if constexpr (requires(T& w, Mouse m) { mouse_press(w, m); }) {
                mouse_press(data_, m);
            }
        }

        void mouse_release_(Mouse m) override
        {
            if constexpr (requires(T& w, Mouse m) { mouse_release(w, m); }) {
                mouse_release(data_, m);
            }
        }

        void mouse_wheel_(Mouse m) override
        {
            if constexpr (requires(T& w, Mouse m) { mouse_wheel(w, m); }) {
                mouse_wheel(data_, m);
            }
        }

        void mouse_move_(Mouse m) override
        {
            if constexpr (requires(T& w, Mouse m) { mouse_move(w, m); }) {
                mouse_move(data_, m);
            }
        }

        void mouse_enter_() override
        {
            if constexpr (requires(T& w) { mouse_enter(w); }) {
                mouse_enter(data_);
            }
        }

        void mouse_leave_() override
        {
            if constexpr (requires(T& w) { mouse_leave(w); }) {
                mouse_leave(data_);
            }
        }

        void key_press_(Key k) override
        {
            if constexpr (requires(T& w, Key k) { key_press(w, k); }) {
                key_press(data_, k);
            }
        }

        void key_release_(Key k) override
        {
            if constexpr (requires(T& w, Key k) { key_release(w, k); }) {
                key_release(data_, k);
            }
        }

        void focus_in_() override
        {
            if constexpr (requires(T& w) { focus_in(w); }) {
                focus_in(data_);
            }
        }

        void focus_out_() override
        {
            if constexpr (requires(T& w) { focus_out(w); }) {
                focus_out(data_);
            }
        }

        void resize_(Area a) override
        {
            if constexpr (requires(T& w, Area a) { resize(w, a); }) {
                resize(data_, a);
            }
        }

        void timer_(int id) override
        {
            if constexpr (requires(T& w, int id) { timer(w, id); }) {
                timer(data_, id);
            }
        }

        auto cursor_() const -> ox::Terminal::Cursor override
        {
            if constexpr (requires(T& w) { cursor(w); }) {
                return cursor(data_);
            }
            else {
                return std::nullopt;
            }
        }

        auto children_() -> std::span<Widget> override
        {
            if constexpr (requires(T& w) { children(w); }) {
                return children(data_);
            }
            else {
                return {};
            }
        }

        auto children_() const -> std::span<Widget const> override
        {
            if constexpr (requires(T const& w) { children(w); }) {
                return children(data_);
            }
            else {
                return {};
            }
        }

       public:
        [[nodiscard]] auto data() -> T& { return data_; }
        [[nodiscard]] auto data() const -> T const& { return data_; }

       private:
        T data_;
    };

   private:
    std::unique_ptr<Concept> self_;
};

/**
 * Execute \p fn on \p w and every descendant of \p w, in a depth first traversal.
 *
 * @param w The Widget to start the traversal from.
 * @param fn The function to execute on each Widget.
 */
inline auto for_each_depth_first(Widget& w,
                                 std::function<void(Widget&)> const& fn) -> void
{
    fn(w);
    for (Widget& child : children(w)) {
        for_each_depth_first(child, fn);
    }
}

/**
 * Execute \p fn on \p w and every descendant of \p w, in a depth first traversal.
 *
 * @param w The Widget to start the traversal from.
 * @param fn The function to execute on each Widget.
 */
inline auto for_each_depth_first(Widget const& w,
                                 std::function<void(Widget const&)> const& fn) -> void
{
    fn(w);
    for (Widget const& child : children(w)) {
        for_each_depth_first(child, fn);
    }
}

/**
 * Find the first Widget in \p w and its descendants that satisfies \p predicate.
 *
 * @details This includes \p w itself in the search.
 * @param w The Widget to start the search from.
 * @param predicate The function to test each Widget with.
 * @returns A pointer to the first Widget that satisfies \p predicate, or nullptr if no
 * Widget is found.
 */
[[nodiscard]] inline auto find_if_depth_first(
    Widget& w,
    std::function<bool(Widget const&)> const& predicate) -> Widget*
{
    Widget* result = nullptr;
    for_each_depth_first(w, [&](Widget& child) {
        if (result == nullptr && predicate(child)) {
            result = &child;
        }
    });
    return result;
}

/**
 * Find the first Widget in \p w and its descendants that satisfies \p predicate.
 *
 * @details This includes \p w itself in the search.
 * @param w The Widget to start the search from.
 * @param predicate The function to test each Widget with.
 * @returns A pointer to the first Widget that satisfies \p predicate, or nullptr if no
 * Widget is found.
 */
[[nodiscard]] inline auto find_if_depth_first(
    Widget const& w,
    std::function<bool(Widget const&)> const& predicate) -> Widget const*
{
    Widget const* result = nullptr;
    for_each_depth_first(w, [&](Widget const& child) {
        if (result == nullptr && predicate(child)) {
            result = &child;
        }
    });
    return result;
}

/**
 * Find the Widget in \p widgets that contains the Point \p p.
 *
 * @details This does not check descendants of the Widgets in \p widgets. \p p is
 * relative to the parent of the passed in Widgets; the top left corner of the parent is
 * {0, 0}.
 * @param widgets The Widgets to search.
 * @param p The Point to search for.
 * @returns A pointer to the Widget that contains \p p, or nullptr if no Widget.
 */
[[nodiscard]] inline auto find_widget_at(std::span<Widget> widgets,
                                         ox::Point p) -> Widget*
{
    auto const at = std::ranges::find_if(widgets, [p](Widget& child) {
        return child.enabled && child.at.x <= p.x &&
               p.x < child.at.x + child.size.width && child.at.y <= p.y &&
               p.y < child.at.y + child.size.height;
    });

    return (at == std::end(widgets)) ? nullptr : &*at;
}

}  // namespace ox::widgets