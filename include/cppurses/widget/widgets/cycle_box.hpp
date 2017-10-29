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

class Cycle_box : public Label {
   public:
    Cycle_box();

    sig::Signal<void()>& add_option(Glyph_string option);
    void remove_option(const std::string& option);
    Glyph_string current_option() const;
    void cycle_forward();
    void cycle_backward();

    // Signals
    sig::Signal<void(std::string)> option_changed;

   protected:
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

   private:
    struct Option {
        Option(Glyph_string name_);
        sig::Signal<void()> enabled;
        Glyph_string name;
    };

    std::vector<Option> options_;
};

namespace slot {

sig::Slot<void(Glyph_string)> add_option(Cycle_box& cb);
sig::Slot<void()> add_option(Cycle_box& cb, const Glyph_string& option);

sig::Slot<void(const std::string&)> remove_option(Cycle_box& cb);
sig::Slot<void()> remove_option(Cycle_box& cb, const std::string& option);

sig::Slot<void()> cycle_forward(Cycle_box& cb);
sig::Slot<void()> cycle_backward(Cycle_box& cb);

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_CYCLE_BOX_HPP
