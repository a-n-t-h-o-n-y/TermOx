#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#include <string>

#include <signals/signal.hpp>

#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>

namespace gol {

/// Provides interface to input filename and to Signal on that filename.
class File_widget : public cppurses::layout::Vertical<> {
    cppurses::Line_edit& filename_box_{
        this->make_child<cppurses::Line_edit>("Filename")};
    cppurses::Confirm_button& confirm_btn_;

   public:
    File_widget(std::string const& btn_text);

    sig::Signal<void(std::string const&)> filename_given;
};

struct Files_box : cppurses::layout::Vertical<File_widget> {
    Files_box();

    File_widget& import_btn{this->make_child("Import")};
    File_widget& export_btn{this->make_child("Export")};

    sig::Signal<void(std::string const&)>& import_request{
        import_btn.filename_given};
    sig::Signal<void(std::string const&)>& export_request{
        export_btn.filename_given};
};
}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
