#ifndef CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#define CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
class Checkbox : public Widget {
   public:
    /// Emitted only when box becomes checked.
    sig::Signal<void()> checked;

    /// Emitted only when box becomes unchecked.
    sig::Signal<void()> unchecked;

    /// Emitted every time the box changes state.
    sig::Signal<void()> toggled;

   public:
    /// Construct a Checkbox with \p label and \p padding between label and box.
    explicit Checkbox(Glyph_string label = "", int padding = 3)
        : label_{std::move(label)}, padding_{padding}
    {
        this->height_policy.fixed(1);
    }

    /// Change state to be unchecked if initially checked, checked otherwise.
    void toggle()
    {
        checked_ = !checked_;
        toggled();
        checked_ ? checked() : unchecked();
        this->update();
    }

    /// Set the state to be checked.
    void check()
    {
        if (not checked_)
            this->toggle();
    }

    /// Set the state to be unchecked.
    void uncheck()
    {
        if (checked_)
            this->toggle();
    }

    /// Return whether Checkbox is currently checked.
    auto is_checked() const -> bool { return checked_; }

    /// Return the current label.
    auto label() const -> Glyph_string const& { return label_; }

    /// Set the Glyph used for a checked box.
    void set_check_look(Glyph const& symbol)
    {
        checked_box_ = symbol;
        this->update();
    }

    /// Set the Glyph used for an unchecked box.
    void set_uncheck_look(Glyph const& symbol)
    {
        unchecked_box_ = symbol;
        this->update();
    }

    /// Return the Glyph representing the checked state.
    auto checked_glyph() const -> Glyph const& { return checked_box_; }

    /// Return the Glyph representing the unchecked state.
    auto unchecked_glyph() const -> Glyph const& { return unchecked_box_; }

   protected:
    auto paint_event() -> bool override
    {
        Painter p{*this};
        if (this->is_checked())
            p.put(checked_box_, 0, 0);
        else
            p.put(unchecked_box_, 0, 0);
        p.put(label_, padding_, 0);
        return Widget::paint_event();
    }

    auto mouse_press_event(Mouse::State const& mouse) -> bool override
    {
        if (mouse.button == Mouse::Button::Left)
            this->toggle();
        return Widget::mouse_press_event(mouse);
    }

   private:
    Glyph unchecked_box_ = L'☐';
    Glyph checked_box_   = L'☒';
    bool checked_        = false;
    Glyph_string label_;
    int padding_;
};

namespace slot {

inline auto toggle(Checkbox& cb) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&cb]() { cb.toggle(); }};
    slot.track(cb.destroyed);
    return slot;
}

inline auto check(Checkbox& cb) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&cb]() { cb.check(); }};
    slot.track(cb.destroyed);
    return slot;
}

inline auto uncheck(Checkbox& cb) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&cb]() { cb.uncheck(); }};
    slot.track(cb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
