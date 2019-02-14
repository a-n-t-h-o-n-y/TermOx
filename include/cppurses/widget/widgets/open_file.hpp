#ifndef CPPURSES_WIDGET_WIDGETS_OPEN_FILE_HPP
#define CPPURSES_WIDGET_WIDGETS_OPEN_FILE_HPP
#include <fstream>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/widget_free_functions.hpp>
#include <cppurses/widget/widgets/fixed_width.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

template <typename CharT = char>
struct Open_file : public layout::Horizontal {
    Open_file();

    Push_button& open_btn{this->make_child<Push_button>("Open")};
    Fixed_width& seperator{this->make_child<Fixed_width>(1)};
    Line_edit& filename_edit{this->make_child<Line_edit>("Filename")};

    // Signals
    sig::Signal<void(std::basic_ifstream<CharT>&)> open_requested;

   private:
    void notify();
};

template <typename CharT>
Open_file<CharT>::Open_file() {
    this->height_policy.fixed(1);
    open_btn.width_policy.fixed(4);
    set_background(open_btn, Color::Blue);
    seperator.wallpaper = L'⏵';
    set_background(filename_edit, Color::White);
    set_foreground(filename_edit, Color::Black);
    filename_edit.set_ghost_color(Color::Dark_gray);
    open_btn.clicked.connect([this] { this->notify(); });
}

template <typename CharT>
void Open_file<CharT>::notify() {
    std::basic_ifstream<CharT> ifs{filename_edit.contents().str()};
    open_requested(ifs);
}
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_OPEN_FILE_HPP
