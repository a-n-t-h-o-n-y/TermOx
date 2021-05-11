#include <termox/widget/widgets/button_list.hpp>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox::detail {

template <template <typename> class Layout_t>
auto Just_a_button_list<Layout_t>::add_button(std::u32string const& name)
    -> Button&
{
    auto& btn = this->template make_child<Button>(name) |
                pipe::on_press([this, name] { button_pressed(name); });
    if constexpr (layout::is_vertical_v<Layout_t<Button>>)
        btn | pipe::fixed_height(1);
    else
        btn | pipe::fixed_width(1);
    return btn;
}

template class Just_a_button_list<layout::Horizontal>;
template class Just_a_button_list<layout::Vertical>;

}  // namespace ox::detail

namespace ox {

template <template <typename> class Layout_t>
Button_list<Layout_t>::Button_list()
{
    link(scrollbar, buttons);
    buffer.install_event_filter(scrollbar);
}

template <template <typename> class Layout_t>
Button_list<Layout_t>::Button_list(Parameters p) : Button_list{}
{
    this->set_scrollbar_bg(p.scrollbar_bg);
    this->set_scrollbar_fg(p.scrollbar_fg);
}

template <template <typename> class Layout_t>
auto Button_list<Layout_t>::add_button(std::u32string const& name) -> Button&
{
    return buttons.add_button(name);
}

template <template <typename> class Layout_t>
void Button_list<Layout_t>::set_scrollbar_bg(Color c)
{
    scrollbar.middle.set_bar_bg(c);
}

template <template <typename> class Layout_t>
void Button_list<Layout_t>::set_scrollbar_fg(Color c)
{
    scrollbar.middle.set_bar_fg(c);
}

template class Button_list<layout::Horizontal>;
template class Button_list<layout::Vertical>;

template <template <typename> class Layout_t>
auto button_list(typename Button_list<Layout_t>::Parameters p)
    -> std::unique_ptr<Button_list<Layout_t>>
{
    return std::make_unique<Button_list<Layout_t>>(std::move(p));
}

template auto button_list(HButton_list::Parameters p)
    -> std::unique_ptr<HButton_list>;

template auto button_list(VButton_list::Parameters p)
    -> std::unique_ptr<VButton_list>;

auto vbutton_list(VButton_list::Parameters p) -> std::unique_ptr<VButton_list>
{
    return std::make_unique<VButton_list>(std::move(p));
}

auto hbutton_list(HButton_list::Parameters p) -> std::unique_ptr<HButton_list>
{
    return std::make_unique<HButton_list>(std::move(p));
}

}  // namespace ox
