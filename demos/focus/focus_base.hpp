#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace demos {
namespace focus {

class Focus_base : public cppurses::Widget {
   public:
    explicit Focus_base(cppurses::Focus_policy policy);

   protected:
    void set_policy(cppurses::Focus_policy policy);

    auto focus_in_event() -> bool override;

    auto focus_out_event() -> bool override;

    auto paint_event() -> bool override
    {
        cppurses::Painter{*this}.put(title_, 0, 0);
        return Widget::paint_event();
    }

   private:
    cppurses::Glyph_string title_;
};

}  // namespace focus
}  // namespace demos
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_BASE_HPP
