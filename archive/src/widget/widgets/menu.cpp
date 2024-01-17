#include <termox/widget/widgets/menu.hpp>

#include <cstddef>
#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

Menu_item::Menu_item(Glyph_string label)
{
    *this | pipe::fixed_height(1);
    if (!label.empty())
        *this | fg(label.front().brush.foreground);
    this->HLabel::set_text(std::move(label));
    this->HLabel::set_alignment(Align::Center);
}

Menu_list::Menu_list()
{
    this->set_increment_selection_keys({Key::Arrow_down, Key::j});
    this->set_decrement_selection_keys({Key::Arrow_up, Key::k});
}

auto Menu_list::append_item(Glyph_string label) -> sl::Signal<void()>&
{
    return this->insert_item(std::move(label), this->child_count());
}

auto Menu_list::insert_item(Glyph_string label, std::size_t index)
    -> sl::Signal<void()>&
{
    return this
        ->insert_child(std::make_unique<Menu_item>(std::move(label)), index)
        .selected;
}

void Menu_list::remove_item(std::size_t index)
{
    this->remove_and_delete_child_at(index);
}

auto Menu_list::key_press_event(Key k) -> bool
{
    auto const result = Base_t::key_press_event(k);
    if (this->child_count() == 0)
        return result;

    switch (k) {
        case Key::Enter: this->selected_child().selected.emit(); break;
        default: break;
    }
    return result;
}

auto Menu_list::mouse_press_event_filter(Widget& w, Mouse const& m) -> bool
{
    auto const result = Base_t::mouse_press_event_filter(w, m);

    if (!this->contains_child(&w))
        return false;

    if (this->child_count() == 0)
        return result;

    if (m.button == Mouse::Button::Left)
        this->selected_child().selected.emit();

    return result;
}

Menu::Menu()
{
    *this | pipe::direct_focus() | pipe::forward_focus(menu_);
    buffer.install_event_filter(menu_);
}

auto Menu::append_item(Glyph_string label) -> sl::Signal<void()>&
{
    return menu_.append_item(std::move(label));
}

auto Menu::insert_item(Glyph_string label, std::size_t index)
    -> sl::Signal<void()>&
{
    return menu_.insert_item(std::move(label), index);
}

void Menu::remove_item(std::size_t index) { menu_.remove_item(index); }

auto menu() -> std::unique_ptr<Menu> { return std::make_unique<Menu>(); }

}  // namespace ox
