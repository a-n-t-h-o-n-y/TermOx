#pragma once

#include <climits>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include <termox/core.hpp>
#include <termox/widgets/focus.hpp>

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
 * Policy for how a widget should be sized by its parent layout.
 */
struct SizePolicy {
    int min    = 0;
    int max    = std::numeric_limits<int>::max();
    float flex = 1.f;
};

/**
 * Runtime Concept Idiom based Widget class.
 *
 * @details This models a Widget concept that can be used to store any type that
 * can act like a widget. At a minimum a widget implements the paint free
 * function: `void paint(YourWidget const&, ox::Canvas);`
 * Looking at the friend functions in the implementation will give you an event
 * handler blueprint.
 */
class Widget {
   public:
    struct Properties {
        FocusPolicy focus_policy = FocusPolicy::None;
        SizePolicy size_policy   = {};
        // bool enabled = true; // TODO
        Point at;
        Area size;
    } properties;

   public:
    template <typename T>
    Widget(T t, Properties p)
        : properties{std::move(p)},
          self_{std::make_unique<Model<T>>(std::move(t))}
    {}

    Widget(Widget const&) = delete;
    Widget(Widget&& other)
        : properties{std::move(other.properties)}, self_{std::move(other.self_)}
    {
        if (&other == Focus::in_focus_) {
            Focus::in_focus_ = this;
        }
    }

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&
    {
        properties = std::move(other.properties);
        self_      = std::move(other.self_);
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
     * @details Please do not use this function for anything but the core
     * library implementation. You must know the type stored. Being used by
     * insert/append in layouts to return a stable reference to the inserted
     * widget.
     *
     * @tparam T The type of the underlying data.
     * @return A reference to the underlying data.
     */
    template <typename T>
    [[nodiscard]] auto data() -> T&
    {
        return static_cast<Model<T>*>(self_.get())->data();
    }

   public:
    friend void paint(Widget const& w, Canvas c)
    {
        w.self_->paint_(c, w.properties);
    }
    friend void mouse_press(Widget& w, Mouse m)
    {
        w.self_->mouse_press_(m, w.properties);
    }
    friend void mouse_release(Widget& w, Mouse m)
    {
        w.self_->mouse_release_(m, w.properties);
    }
    friend void mouse_wheel(Widget& w, Mouse m)
    {
        w.self_->mouse_wheel_(m, w.properties);
    }
    friend void mouse_move(Widget& w, Mouse m)
    {
        w.self_->mouse_move_(m, w.properties);
    }
    friend void key_press(Widget& w, Key k)
    {
        w.self_->key_press_(k, w.properties);
    }
    friend void key_release(Widget& w, Key k)
    {
        w.self_->key_release_(k, w.properties);
    }
    friend void focus_in(Widget& w) { w.self_->focus_in_(w.properties); }
    friend void focus_out(Widget& w) { w.self_->focus_out_(w.properties); }
    friend void resize(Widget& w, Area a) { w.self_->resize_(a, w.properties); }
    friend void timer(Widget& w, int id) { w.self_->timer_(id, w.properties); }

    friend auto find_next_tab_focus(Widget& w,
                                    Widget const* current_focus,
                                    bool is_active) -> Widget*
    {
        return w.self_->find_next_tab_focus_(current_focus, is_active);
    }

    friend void for_each(Widget& w, std::function<void(Widget&)> const& fn)
    {
        w.self_->for_each_(fn);
    }
    friend void for_each(Widget const& w,
                         std::function<void(Widget const&)> const& fn)
    {
        const_cast<Concept const*>(w.self_.get())->for_each_(fn);
    }

    friend auto find_if(Widget& w,
                        std::function<bool(Widget const&)> const& predicate)
        -> Widget*
    {
        return w.self_->find_if_(predicate);
    }
    friend auto find_if(Widget const& w,
                        std::function<bool(Widget const&)> const& predicate)
        -> Widget const*
    {
        return const_cast<Concept const*>(w.self_.get())->find_if_(predicate);
    }

   private:
    struct Concept {
        virtual ~Concept() = default;

        virtual void paint_(Canvas, Properties const&) const = 0;
        virtual void mouse_press_(Mouse, Properties&)        = 0;
        virtual void mouse_release_(Mouse, Properties&)      = 0;
        virtual void mouse_wheel_(Mouse, Properties&)        = 0;
        virtual void mouse_move_(Mouse, Properties&)         = 0;
        virtual void key_press_(Key, Properties&)            = 0;
        virtual void key_release_(Key, Properties&)          = 0;
        virtual void focus_in_(Properties& p)                = 0;
        virtual void focus_out_(Properties& p)               = 0;
        virtual void resize_(Area, Properties&)              = 0;
        virtual void timer_(int, Properties&)                = 0;

        virtual auto find_next_tab_focus_(Widget const*, bool) -> Widget*   = 0;
        virtual auto for_each_(std::function<void(Widget&)> const&) -> void = 0;
        virtual auto for_each_(std::function<void(Widget const&)> const&) const
            -> void = 0;
        virtual auto find_if_(std::function<bool(Widget const&)> const&)
            -> Widget* = 0;
        virtual auto find_if_(std::function<bool(Widget const&)> const&) const
            -> Widget const* = 0;
    };

    template <typename T>
    class Model final : public Concept {
       public:
        Model(T t) : data_{std::move(t)} {}

       public:
        auto data() -> T& { return data_; }

        auto data() const -> const T& { return data_; }

       public:
        void paint_(Canvas c, Properties const& p) const override
        {
            if constexpr (requires(T const& w, Canvas c, Properties const& p) {
                              paint(w, c, p);
                          }) {
                paint(data_, c, p);
            }
            else if constexpr (requires(T const& w, Canvas c) {
                                   paint(w, c);
                               }) {
                paint(data_, c);
            }
        }

        void mouse_press_(Mouse m, Properties& p) override
        {
            if constexpr (requires(T& w, Mouse m, Properties& p) {
                              mouse_press(w, m, p);
                          }) {
                mouse_press(data_, m, p);
            }
            else if constexpr (requires(T& w, Mouse m) { mouse_press(w, m); }) {
                mouse_press(data_, m);
            }
        }

        void mouse_release_(Mouse m, Properties& p) override
        {
            if constexpr (requires(T& w, Mouse m, Properties& p) {
                              mouse_release(w, m, p);
                          }) {
                mouse_release(data_, m, p);
            }
            else if constexpr (requires(T& w, Mouse m) {
                                   mouse_release(w, m);
                               }) {
                mouse_release(data_, m);
            }
        }

        void mouse_wheel_(Mouse m, Properties& p) override
        {
            if constexpr (requires(T& w, Mouse m, Properties& p) {
                              mouse_wheel(w, m, p);
                          }) {
                mouse_wheel(data_, m, p);
            }
            else if constexpr (requires(T& w, Mouse m) { mouse_wheel(w, m); }) {
                mouse_wheel(data_, m);
            }
        }

        void mouse_move_(Mouse m, Properties& p) override
        {
            if constexpr (requires(T& w, Mouse m, Properties& p) {
                              mouse_move(w, m, p);
                          }) {
                mouse_move(data_, m, p);
            }
            else if constexpr (requires(T& w, Mouse m) { mouse_move(w, m); }) {
                mouse_move(data_, m);
            }
        }

        void key_press_(Key k, Properties& p) override
        {
            if constexpr (requires(T& w, Key k, Properties&) {
                              key_press(w, k, p);
                          }) {
                key_press(data_, k, p);
            }
            else if constexpr (requires(T& w, Key k) { key_press(w, k); }) {
                key_press(data_, k);
            }
        }

        void key_release_(Key k, Properties& p) override
        {
            if constexpr (requires(T& w, Key k, Properties& p) {
                              key_release(w, k, p);
                          }) {
                key_release(data_, k, p);
            }
            else if constexpr (requires(T& w, Key k) { key_release(w, k); }) {
                key_release(data_, k);
            }
        }

        void focus_in_(Properties& p) override
        {
            if constexpr (requires(T& w, Properties& p) { focus_in(w, p); }) {
                focus_in(data_, p);
            }
            else if constexpr (requires(T& w) { focus_in(w); }) {
                focus_in(data_);
            }
        }

        void focus_out_(Properties& p) override
        {
            if constexpr (requires(T& w, Properties& p) { focus_out(w, p); }) {
                focus_out(data_, p);
            }
            else if constexpr (requires(T& w) { focus_out(w); }) {
                focus_out(data_);
            }
        }

        void resize_(Area a, Properties& p) override
        {
            if constexpr (requires(T& w, Area a, Properties& p) {
                              resize(w, a, p);
                          }) {
                resize(data_, a, p);
            }
            else if constexpr (requires(T& w, Area a) { resize(w, a); }) {
                resize(data_, a);
            }
        }

        void timer_(int id, Properties& p) override
        {
            if constexpr (requires(T& w, int id, Properties& p) {
                              timer(w, id, p);
                          }) {
                timer(data_, id, p);
            }
            else if constexpr (requires(T& w, int id) { timer(w, id); }) {
                timer(data_, id);
            }
        }

        auto find_next_tab_focus_(Widget const* current_focus, bool is_active)
            -> Widget* override
        {
            if constexpr (requires(T& w, Widget const* cf, bool ia) {
                              find_next_tab_focus(w, cf, ia);
                          }) {
                return find_next_tab_focus(data_, current_focus, is_active);
            }
            else {
                return nullptr;
            }
        }

        auto for_each_(std::function<void(Widget&)> const& fn) -> void override
        {
            if constexpr (requires(T& w,
                                   std::function<void(Widget&)> const& fn) {
                              for_each(w, fn);
                          }) {
                for_each(data_, fn);
            }
        }

        auto for_each_(std::function<void(Widget const&)> const& fn) const
            -> void override
        {
            if constexpr (requires(
                              T const& w,
                              std::function<void(Widget const&)> const& fn) {
                              for_each(w, fn);
                          }) {
                for_each(data_, fn);
            }
        }

        auto find_if_(std::function<bool(Widget const&)> const& predicate)
            -> Widget* override
        {
            if constexpr (requires(T& w,
                                   std::function<bool(Widget const&)> const&
                                       predicate) { find_if(w, predicate); }) {
                return find_if(data_, predicate);
            }
            else {
                return nullptr;
            }
        }

        auto find_if_(std::function<bool(Widget const&)> const& predicate) const
            -> Widget const* override
        {
            if constexpr (requires(T const& w,
                                   std::function<bool(Widget const&)> const&
                                       predicate) { find_if(w, predicate); }) {
                return find_if(data_, predicate);
            }
            else {
                return nullptr;
            }
        }

       private:
        T data_;
    };

   private:
    std::unique_ptr<Concept> self_;
};

}  // namespace ox::widgets