#include <termox/widget/widgets/menu.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

Menu::Menu(Glyph_string title_text)
    : title{this->make_child<HLabel>(std::move(title_text))}
{
    this->focus_policy = Focus_policy::Strong;
    title.set_alignment(Align::Center);
    title.brush.add_traits(Trait::Bold);
    line_break.set_wallpaper(L'─');
}

auto Menu::insert_item(Glyph_string label, std::size_t index)
    -> sl::Signal<void()>&
{
    auto button_ptr    = std::make_unique<Button>(std::move(label));
    Button& new_button = *button_ptr;
    this->insert_child(std::move(button_ptr), index + 2uL);
    items_.emplace(std::begin(items_) + index, new_button);
    new_button.install_event_filter(*this);
    new_button.height_policy.fixed(1);

    if (items_.size() == 1)
        this->select_item(0);
    new_button.pressed.connect([this, index] {
        this->select_item(index);
        this->send_selected_signal();
    });
    return items_[index].selected;
}

void Menu::remove_item(std::size_t index)
{
    if (index >= items_.size())
        return;
    this->remove_and_delete_child(std::addressof(items_[index].button.get()));
    items_.erase(std::begin(items_) + index);
    if (index == selected_index_)
        this->select_item(0);
}

void Menu::select_item(std::size_t index)
{
    if (items_.empty())
        return;
    auto& previous_btn = items_[selected_index_].button.get();
    previous_btn.brush.remove_traits(selected_trait_);
    previous_btn.update();

    selected_index_ = index >= items_.size() ? items_.size() - 1 : index;

    auto& current_btn = items_[selected_index_].button.get();
    current_btn.brush.add_traits(selected_trait_);
    current_btn.update();
}

void Menu::set_selected_trait(Trait const& trait)
{
    auto& selected_btn = items_[selected_index_].button.get();
    selected_btn.brush.remove_traits(selected_trait_);
    selected_trait_ = trait;
    selected_btn.brush.add_traits(selected_trait_);
    selected_btn.update();
}

void Menu::enable(bool enable, bool post_child_polished_event)
{
    this->enable_and_post_events(enable, post_child_polished_event);
    line_break.enable(line_break_enabled_ and enable,
                      post_child_polished_event);
    title.enable(title_enabled_ and enable, post_child_polished_event);
    for (Menu_item& item : items_) {
        item.button.get().enable(enable, post_child_polished_event);
    }
}

auto Menu::key_press_event(Key k) -> bool
{
    switch (k) {
        case Key::Arrow_down:
        case Key::j: this->select_down(); break;
        case Key::Arrow_up:
        case Key::k: this->select_up(); break;
        case Key::Enter: this->send_selected_signal();
        default: break;
    }
    return layout::Vertical<>::key_press_event(k);
}

auto Menu::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
        case Mouse::Button::ScrollUp: this->select_up(); break;
        case Mouse::Button::ScrollDown: this->select_down(); break;
        default: break;
    }
    return layout::Vertical<>::mouse_wheel_event(m);
}

auto Menu::mouse_wheel_event_filter(Widget& /* receiver */, Mouse const& m)
    -> bool
{
    switch (m.button) {
        case Mouse::Button::ScrollUp: this->select_up(); return true;
        case Mouse::Button::ScrollDown: this->select_down(); return true;
        default: return false;
    }
}

}  // namespace ox

namespace ox::slot {

auto select_up(Menu& m) -> sl::Slot<void(std::size_t)>
{
    return link_lifetimes([&m](std::size_t n) { m.select_up(n); }, m);
}

auto select_up(Menu& m, std::size_t n) -> sl::Slot<void()>
{
    return link_lifetimes([&m, n] { m.select_up(n); }, m);
}

auto select_down(Menu& m) -> sl::Slot<void(std::size_t)>
{
    return link_lifetimes([&m](std::size_t n) { m.select_down(n); }, m);
}

auto select_down(Menu& m, std::size_t n) -> sl::Slot<void()>
{
    return link_lifetimes([&m, n] { m.select_down(n); }, m);
}

auto select_item(Menu& m) -> sl::Slot<void(std::size_t)>
{
    return link_lifetimes([&m](std::size_t index) { m.select_item(index); }, m);
}

auto select_item(Menu& m, std::size_t index) -> sl::Slot<void()>
{
    return link_lifetimes([&m, index] { m.select_item(index); }, m);
}

}  // namespace ox::slot
