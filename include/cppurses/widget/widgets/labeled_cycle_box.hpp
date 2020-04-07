#ifndef CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#define CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/tile.hpp>
#include <cppurses/widget/pipe.hpp>

namespace cppurses {
struct Glyph;

/// A label on the left and a Cycle_box on the right.
class Labeled_cycle_box : public layout::Horizontal<> {
   public:
    Label& label         = this->make_child<Label>();
    Tile& tile           = this->make_child<Tile>(L'├');
    Cycle_box& cycle_box = this->make_child<Cycle_box>();

   public:
    explicit Labeled_cycle_box(Glyph_string title = "")
    {
        this->set_title(std::move(title));
        *this | pipe::fixed_height(1);
    }

    void set_title(Glyph_string title)
    {
        label.set_contents(std::move(title));
        this->resize_label();
    }

    void set_divider(Glyph divider) { tile.set(divider); }

   private:
    void resize_label()
    {
        label | pipe::fixed_width(label.contents().size() + 1);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABELED_CYCLE_BOX_HPP
