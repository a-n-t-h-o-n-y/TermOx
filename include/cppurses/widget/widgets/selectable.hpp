#ifndef CPPURSES_WIDGET_WIDGETS_SELECTABLE_HPP
#define CPPURSES_WIDGET_WIDGETS_SELECTABLE_HPP
#include <cppurses/painter/trait.hpp>
#include <cppurses/widget/pipe.hpp>

namespace cppurses {

/// Wraps Widget_t to provide select() and unselect() methods to modify display.
template <typename Widget_t,
          typename Select_method   = void,
          typename Unselect_method = void>
class Selectable : public Widget_t {
   public:
    /// Provide two methods to modify Widget_t. Signature: void(Widget_t&);
    Selectable(Select_method s, Unselect_method u) : select_{s}, unselect_{u} {}

    /// Change visual to mark as selected.
    void select() { select_(*this); }

    /// Change visual to mark as unselected.
    void unselect() { unselect_(*this); }

   private:
    Select_method select_;
    Unselect_method unselect_;
};

/// Default overload that set's Widget_t's brush to Trait::Standout.
template <typename Widget_t>
class Selectable<Widget_t, void, void> : public Widget_t {
   public:
    /// Change visual to mark as selected.
    void select()
    {
        *this | pipe::add(Trait::Standout);
        *this | pipe::descendants() | pipe::add(Trait::Standout);
    }

    /// Change visual to mark as unselected.
    void unselect()
    {
        *this | pipe::discard(Trait::Standout);
        *this | pipe::descendants() | pipe::discard(Trait::Standout);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_SELECTABLE_HPP