#ifndef TERMOX_SYSTEM_DETAIL_FOCUS_HPP
#define TERMOX_SYSTEM_DETAIL_FOCUS_HPP

namespace ox {
class Widget;
}  // namespace ox

namespace ox::detail {

class Focus {
   public:
    /// Return a pointer to the currently focused Widget, can return nullptr.
    [[nodiscard]] static auto focus_widget() -> ox::Widget*;

    /// Sets the focus to \p clicked if it has a Focus_policy::Mouse/Strong.
    static void mouse_press(ox::Widget& clicked);

    /// Find the next Focus_policy::Tab/Strong Widget and set focus to it.
    static auto tab_press() -> bool;

    /// Find the previous Focus_policy::Tab/Strong Widget and set focus to it.
    static auto shift_tab_press() -> bool;

    /// Sets focus to \p new_focus, via a posted Focus_in event.
    /** If \p new_focus has Focus_policy::None, calls Focus::clear(). */
    static void set(ox::Widget& new_focus);

    /// Set the focus widget to nullptr and send Focus_out_event to focus_widg.
    static void clear();

    /// Set the focus widget to nullptr and do not send a Focus_out_event.
    /** Needed to unregister the focus widget if it is being destroyed. */
    static void clear_without_posting_event();

    /// Enable Tab/Back_tab keys to change the focus Widget.
    static void enable_tab_focus();

    /// Disable Tab/Back_tab keys from changing focus Widget.
    static void disable_tab_focus();

    /// Stops a Tab or Back_tab from changing focus to the next Widget.
    static void suppress_tab();

    /// Re-enable a Tab or Back_tab to change focus to the next Widget.
    static void unsuppress_tab();

   private:
    static ox::Widget* focus_widget_;
    static bool tab_enabled_;
    static bool tab_suppressed_;
};

}  // namespace ox::detail
#endif  // TERMOX_SYSTEM_DETAIL_FOCUS_HPP
