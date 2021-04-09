#ifndef TERMOX_WIDGET_WIDGETS_BUTTON_LIST_HPP
#define TERMOX_WIDGET_WIDGETS_BUTTON_LIST_HPP
#include <memory>
#include <string>

#include <signals_light/signal.hpp>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/opposite.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/scrollbar.hpp>

namespace ox::detail {

template <template <typename> class Layout_t>
class Just_a_button_list : public Passive<Layout_t<Button>> {
   public:
    sl::Signal<void(std::u32string const& name)> button_pressed;

   public:
    auto add_button(std::u32string const& name) -> Button&
    {
        auto& btn = this->template make_child<Button>(name) |
                    pipe::on_press([this, name] { button_pressed(name); });
        if constexpr (layout::is_vertical_v<Layout_t<Button>>)
            btn | pipe::fixed_height(1);
        else
            btn | pipe::fixed_width(1);
        return btn;
    }
};

template <template <typename> class Layout_t>
struct Just_a_button_list_and_buffer
    : Pair<Layout_t<Widget>, Just_a_button_list<Layout_t>, Widget> {
    Just_a_button_list<Layout_t>& buttons = this->first;
    Widget& buffer                        = this->second;
};

}  // namespace ox::detail

namespace ox {

/// A list of buttons with connected scrollbar.
template <template <typename> class Layout_t>
class Button_list
    : public Pair<layout::Opposite_t<Layout_t<Widget>>,
                  Scrollbar<Layout_t<Widget>>,
                  detail::Just_a_button_list_and_buffer<Layout_t>> {
   private:
    Scrollbar<Layout_t<Widget>>& scrollbar        = this->first;
    detail::Just_a_button_list<Layout_t>& buttons = this->second.buttons;
    Widget& buffer                                = this->second.buffer;

   public:
    sl::Signal<void(std::u32string const& name)>& button_pressed =
        buttons.button_pressed;

    struct Parameters {
        Color scrollbar_bg = Color::Background;
        Color scrollbar_fg = Color::Foreground;
    };

   public:
    Button_list()
    {
        link(scrollbar, buttons);
        buffer.install_event_filter(scrollbar);
    }

    Button_list(Parameters p) : Button_list{}
    {
        this->set_scrollbar_bg(p.scrollbar_bg);
        this->set_scrollbar_fg(p.scrollbar_fg);
    }

   public:
    /// Returns reference to the Button added.
    /** The Brush and the 'pressed' signal are accessible to customize. */
    auto add_button(std::u32string const& name) -> Button&
    {
        return buttons.add_button(name);
    }

    /// Set the background color of the Scrollbar.
    void set_scrollbar_bg(Color c) { scrollbar.middle.set_bar_bg(c); }

    /// Set the foreground color of the Scrollbar.
    void set_scrollbar_fg(Color c) { scrollbar.middle.set_bar_fg(c); }
};

/// Helper function to create a Button_list instance.
template <template <typename> class Layout_t>
[[nodiscard]] auto button_list(
    typename Button_list<Layout_t>::Parameters parameters = {})
    -> std::unique_ptr<Button_list<Layout_t>>
{
    return std::make_unique<Button_list<Layout_t>>(std::move(parameters));
}

using VButton_list = Button_list<layout::Vertical>;
using HButton_list = Button_list<layout::Horizontal>;

/// Helper function to create a VButton_list instance.
[[nodiscard]] inline auto vbutton_list(VButton_list::Parameters parameters = {})
    -> std::unique_ptr<VButton_list>
{
    return std::make_unique<VButton_list>(std::move(parameters));
}

/// Helper function to create an HButton_list instance.
[[nodiscard]] inline auto hbutton_list(HButton_list::Parameters parameters = {})
    -> std::unique_ptr<HButton_list>
{
    return std::make_unique<HButton_list>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BUTTON_LIST_HPP
