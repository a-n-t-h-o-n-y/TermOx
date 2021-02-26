#ifndef TERMOX_WIDGET_WIDGETS_MENU_HPP
#define TERMOX_WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/selecting.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/selectable.hpp>

namespace ox {

class Menu_item : public Selectable<HLabel> {
   public:
    sl::Signal<void()> selected;

   public:
    explicit Menu_item(Glyph_string label)
    {
        // TODO Selectable should take Widget_t::Parameters if that type exists,
        // in the constructor.
        *this | pipe::fixed_height(1);
        if (!label.empty())
            *this | fg(label.front().brush.foreground);
        this->HLabel::set_text(std::move(label));
        this->HLabel::set_alignment(Align::Center);
    }
};

class Menu_list
    : public Passive<layout::Selecting<layout::Vertical<Menu_item>>> {
    using Base_t = layout::Selecting<layout::Vertical<Menu_item>>;

   public:
    Menu_list()
    {
        this->set_increment_selection_keys({Key::Arrow_down, Key::j});
        this->set_decrement_selection_keys({Key::Arrow_up, Key::k});
    }

   public:
    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&
    {
        return this->insert_item(std::move(label), this->child_count());
    }

    /// Insert item at \p index into the Menu_list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index)
        -> sl::Signal<void()>&
    {
        return this
            ->insert_child(std::make_unique<Menu_item>(std::move(label)), index)
            .selected;
    }

    /// Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index)
    {
        this->remove_and_delete_child_at(index);
    }

   protected:
    auto key_press_event(Key k) -> bool override
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

    // Selecting base class installs event filter on children.
    auto mouse_press_event_filter(Widget& w, Mouse const& m) -> bool override
    {
        auto const result = Base_t::mouse_press_event_filter(w, m);
        if (this->child_count() == 0)
            return result;

        switch (m.button) {
            case Mouse::Button::Left:
                this->selected_child().selected.emit();
                break;
            default: break;
        }
        return result;
    }
};

class Menu : public VPair<Menu_list, Widget> {
   public:
    Menu()
    {
        *this | pipe::direct_focus();
        buffer.install_event_filter(menu_);
    }

   public:
    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&
    {
        return menu_.append_item(std::move(label));
    }

    /// Insert item at \p index into the Menu_list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index)
        -> sl::Signal<void()>&
    {
        return menu_.insert_item(std::move(label), index);
    }

    /// Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index) { menu_.remove_item(index); }

   protected:
    auto focus_in_event() -> bool override
    {
        ox::System::set_focus(menu_);
        return true;
    }

   private:
    Menu_list& menu_ = this->first;
    Widget& buffer   = this->second;
};

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_MENU_HPP
