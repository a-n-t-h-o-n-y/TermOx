#ifndef CPPURSES_SYSTEM_TREE_HPP
#define CPPURSES_SYSTEM_TREE_HPP

namespace cppurses {
class Widget;

/// Provides access to the current hierarchical Widget tree.
/** Defines the working set of Widgets that user input will post events to. */
class Tree {
   public:
    /// Returns a pointer to the head Widget.
    /** This Widget is the ancestor of every other widget that will be displayed
     *  on the screen. */
    static Widget* head() { return head_; }

    /// Sets a new head Widget for the entire system.
    /** Will disable the previous head widget and enable \p new_head, if not
     *  nullptr. Will also send Resize_event and Paint_events to \p new_head. */
    static void set_head(Widget* new_head);

   private:
    static Widget* head_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_TREE_HPP
