#pragma once

#include <memory>
#include <vector>

#include <termox/core.hpp>
#include <termox/widgets/focus.hpp>

namespace ox::widgets {

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
    template <typename T>
    Widget(T t) : self_{std::make_unique<Model<T>>(std::move(t))}
    {}

    Widget(Widget const&) = delete;
    Widget(Widget&& other) : self_{std::move(other.self_)}
    {
        if (&other == Focus::in_focus_) {
            Focus::in_focus_ = this;
        }
    }

    auto operator=(Widget const&) -> Widget& = delete;
    auto operator=(Widget&& other) -> Widget&
    {
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
    friend void paint(Widget const& w, Canvas c) { w.self_->paint_(c); }
    friend void mouse_press(Widget& w, Mouse m) { w.self_->mouse_press_(m); }
    friend void mouse_release(Widget& w, Mouse m)
    {
        w.self_->mouse_release_(m);
    }
    friend void mouse_wheel(Widget& w, Mouse m) { w.self_->mouse_wheel_(m); }
    friend void mouse_move(Widget& w, Mouse m) { w.self_->mouse_move_(m); }
    friend void key_press(Widget& w, Key k) { w.self_->key_press_(k); }
    friend void key_release(Widget& w, Key k) { w.self_->key_release_(k); }
    friend void focus_in(Widget& w) { w.self_->focus_in_(); }
    friend void focus_out(Widget& w) { w.self_->focus_out_(); }
    friend void resize(Widget& w, Area a) { w.self_->resize_(a); }
    friend void timer(Widget& w, int id) { w.self_->timer_(id); }

   private:
    struct Concept {
        virtual ~Concept() = default;

        virtual void paint_(Canvas) const  = 0;
        virtual void mouse_press_(Mouse)   = 0;
        virtual void mouse_release_(Mouse) = 0;
        virtual void mouse_wheel_(Mouse)   = 0;
        virtual void mouse_move_(Mouse)    = 0;
        virtual void key_press_(Key)       = 0;
        virtual void key_release_(Key)     = 0;
        virtual void focus_in_()           = 0;
        virtual void focus_out_()          = 0;
        virtual void resize_(Area)         = 0;
        virtual void timer_(int)           = 0;
    };

    template <typename T>
    class Model final : public Concept {
       public:
        Model(T t) : data_{std::move(t)} {}

       public:
        auto data() -> T& { return data_; }

        auto data() const -> const T& { return data_; }

       public:
        void paint_(Canvas c) const override
        {
            static_assert(
                requires(T const& w, Canvas c) { paint(w, c); },
                "Widget must have a paint function: void paint(T const&, "
                "Canvas)");
            paint(data_, c);
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

       private:
        T data_;
    };

   private:
    std::unique_ptr<Concept> self_;
};

}  // namespace ox::widgets