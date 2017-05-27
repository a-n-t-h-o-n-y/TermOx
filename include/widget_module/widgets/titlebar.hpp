#ifndef WIDGET_MODULE_WIDGETS_TITLEBAR_HPP
#define WIDGET_MODULE_WIDGETS_TITLEBAR_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/layouts/horizontal_layout.hpp"

namespace twf {
class Push_button;

class Titlebar : public Horizontal_layout {
   public:
    explicit Titlebar(const Glyph_string& title = "");

   private:
    Push_button& title_;
    Push_button& exit_button_;
};

}  // namespace twf

#endif  // WIDGET_MODULE_WIDGETS_TITLEBAR_HPP
