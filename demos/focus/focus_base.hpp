#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace demos {
namespace focus {

class Focus_base : public cppurses::Horizontal_layout {
   public:
    Focus_base(cppurses::Focus_policy policy);

    bool focus_in_event() override;
    bool focus_out_event() override;
    void set_policy(cppurses::Focus_policy policy);

    // Intercept title_'s events so mouse clicks give focus to *this.
    bool focus_in_event_filter(Widget& receiver) override;

   protected:
    cppurses::Label& title_{this->make_child<cppurses::Label>()};
};

}  // namespace focus
}  // namespace demos
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
