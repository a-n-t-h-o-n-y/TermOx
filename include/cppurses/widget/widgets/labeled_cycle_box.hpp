#ifndef CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#define CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {
struct Glyph;

/// A label on the left and a Cycle_box on the right.
class Labeled_cycle_box : public layout::Horizontal {
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
#endif  // CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
