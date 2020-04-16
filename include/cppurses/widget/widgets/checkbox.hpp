#ifndef CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#define CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/detail/tracks_lifetime.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
class Checkbox : public Widget {
   public:
    /// Emitted when box becomes checked.
    sig::Signal<void()> checked;

    /// Emitted when box becomes unchecked.
    sig::Signal<void()> unchecked;

    /// Emitted every time the box changes state.
    sig::Signal<void()> toggled;

   public:
    explicit Checkbox(bool is_checked = false,
                      Glyph checked   = L'☒',
                      Glyph unchecked = L'☐')
        : is_checked_{is_checked}, checked_{checked}, unchecked_{unchecked}
    {
        using namespace pipe;
        *this | fixed_height(1) | fixed_width(1);
    }

    /// Change state to be unchecked if currently checked, checked otherwise.
    void toggle()
    {
        is_checked_ = !is_checked_;
        toggled();
        is_checked_ ? checked() : unchecked();
        this->update();
    }

    /// Set the state to be checked.
    void check()
    {
        if (not is_checked_)
            this->toggle();
    }

    /// Set the state to be unchecked.
    void uncheck()
    {
        if (is_checked_)
            this->toggle();
    }

    /// Return whether Checkbox is currently checked.
    auto is_checked() const -> bool { return is_checked_; }

    /// Set the Glyph used for a checked box.
    void set_checked_glyph(Glyph const& symbol)
    {
        checked_ = symbol;
        this->update();
    }

    /// Set the Glyph used for an unchecked box.
    void set_unchecked_glyph(Glyph const& symbol)
    {
        unchecked_ = symbol;
        this->update();
    }

    /// Return the Glyph representing the checked state.
    auto get_checked_glyph() const -> Glyph const& { return checked_; }

    /// Return the Glyph representing the unchecked state.
    auto get_unchecked_glyph() const -> Glyph const& { return unchecked_; }

   protected:
    auto paint_event() -> bool override
    {
        Painter{*this}.put(this->is_checked() ? checked_ : unchecked_, 0, 0);
        return Widget::paint_event();
    }

    auto mouse_press_event(Mouse::State const& mouse) -> bool override
    {
        if (mouse.button == Mouse::Button::Left)
            this->toggle();
        return Widget::mouse_press_event(mouse);
    }

   private:
    bool is_checked_ = false;
    Glyph checked_;
    Glyph unchecked_;
};

class Labeled_checkbox : public Label_right<Checkbox> {
   public:
    Checkbox& checkbox = Label_right::wrapped;

    sig::Signal<void()>& checked   = checkbox.checked;
    sig::Signal<void()>& unchecked = checkbox.unchecked;
    sig::Signal<void()>& toggled   = checkbox.toggled;

   public:
    Labeled_checkbox(Glyph_string label_ = "") : Label_right{std::move(label_)}
    {
        using namespace pipe;
        Label_right::label | on_mouse_press([&](auto) { checkbox.toggle(); });
        Label_right::padding | on_mouse_press([&](auto) { checkbox.toggle(); });
    }
};

namespace slot {

inline auto toggle(Checkbox& cb) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb] { cb.toggle(); });
}

inline auto check(Checkbox& cb) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb] { cb.check(); });
}

inline auto uncheck(Checkbox& cb) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb] { cb.uncheck(); });
}

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_CHECKBOX_HPP
