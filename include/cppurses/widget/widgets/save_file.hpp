#ifndef CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
#define CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
#include <fstream>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/fixed_width.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>

namespace cppurses {

template <typename Char_t = char>
class Save_file : public layout::Horizontal<> {
   public:
    Button& save_btn         = this->make_child<Button>("Save");
    Fixed_width& separator   = this->make_child<Fixed_width>(1);
    Line_edit& filename_edit = this->make_child<Line_edit>("Filename");

    sig::Signal<void(std::basic_ofstream<Char_t>&)> save_requested;

   public:
    Save_file()
    {
        this->height_policy.fixed(1);
        save_btn.width_policy.fixed(4);
        save_btn.brush.set_background(Color::Blue);
        separator.set_wallpaper(L'>');
        filename_edit.brush.set_background(Color::White);
        filename_edit.brush.set_foreground(Color::Black);
        filename_edit.set_ghost_color(Color::Dark_gray);
        save_btn.pressed.connect([this] { this->notify(); });
    }

   private:
    void notify()
    {
        auto ofs = std::basic_ofstream<Char_t>{filename_edit.contents().str()};
        save_requested(ofs);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
