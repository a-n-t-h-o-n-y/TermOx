#ifndef WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#define WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#include <string>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {
struct Glyph;

class Labeled_cycle_box : public Horizontal_layout {
   public:
    explicit Labeled_cycle_box(Glyph_string title = "");

    void set_title(Glyph_string title);
    void set_divider(const Glyph& divider);

    Label& label{this->make_child<Label>()};
    Cycle_box& cycle_box{this->make_child<Cycle_box>()};

   private:
    void resize_label();
};

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
