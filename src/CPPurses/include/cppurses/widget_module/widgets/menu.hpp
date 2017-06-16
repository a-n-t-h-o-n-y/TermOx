#ifndef WIDGET_MODULE_WIDGETS_MENU_HPP
#define WIDGET_MODULE_WIDGETS_MENU_HPP

#include "widget_module/widget.hpp"
#include "painter_module/glyph_string.hpp"
#include <utility>

namespace cppurses {

class Menu : public Widget {
   public:
    explicit Menu(Glyph_string title) : menu_title_{std::move(title)} {}

   private:
    // vector of Menu_item(title, action());

    Glyph_string menu_title_;
};

}  // namespace cppurses

#endif  // WIDGET_MODULE_WIDGETS_MENU_HPP
