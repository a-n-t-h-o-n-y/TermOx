#ifndef TERMOX_WIDGET_WIDGETS_MENU_HPP
#define TERMOX_WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/selecting.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/selectable.hpp>

namespace ox {

class Menu_item : public Selectable<HLabel> {
   public:
    sl::Signal<void()> selected;

   public:
    explicit Menu_item(Glyph_string label);
};

class Menu_list
    : public Passive<layout::Selecting<layout::Vertical<Menu_item>>> {
    using Base_t = layout::Selecting<layout::Vertical<Menu_item>>;

   public:
    Menu_list();

   public:
    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&;

    /// Insert item at \p index into the Menu_list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index)
        -> sl::Signal<void()>&;

    /// Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index);

   protected:
    auto key_press_event(Key k) -> bool override;

    // Selecting base class installs event filter on children.
    auto mouse_press_event_filter(Widget& w, Mouse const& m) -> bool override;
};

class Menu : public VPair<Menu_list, Widget> {
   public:
    Menu();

   public:
    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&;

    /// Insert item at \p index into the Menu_list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index)
        -> sl::Signal<void()>&;

    /// Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index);

   protected:
    auto focus_in_event() -> bool override;

   private:
    Menu_list& menu_ = this->first;
    Widget& buffer   = this->second;
};

/// Helper function to create a Menu instance.
[[nodiscard]] auto menu() -> std::unique_ptr<Menu>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_MENU_HPP
