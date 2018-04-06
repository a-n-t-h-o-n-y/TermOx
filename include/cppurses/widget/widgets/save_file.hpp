#ifndef CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
#define CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
#include <fstream>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/blank_width.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

template <typename CharT = char>
struct Save_file : public Horizontal_layout {
    Save_file();

    Push_button& save_btn{this->make_child<Push_button>("Save")};
    Blank_width& seperator{this->make_child<Blank_width>(1)};
    Line_edit& filename_edit{this->make_child<Line_edit>("Filename")};

    // Signals
    sig::Signal<void(std::basic_ofstream<CharT>&)> save_requested;

   private:
    void notify();
};

template <typename CharT>
Save_file<CharT>::Save_file() {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);

    save_btn.width_policy.type(Size_policy::Fixed);
    save_btn.width_policy.hint(4);
    set_background(save_btn, Color::Blue);
    seperator.background_tile = L'⏵';
    set_background(filename_edit, Color::White);
    set_foreground(filename_edit, Color::Black);
    filename_edit.set_ghost_color(Color::Dark_gray);

    save_btn.clicked.connect([this] { this->notify(); });
}

template <typename CharT>
void Save_file<CharT>::notify() {
    std::basic_ofstream<CharT> ofs{filename_edit.contents().str()};
    save_requested(ofs);
}

}  // namespace cppurses

#endif  // CPPURSES_WIDGET_WIDGETS_SAVE_FILE_HPP
