#ifndef TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
#define TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
#include <utility>

#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Policy to make any Widget or Layout hideable with a hide() and show() method
template <typename Widget_t>
class Hideable : public SPair<Widget, Widget_t> {
   public:
    using Parameters = typename Widget_t::Parameter;

   public:
    Widget_t& w = this->second;

   public:
    Hideable()
    {
        this->Stack::set_active_page(widget_index_);
        this->Stack::give_focus_on_change(false);
    }

    /// Forward constructor Parameters for the Widget_t that is being wrapped.
    /** Wrapped Widget is in visible state when constructed. Access via w. */
    explicit Hideable(Parameters parameters)
        : SPair<Widget, Widget_t>{{}, parameters}
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
    [[nodiscard]] auto is_hidden() const -> bool
    {
        return this->Stack::active_page_index() == blank_index_;
    }

   private:
    static constexpr auto blank_index_  = 0uL;
    static constexpr auto widget_index_ = 1uL;
};

/// Helper function to create a Hideable instance.
template <typename Widget_t>
[[nodiscard]] auto hideable() -> std::unique_ptr<Hideable<Widget_t>>
{
    return std::make_unique<Hideable<Widget_t>>();
}

/// Helper function to create a Hideable instance.
template <typename Widget_t>
[[nodiscard]] auto hideable(typename Hideable<Widget_t>::Parameters parameters)
    -> std::unique_ptr<Hideable<Widget_t>>
{
    return std::make_unique<Hideable<Widget_t>>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_HIDEABLE_HPP
