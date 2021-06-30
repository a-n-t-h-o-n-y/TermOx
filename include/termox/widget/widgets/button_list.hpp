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
    auto add_button(std::u32string const& name) -> Button&;
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
                  Scrollbar<Layout_t>,
                  detail::Just_a_button_list_and_buffer<Layout_t>> {
   private:
    Scrollbar<Layout_t>& scrollbar                = this->first;
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
    explicit Button_list(Color scrollbar_bg = Color::Background,
                         Color scrollbar_fg = Color::Foreground);

    explicit Button_list(Parameters p);

   public:
    /// Returns reference to the Button added.
    /** The Brush and the 'pressed' signal are accessible to customize. */
    auto add_button(std::u32string const& name) -> Button&;

    /// Set the background color of the Scrollbar.
    void set_scrollbar_bg(Color c);

    /// Set the foreground color of the Scrollbar.
    void set_scrollbar_fg(Color c);
};

/// Helper function to create a Button_list instance.
template <template <typename> class Layout_t>
[[nodiscard]] auto button_list(
    typename Button_list<Layout_t>::Parameters p = {})
    -> std::unique_ptr<Button_list<Layout_t>>;

using VButton_list = Button_list<layout::Vertical>;
using HButton_list = Button_list<layout::Horizontal>;

/// Helper function to create a VButton_list instance.
[[nodiscard]] auto vbutton_list(VButton_list::Parameters p = {})
    -> std::unique_ptr<VButton_list>;

/// Helper function to create an HButton_list instance.
[[nodiscard]] auto hbutton_list(HButton_list::Parameters p = {})
    -> std::unique_ptr<HButton_list>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BUTTON_LIST_HPP
