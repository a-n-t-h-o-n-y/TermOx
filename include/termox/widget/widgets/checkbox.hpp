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

// ☒
// [x]

// ☐
// [ ]

// TODO let the checkbox be a Glyph_string, don't use tile then..
// you'll need to have paint event again then.

// TODO add 'locked' state, where it is greyed out in some way and it can't be
// toggled?

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
class Checkbox : public Tile {
   public:
    enum class State : bool { Unchecked, Checked };

    struct Parameters {
        State initial_state = State::Unchecked;
        Glyph checked       = L'☒';
        Glyph unchecked     = L'☐';
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
                      Glyph checked       = L'☒',
                      Glyph unchecked     = L'☐')
        : Tile{initial_state == State::Unchecked ? unchecked : checked},
          state_{initial_state},
          checked_{checked},
          unchecked_{unchecked}
    {}

    /// Construct a new Checkbox
    explicit Checkbox(Parameters p)
        : Checkbox{p.initial_state, p.checked, p.unchecked} {};

   public:
    /// Set the state to be checked.
    void check()
    {
        if (state_ == State::Unchecked) {
            state_ = State::Checked;
            this->Tile::set(checked_);
            checked.emit();
            this->update();
        }
    }

    /// Set the state to be unchecked.
    void uncheck()
    {
        if (state_ == State::Checked) {
            state_ = State::Unchecked;
            this->Tile::set(unchecked_);
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

    /// Set the Glyph used for the Checked State.
    void set_checked_glyph(Glyph symbol)
    {
        checked_ = symbol;
        this->update();
    }

    /// Return the Glyph used for the Checked State.
    auto get_checked_glyph() const -> Glyph { return checked_; }

    /// Set the Glyph used for the Unchecked State.
    void set_unchecked_glyph(Glyph symbol)
    {
        unchecked_ = symbol;
        this->update();
    }

    /// Return the Glyph used for the Unchecked State.
    auto get_unchecked_glyph() const -> Glyph { return unchecked_; }

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override
    {
        if (m.button == Mouse::Button::Left)
            this->toggle();
        return Tile::mouse_press_event(m);
    }

   private:
    State state_;
    Glyph checked_;
    Glyph unchecked_;
};

/// Helper function to create an instance.
template <typename... Args>
auto checkbox(Args&&... args) -> std::unique_ptr<Checkbox>
{
    return std::make_unique<Checkbox>(std::forward<Args>(args)...);
}

// TODO possibly provide checkbox on either side and vertical too, via template
// parameters that you can type alias and give names to. very similar to ..
// actually you might be able to use the Label_wrapper type directly or
// something to create the types directly, but you want to wrap that too, so
// that you have a checkbox name to access, that just redirects to wrapped.
class Labeled_checkbox : public Label_right<layout::Horizontal, Checkbox> {
   private:
    using Label_t = Label_right<layout::Horizontal, Checkbox>;

   public:
    Checkbox& checkbox = Label_t::wrapped;
    HLabel& label      = Label_t::label;

   public:
    Labeled_checkbox(Glyph_string label_ = L"") : Label_t{std::move(label_)}
    {
        using namespace pipe;
        Label_t::label | on_mouse_press([&](auto) { checkbox.toggle(); });
        Label_t::padding | on_mouse_press([&](auto) { checkbox.toggle(); });
        *this | fixed_height(1);  // TODO this should be taken care of if you
                                  // use the Label wrapper type
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto labeled_checkbox(Args&&... args) -> std::unique_ptr<Labeled_checkbox>
{
    return std::make_unique<Labeled_checkbox>(std::forward<Args>(args)...);
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
