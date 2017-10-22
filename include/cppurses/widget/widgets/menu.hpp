#ifndef WIDGET_WIDGETS_MENU_HPP
#define WIDGET_WIDGETS_MENU_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/widget/widget.hpp>

#include <signals/slot.hpp>

#include <cstddef>
#include <vector>

namespace cppurses {

struct Menu_item {
    Glyph_string title;
    sig::Slot<void()> action;
};

class Menu : public Widget {
   public:
    explicit Menu(Glyph_string title);

    void add_item(Menu_item item);
    void make_item(Glyph_string title, sig::Slot<void()> action);
    void remove_item(std::size_t index);

    void select_up(std::size_t n = 1);
    void select_down(std::size_t n = 1);
    void select_item(std::size_t index);

    std::size_t size() const;

   protected:
    bool paint_event() override;
    bool key_press_event(Key key, char symbol) override;

   private:
    Glyph_string menu_title_;
    std::vector<Menu_item> items_;
    std::size_t selected_index_{1};
};

namespace slot {

sig::Slot<void(Menu_item)> add_item(Menu& m);
sig::Slot<void()> add_item(Menu& m, const Menu_item& item);

sig::Slot<void(Glyph_string, sig::Slot<void()>)> make_item(Menu& m);
sig::Slot<void(Glyph_string)> make_item(Menu& m,
                                        const sig::Slot<void()>& action);
sig::Slot<void(sig::Slot<void()>)> make_item(Menu& m,
                                             const Glyph_string& title);
sig::Slot<void()> make_item(Menu& m,
                            const Glyph_string& title,
                            const sig::Slot<void()>& action);

sig::Slot<void(std::size_t)> remove_item(Menu& m);
sig::Slot<void()> remove_item(Menu& m, std::size_t index);

sig::Slot<void(std::size_t)> select_up(Menu& m);
sig::Slot<void()> select_up(Menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_down(Menu& m);
sig::Slot<void()> select_down(Menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_item(Menu& m);
sig::Slot<void()> select_item(Menu& m, std::size_t index);

}  // namespace slot
}  // namespace cppurses

#endif  // WIDGET_WIDGETS_MENU_HPP
