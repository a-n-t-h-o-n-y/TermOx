#ifndef WIDGET_WIDGETS_CYCLE_BOX_HPP
#define WIDGET_WIDGETS_CYCLE_BOX_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

#include <signals/signals.hpp>

#include <string>
#include <vector>

namespace cppurses {

class Cycle_box : public Horizontal_layout {
   public:
    Cycle_box(Glyph_string title = "");

    void set_title(Glyph_string title);

    void add_option(std::string option);
    void remove_option(const std::string& option);

    std::string current_option() const;

    void cycle();

    // Signals
    sig::Signal<void(std::string)> option_changed;

    Label& label{this->make_child<Label>()};
    Push_button& options_box{this->make_child<Push_button>()};

   private:
    std::vector<std::string> options_;

    void resize_label();
};

namespace slot {

sig::Slot<void(std::string)> add_option(Cycle_box& cb);
sig::Slot<void()> add_option(Cycle_box& cb, const std::string& option);

sig::Slot<void(const std::string&)> remove_option(Cycle_box& cb);
sig::Slot<void()> remove_option(Cycle_box& cb, const std::string& option);

sig::Slot<void()> cycle(Cycle_box& cb);

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_CYCLE_BOX_HPP
