#ifndef TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
#define TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
#include <utility>

#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Policy to make any Widget or Layout hideable with a hide() and show() method
template <typename Widget_t>
class Hideable : public layout::Stack<> {
   private:
    Widget& blank_ = this->Stack::make_page();

   public:
    Widget_t& w;

   public:
    /// Forward constructor arguments for the Widget_t that is being wrapped.
    /** Wrapped Widget is in visible state when constructed. Access via w. */
    template <typename... Args>
    explicit Hideable(Args&&... args)
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

/// Helper function to create an instance.
template <typename Widget_t, typename... Args>
auto hideable(Args&&... args) -> std::unique_ptr<Hideable<Widget_t>>
{
    return std::make_unique<Hideable<Widget_t>>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
