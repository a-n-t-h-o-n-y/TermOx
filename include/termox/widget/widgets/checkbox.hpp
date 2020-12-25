#ifndef TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
#define TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/tile.hpp>

namespace ox {

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
class Checkbox : public HLabel {
   public:
    enum class State : bool { Unchecked, Checked };

    struct Display {
        Glyph_string checked;
        Glyph_string unchecked;
    };

    struct Parameters {
        State initial_state = State::Unchecked;
        Display display     = {L"☒", L"☐"};
        bool locked         = false;
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
    explicit Checkbox(State initial_state = State::Unchecked,
                      Display display     = {L"☒", L"☐"},
                      bool locked         = false)
        : HLabel{initial_state == State::Unchecked ? display.unchecked
                                                   : display.checked,
                 Align::Left, 0, 0, Growth::Dynamic},
          state_{initial_state},
          display_{display},
          locked_{locked}
    {
        if (locked_) {
            display_.checked | Trait::Dim;
            display_.unchecked | Trait::Dim;
            this->HLabel::set_text(state_ == State::Unchecked
                                       ? display_.unchecked
                                       : display_.checked);
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
            this->HLabel::set_text(display_.checked);
            checked.emit();
            this->update();
        }
    }

    /// Set the state to be unchecked.
    void uncheck()
    {
        if (locked_)
            return;
        if (state_ == State::Checked) {
            state_ = State::Unchecked;
            this->HLabel::set_text(display_.unchecked);
            unchecked.emit();
            this->update();
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
    }

    /// Unlock the Checkbox, allowing it to be toggled.
    void unlock()
    {
        locked_ = false;
        display_.checked.remove_traits(Trait::Dim);
        display_.unchecked.remove_traits(Trait::Dim);
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
        this->update();
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
        return HLabel::mouse_press_event(m);
    }

   private:
    State state_;
    Display display_;
    bool locked_;
};

/// Helper function to create an instance.
template <typename... Args>
auto checkbox(Args&&... args) -> std::unique_ptr<Checkbox>
{
    return std::make_unique<Checkbox>(std::forward<Args>(args)...);
}

namespace detail {

/// Label, buffer and Checkbox tuple implementation.
template <template <typename> typename Layout_t, bool label_last>
class Label_checkbox_impl
    : public Label_wrapper<Layout_t, Checkbox, Layout_t, label_last> {
   private:
    using Base_t = Label_wrapper<Layout_t, Checkbox, Layout_t, label_last>;

   public:
    using Label_t = Label<Layout_t>;

   public:
    Checkbox& checkbox = Base_t::wrapped;
    Label_t& label     = Base_t::label;

   public:
    explicit Label_checkbox_impl(
        typename Label_t::Parameters label_parameters = {},
        Checkbox::Parameters checkbox_parameters      = {})
        : Base_t{std::move(label_parameters), std::move(checkbox_parameters)}
    {
        using namespace pipe;
        Base_t::label | on_mouse_press([&](auto) { checkbox.toggle(); });
        Base_t::padding | on_mouse_press([&](auto) { checkbox.toggle(); });
    }

   private:
    using Base_t::wrapped;
};

}  // namespace detail

/// Horizontal Label and then Checkbox
using HLabel_checkbox = detail::Label_checkbox_impl<layout::Horizontal, false>;

/// Horizontal Checkbox and then Label
using HCheckbox_label = detail::Label_checkbox_impl<layout::Horizontal, true>;

/// Vertical Label and then Checkbox
using VLabel_checkbox = detail::Label_checkbox_impl<layout::Vertical, false>;

/// Vertical Checkbox and then Label
using VCheckbox_label = detail::Label_checkbox_impl<layout::Vertical, true>;

/// Helper function to create an HLabel_checkbox instance.
inline auto hlabel_checkbox(
    typename HLabel_checkbox::Label_t::Parameters label_parameters = {},
    Checkbox::Parameters checkbox_parameters                       = {})
    -> std::unique_ptr<HLabel_checkbox>
{
    return std::make_unique<HLabel_checkbox>(std::move(label_parameters),
                                             std::move(checkbox_parameters));
}

/// Helper function to create an HCheckbox_label instance.
inline auto hcheckbox_label(
    typename HCheckbox_label::Label_t::Parameters label_parameters = {},
    Checkbox::Parameters checkbox_parameters                       = {})
    -> std::unique_ptr<HCheckbox_label>
{
    return std::make_unique<HCheckbox_label>(std::move(label_parameters),
                                             std::move(checkbox_parameters));
}

/// Helper function to create an VLabel_checkbox instance.
inline auto vlabel_checkbox(
    typename VLabel_checkbox::Label_t::Parameters label_parameters = {},
    Checkbox::Parameters checkbox_parameters                       = {})
    -> std::unique_ptr<VLabel_checkbox>
{
    return std::make_unique<VLabel_checkbox>(std::move(label_parameters),
                                             std::move(checkbox_parameters));
}

/// Helper function to create an VCheckbox_label instance.
inline auto vcheckbox_label(
    typename VCheckbox_label::Label_t::Parameters label_parameters = {},
    Checkbox::Parameters checkbox_parameters                       = {})
    -> std::unique_ptr<VCheckbox_label>
{
    return std::make_unique<VCheckbox_label>(std::move(label_parameters),
                                             std::move(checkbox_parameters));
}

}  // namespace ox

namespace ox::slot {

inline auto toggle(Checkbox& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.toggle(); }, cb);
}

inline auto check(Checkbox& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.check(); }, cb);
}

inline auto uncheck(Checkbox& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.uncheck(); }, cb);
}

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_CHECKBOX_HPP
