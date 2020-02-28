#ifndef CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#define CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_mask.hpp>
#include <cppurses/painter/glyph.hpp>

namespace cppurses {
namespace layout {
template <typename Widget_t>
class Layout;
}
class Enable_event;
class Disable_event;
class Child_event;
class Move_event;
class Resize_event;
namespace detail {

/// Holds a Screen_descriptor representing the current screen state of a Widget.
/** A Widget is the owner of this object, but only the flush function can modify
 *  its state, as well as Event object that can inform flush about optimization
 *  opportunities. */
class Screen_state {
   private:
    /// Holds a description of the widget's current screen state. In global
    /// coordinates, and modified by Screen::flush() function.
    Screen_descriptor tiles;

   public:
    /// Holds flags and data structures used to optimize flushing to the screen.
    struct Optimize {
       public:
        bool just_enabled = false;
        bool moved        = false;
        bool resized      = false;
        bool child_event  = false;
        Glyph wallpaper;  // previous wallpaper
        detail::Screen_mask move_mask;
        detail::Screen_mask resize_mask;

       public:
        /// Reset all flags to initial and clear state, except for wallpaper.
        void reset()
        {
            this->just_enabled = false;
            this->moved        = false;
            this->resized      = false;
            this->child_event  = false;
            this->move_mask.clear();
            this->resize_mask.clear();
        }
    } optimize;

   private:
    friend class Screen;
    template <typename Widget_t>
    friend class cppurses::layout::Layout;
    friend class cppurses::Enable_event;
    friend class cppurses::Disable_event;
    friend class cppurses::Child_event;
    friend class cppurses::Move_event;
    friend class cppurses::Resize_event;
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_PAINTER_DETAIL_SCREEN_STATE_HPP
