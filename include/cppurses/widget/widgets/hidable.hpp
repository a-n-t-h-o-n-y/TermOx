#ifndef CPPURSES_WIDGET_WIDGETS_HIDABLE_HPP
#define CPPURSES_WIDGET_WIDGETS_HIDABLE_HPP
#include <utility>

#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Policy to make any Widget or Layout hidable with a hide() and show() method.
template <typename Widget_t>
class Hidable : public layout::Stack<> {
   private:
    Widget& blank_ = this->Stack::make_page();

   public:
    Widget_t& w;

   public:
    /// Forward constructor arguments for the Widget_t that is being wrapped.
    /** Wrapped Widget is in visible state when constructed. Access via w. */
    template <typename... Args>
    Hidable(Args&&... args)
        : w{this->Stack::make_page<Widget_t>(std::forward<Args>(args)...)}
    {
        this->Stack::set_active_page(widget_index_);
        this->Stack::give_focus_on_change(false);
    }

   public:
    /// Display the held widget
    void show() { this->Stack::set_active_page(widget_index_); }

    /// Display nothing on the widget space.
    void hide() { this->Stack::set_active_page(blank_index_); }

    /// Return if w is hidden.
    auto is_hidden() const -> bool
    {
        return this->Stack::active_page_index() == blank_index_;
    }

   private:
    static constexpr auto blank_index_  = 0uL;
    static constexpr auto widget_index_ = 1uL;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_HIDABLE_HPP
