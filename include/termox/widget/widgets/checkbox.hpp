#ifndef TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
#define TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/common/overload.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
template <template <typename> typename Layout_t>
class Checkbox : public Label<Layout_t> {
   private:
    using Base_t = Label<Layout_t>;

   public:
    enum class State : bool { Unchecked, Checked };

    struct Display {
        Glyph_string checked;
        Glyph_string unchecked;
    };

    struct Parameters {
        State initial_state;
        Display display;
        bool locked;
    };

   public:
    /// Emitted when box becomes checked.
    sl::Signal<void()> checked;

    /// Emitted when box becomes unchecked.
    sl::Signal<void()> unchecked;

    /// Emitted every time the box changes state.
    sl::Signal<void()> toggled;

   public:
    /// Construct a new Checkbox
    explicit Checkbox(State initial_state, Display display, bool locked)
        : Base_t{initial_state == State::Unchecked ? display.unchecked
                                                   : display.checked,
                 Align::Left, 0, 0, Growth::Dynamic},
          state_{initial_state},
          display_{display},
          locked_{locked}
    {
        if (locked_) {
            display_.checked | Trait::Dim;
            display_.unchecked | Trait::Dim;
            this->update_display();
        }
    }

    /// Construct a new Checkbox
    explicit Checkbox(Parameters p)
        : Checkbox{p.initial_state, p.display, p.locked} {};

   public:
    /// Set the state to be checked.
    void check()
    {
        if (locked_)
            return;
        if (state_ == State::Unchecked) {
            state_ = State::Checked;
            this->Base_t::set_text(display_.checked);
            checked.emit();
            toggled.emit();
        }
    }

    /// Set the state to be unchecked.
    void uncheck()
    {
        if (locked_)
            return;
        if (state_ == State::Checked) {
            state_ = State::Unchecked;
            this->Base_t::set_text(display_.unchecked);
            unchecked.emit();
            toggled.emit();
        }
    }

    /// Change state to be unchecked if currently checked, checked otherwise.
    void toggle()
    {
        switch (state_) {
            case State::Checked: this->uncheck(); break;
            case State::Unchecked: this->check(); break;
        }
    }

    /// Return the current state of the Checkbox as Checkbox::State enum value.
    auto get_state() const -> State { return state_; }

    /// Lock the Checkbox, it can not be toggled when locked.
    void lock()
    {
        locked_ = true;
        display_.checked | Trait::Dim;
        display_.unchecked | Trait::Dim;
        this->update_display();
    }

    /// Unlock the Checkbox, allowing it to be toggled.
    void unlock()
    {
        locked_ = false;
        display_.checked.remove_traits(Trait::Dim);
        display_.unchecked.remove_traits(Trait::Dim);
        this->update_display();
    }

    /// Return true if the Checkbox is locked.
    auto is_locked() const -> bool { return locked_; }

    /// Set the look of each Checkbox State.
    void set_display(Display d)
    {
        display_ = std::move(d);
        if (locked_) {
            display_.checked | Trait::Dim;
            display_.unchecked | Trait::Dim;
        }
        this->update_display();
    }

    /// Return the look of each Checkbox State.
    auto get_display() -> Display
    {
        auto result = display_;
        if (locked_) {
            result.checked.remove_traits(Trait::Dim);
            result.unchecked.remove_traits(Trait::Dim);
        }
        return result;
    }

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->toggle();
        return Base_t::mouse_press_event(m);
    }

   private:
    State state_;
    Display display_;
    bool locked_;

   private:
    /// Push changes to display_ to the Label base class to update the display.
    void update_display()
    {
        this->Base_t::set_text(state_ == State::Unchecked ? display_.unchecked
                                                          : display_.checked);
    }
};

using HCheckbox = Checkbox<layout::Horizontal>;
using VCheckbox = Checkbox<layout::Vertical>;

/// Helper function to create an HCheckbox instance.
template <typename... Args>
auto hcheckbox(Args&&... args) -> std::unique_ptr<HCheckbox>
{
    return std::make_unique<HCheckbox>(std::forward<Args>(args)...);
}

/// Helper function to create a VCheckbox instance.
template <typename... Args>
auto vcheckbox(Args&&... args) -> std::unique_ptr<VCheckbox>
{
    return std::make_unique<VCheckbox>(std::forward<Args>(args)...);
}

}  // namespace ox
namespace ox::detail {

/// Label, buffer and Checkbox tuple implementation.
template <template <typename> typename Layout_t,
          typename Checkbox_type,
          bool label_last>
class Label_checkbox_impl
    : public Label_wrapper<Layout_t, Checkbox_type, Layout_t, label_last> {
   private:
    using Base_t = Label_wrapper<Layout_t, Checkbox_type, Layout_t, label_last>;

   public:
    using Label_wrapper_t = Base_t;
    using Checkbox_t      = Checkbox_type;
    using Label_t         = Label<Layout_t>;

   public:
    Checkbox_t& checkbox = Base_t::wrapped;
    Label_t& label       = Base_t::label;

   public:
    explicit Label_checkbox_impl(
        typename Base_t::Parameters label_parameters        = {},
        typename Checkbox_t::Parameters checkbox_parameters = {})
        : Base_t{std::move(label_parameters), std::move(checkbox_parameters)}
    {
        using namespace pipe;
        Base_t::label | on_mouse_press([&](auto) { checkbox.toggle(); });
        Base_t::padding | on_mouse_press([&](auto) { checkbox.toggle(); });
    }

   private:
    using Base_t::wrapped;
};

}  // namespace ox::detail

namespace ox {

/// Template function to build a specified Labeled_checkbox type.
/** T is some sort of Labeled_checkbox type. */
template <typename T>
inline auto constexpr labeled_checkbox =
    [](typename T::Label_wrapper_t::Parameters label_parameters = {},
       typename T::Checkbox_t::Parameters checkbox_parameters   = {})
    -> std::unique_ptr<T> {
    return std::make_unique<T>(std::move(label_parameters),
                               std::move(checkbox_parameters));
};

/// Template function to build a specified Labeled_checkbox type.
/** T is some sort of Labeled_checkbox type. */
template <typename Checkbox_t>
inline auto constexpr make_checkbox = Overload{
    [](typename Checkbox_t::Parameters parameters = {})
        -> std::unique_ptr<Checkbox_t> {
        return std::make_unique<Checkbox_t>(std::move(parameters));
    },
    [](typename Checkbox_t::State initial_state,
       bool locked) -> std::unique_ptr<Checkbox_t> {
        return std::make_unique<Checkbox_t>(initial_state, locked);
    },
};

// ☒
class Checkbox1 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox1(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"☒", L"☐"}, locked}
    {}

    Checkbox1(Parameters p) : Checkbox1{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox1 = make_checkbox<Checkbox1>;

using HCheckbox1_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox1, true>;
using HCheckbox_label = HCheckbox1_label;

using HLabel_checkbox1 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox1, false>;
using HLabel_checkbox = HLabel_checkbox1;

using VCheckbox1_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox1, true>;
using VCheckbox_label = VCheckbox1_label;

using VLabel_checkbox1 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox1, false>;
using VLabel_checkbox = VLabel_checkbox1;

inline auto constexpr hcheckbox1_label = labeled_checkbox<HCheckbox_label>;
inline auto constexpr hcheckbox_label  = hcheckbox1_label;
inline auto constexpr hlabel_checkbox1 = labeled_checkbox<HLabel_checkbox>;
inline auto constexpr hlabel_checkbox  = hlabel_checkbox1;
inline auto constexpr vcheckbox1_label = labeled_checkbox<VCheckbox_label>;
inline auto constexpr vcheckbox_label  = vcheckbox1_label;
inline auto constexpr vlabel_checkbox1 = labeled_checkbox<VLabel_checkbox>;
inline auto constexpr vlabel_checkbox  = vlabel_checkbox1;

// [x]
class Checkbox2 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox2(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"[x]", L"[ ]"}, locked}
    {}

    Checkbox2(Parameters p) : Checkbox2{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox2 = make_checkbox<Checkbox2>;

using HCheckbox2_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox2, true>;
using HLabel_checkbox2 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox2, false>;
using VCheckbox2_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox2, true>;
using VLabel_checkbox2 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox2, false>;

inline auto constexpr hcheckbox2_label = labeled_checkbox<HCheckbox2_label>;
inline auto constexpr hlabel_checkbox2 = labeled_checkbox<HLabel_checkbox2>;
inline auto constexpr vcheckbox2_label = labeled_checkbox<VCheckbox2_label>;
inline auto constexpr vlabel_checkbox2 = labeled_checkbox<VLabel_checkbox2>;

// ┌
// x
// └
class Checkbox3 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox3(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"┌x└", L"┌ └"}, locked}
    {}

    Checkbox3(Parameters p) : Checkbox3{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox3 = make_checkbox<Checkbox3>;

using HCheckbox3_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox3, true>;

inline auto constexpr hcheckbox3_label = labeled_checkbox<HCheckbox3_label>;

// ┐
// x
// ┘
class Checkbox4 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox4(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"┐x┘", L"┐ ┘"}, locked}
    {}

    Checkbox4(Parameters p) : Checkbox4{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox4 = make_checkbox<Checkbox4>;

using HLabel_checkbox4 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox4, false>;

inline auto constexpr hlabel_checkbox4 = labeled_checkbox<HLabel_checkbox4>;

// ┌x┐
class Checkbox5 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox5(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"┌x┐", L"┌ ┐"}, locked}
    {}

    Checkbox5(Parameters p) : Checkbox5{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox5 = make_checkbox<Checkbox5>;

using VCheckbox5_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox5, true>;

inline auto constexpr vcheckbox5_label = labeled_checkbox<VCheckbox5_label>;

// └x┘
class Checkbox6 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox6(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"└x┘", L"└ ┘"}, locked}
    {}

    Checkbox6(Parameters p) : Checkbox6{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox6 = make_checkbox<Checkbox6>;

using VLabel_checkbox6 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox6, false>;

inline auto constexpr vlabel_checkbox6 = labeled_checkbox<VLabel_checkbox6>;

// ╭
// x
// ╰
class Checkbox7 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox7(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"╭x╰", L"╭ ╰"}, locked}
    {}

    Checkbox7(Parameters p) : Checkbox7{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox7 = make_checkbox<Checkbox7>;

using HCheckbox7_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox7, true>;

inline auto constexpr hcheckbox7_label = labeled_checkbox<HCheckbox7_label>;

// ╮
// x
// ╯
class Checkbox8 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox8(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"╮x╯", L"╮ ╯"}, locked}
    {}

    Checkbox8(Parameters p) : Checkbox8{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox8 = make_checkbox<Checkbox8>;

using HLabel_checkbox8 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox8, false>;

inline auto constexpr hlabel_checkbox8 = labeled_checkbox<HLabel_checkbox8>;

// ╭x╮
class Checkbox9 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox9(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"╭x╮", L"╭ ╮"}, locked}
    {}

    Checkbox9(Parameters p) : Checkbox9{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox9 = make_checkbox<Checkbox9>;

using VCheckbox9_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox9, true>;

inline auto constexpr vcheckbox9_label = labeled_checkbox<VCheckbox9_label>;

// ╰x╯
class Checkbox10 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox10(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"╰x╯", L"╰ ╯"}, locked}
    {}

    Checkbox10(Parameters p) : Checkbox10{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox10 = make_checkbox<Checkbox10>;

using VLabel_checkbox10 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox10, false>;

inline auto constexpr vlabel_checkbox10 = labeled_checkbox<VLabel_checkbox10>;

// -----------------------------------------------------------------------------

// ┘
// ╴
// ┐
class Checkbox11 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox11(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"┘╴┐", L"┘ ┐"}, locked}
    {}

    Checkbox11(Parameters p) : Checkbox11{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox11 = make_checkbox<Checkbox11>;

using HCheckbox11_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox11, true>;

inline auto constexpr hcheckbox11_label = labeled_checkbox<HCheckbox11_label>;

// └
// ╶
// ┌
class Checkbox12 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox12(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"└╶┌", L"└ ┌"}, locked}
    {}

    Checkbox12(Parameters p) : Checkbox12{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox12 = make_checkbox<Checkbox12>;

using HLabel_checkbox12 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox12, false>;

inline auto constexpr hlabel_checkbox12 = labeled_checkbox<HLabel_checkbox12>;

// ┘╵└
class Checkbox13 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox13(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"┘╵└", L"┘ └"}, locked}
    {}

    Checkbox13(Parameters p) : Checkbox13{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox13 = make_checkbox<Checkbox13>;

using VCheckbox13_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox13, true>;

inline auto constexpr vcheckbox13_label = labeled_checkbox<VCheckbox13_label>;

// ┐╷┌
class Checkbox14 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox14(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"┐╷┌", L"┐ ┌"}, locked}
    {}

    Checkbox14(Parameters p) : Checkbox14{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox14 = make_checkbox<Checkbox14>;

using VLabel_checkbox14 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox14, false>;

inline auto constexpr vlabel_checkbox14 = labeled_checkbox<VLabel_checkbox14>;

// ╯
// ╴
// ╮
class Checkbox15 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox15(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"╯╴╮", L"╯ ╮"}, locked}
    {}

    Checkbox15(Parameters p) : Checkbox15{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox15 = make_checkbox<Checkbox15>;

using HCheckbox15_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox15, true>;

inline auto constexpr hcheckbox15_label = labeled_checkbox<HCheckbox15_label>;

// ╰
// ╶
// ╭
class Checkbox16 : public VCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox16(State initial_state = State::Unchecked, bool locked = false)
        : VCheckbox{initial_state, {L"╰╶╭", L"╰ ╭"}, locked}
    {}

    Checkbox16(Parameters p) : Checkbox16{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox16 = make_checkbox<Checkbox16>;

using HLabel_checkbox16 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox16, false>;

inline auto constexpr hlabel_checkbox16 = labeled_checkbox<HLabel_checkbox16>;

// ╯╵╰
class Checkbox17 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox17(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"╯╵╰", L"╯ ╰"}, locked}
    {}

    Checkbox17(Parameters p) : Checkbox17{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox17 = make_checkbox<Checkbox17>;

using VCheckbox17_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox17, true>;

inline auto constexpr vcheckbox17_label = labeled_checkbox<VCheckbox17_label>;

// ╮╷╭
class Checkbox18 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox18(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"╮╷╭", L"╮ ╭"}, locked}
    {}

    Checkbox18(Parameters p) : Checkbox18{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox18 = make_checkbox<Checkbox18>;

using VLabel_checkbox18 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox18, false>;

inline auto constexpr vlabel_checkbox18 = labeled_checkbox<VLabel_checkbox18>;

// -----------------------------------------------------------------------------

// ├x┤
class Checkbox19 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox19(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"├x┤", L"├ ┤"}, locked}
    {}

    Checkbox19(Parameters p) : Checkbox19{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox19 = make_checkbox<Checkbox19>;

using HCheckbox19_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox19, true>;
using HLabel_checkbox19 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox19, false>;
using VCheckbox19_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox19, true>;
using VLabel_checkbox19 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox19, false>;

inline auto constexpr hcheckbox19_label = labeled_checkbox<HCheckbox19_label>;
inline auto constexpr hlabel_checkbox19 = labeled_checkbox<HLabel_checkbox19>;
inline auto constexpr vcheckbox19_label = labeled_checkbox<VCheckbox19_label>;
inline auto constexpr vlabel_checkbox19 = labeled_checkbox<VLabel_checkbox19>;

// ┤x├
class Checkbox20 : public HCheckbox {
   public:
    struct Parameters {
        State initial_state = State::Unchecked;
        bool locked         = false;
    };

   public:
    Checkbox20(State initial_state = State::Unchecked, bool locked = false)
        : HCheckbox{initial_state, {L"┤x├", L"┤ ├"}, locked}
    {}

    Checkbox20(Parameters p) : Checkbox20{p.initial_state, p.locked} {}
};

inline auto constexpr checkbox20 = make_checkbox<Checkbox20>;

using HCheckbox20_label =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox20, true>;
using HLabel_checkbox20 =
    detail::Label_checkbox_impl<layout::Horizontal, Checkbox20, false>;
using VCheckbox20_label =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox20, true>;
using VLabel_checkbox20 =
    detail::Label_checkbox_impl<layout::Vertical, Checkbox20, false>;

inline auto constexpr hcheckbox20_label = labeled_checkbox<HCheckbox20_label>;
inline auto constexpr hlabel_checkbox20 = labeled_checkbox<HLabel_checkbox20>;
inline auto constexpr vcheckbox20_label = labeled_checkbox<VCheckbox20_label>;
inline auto constexpr vlabel_checkbox20 = labeled_checkbox<VLabel_checkbox20>;

}  // namespace ox

namespace ox::slot {

template <template <typename> typename Layout_t>
auto toggle(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.toggle(); }, cb);
}

template <template <typename> typename Layout_t>
auto check(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.check(); }, cb);
}

template <template <typename> typename Layout_t>
auto uncheck(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.uncheck(); }, cb);
}

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
