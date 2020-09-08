#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#include <string>

#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>

#include "colors.hpp"
#include "make_break.hpp"

namespace gol {

/// Provides interface to input filename and to Signal on that filename.
class File_widget : public cppurses::layout::Vertical<> {
   public:
    cppurses::Line_edit& filename_box_ =
        this->make_child<cppurses::Line_edit>("Filename");

    cppurses::Confirm_button& confirm_btn_;

   public:
    sig::Signal<void(std::string const&)> file_request;

   public:
    explicit File_widget(std::string const& btn_text)
        : confirm_btn_{this->make_child<cppurses::Confirm_button>(btn_text)}
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(2);

        confirm_btn_ | on_press([this]() {
            file_request(filename_box_.contents().str());
        });

        confirm_btn_.main_btn | bg(color::Teal) | fg(color::White);
        confirm_btn_.confirm_page.confirm_btn | bg(color::Light_green) |
            fg(color::Black);
        confirm_btn_.confirm_page.exit_btn | bg(color::Teal) | fg(color::White);
    }
};

struct Files_box : cppurses::layout::Vertical<File_widget> {
   public:
    File_widget& import_btn = this->make_child("Import");
    File_widget& export_btn = this->make_child("Export");
    Widget& break_          = this->append(make_break());

   public:
    sig::Signal<void(std::string const&)>& import_request =
        import_btn.file_request;
    sig::Signal<void(std::string const&)>& export_request =
        export_btn.file_request;

   public:
    Files_box() { *this | cppurses::pipe::fixed_height(5uL); }
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
