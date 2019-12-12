#ifndef CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#define CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {
struct Glyph;

/// A label on the left and a Cycle_box on the right.
class Labeled_cycle_box : public layout::Horizontal<Label> {
   public:
    explicit Labeled_cycle_box(Glyph_string title = "")
    {
        this->set_title(std::move(title));

        label.border.enable();
        label.border.segments.disable_all();
        label.border.segments.east.enable();
        label.border.segments.east = L'├';
    }

    void set_title(Glyph_string title)
    {
        label.set_contents(std::move(title));
        this->resize_label();
    }

    void set_divider(const Glyph& divider)
    {
        label.border.segments.east = divider;
        this->update();
    }

    Label& label{this->make_child()};
    Cycle_box& cycle_box{this->make_child<Cycle_box>()};

   private:
    void resize_label()
    {
        label.width_policy.fixed(label.contents().size() + 2);
        this->update();
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
