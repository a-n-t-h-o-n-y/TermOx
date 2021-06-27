#ifndef TERMOX_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
#include <string>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/line_edit.hpp>

#include "colors.hpp"

namespace gol {

/// Provides interface to input filename and to Signal on that filename.
class File_widget : public ox::layout::Vertical<> {
   public:
    struct Parameters {
        std::u32string btn_text;
    };

   public:
    ox::Line_edit& filename_box_ = this->make_child<ox::Line_edit>(U"Filename");

    ox::Confirm_button& confirm_btn_;

    sl::Signal<void(std::string const&)> file_request;

   public:
    explicit File_widget(Parameters parameters)
        : confirm_btn_{
              this->make_child<ox::Confirm_button>(parameters.btn_text)}
    {
        using namespace ox;
        using namespace ox::pipe;

        *this | fixed_height(2);

        confirm_btn_ |
            on_press([this]() { file_request(filename_box_.text().str()); });

        confirm_btn_.main_btn | bg(color::Teal) | fg(color::White);
        confirm_btn_.confirm_page.confirm_btn | bg(color::Light_green) |
            fg(color::Black);
        confirm_btn_.confirm_page.exit_btn | bg(color::Teal) | fg(color::White);
    }
};

struct Files_box : ox::VTuple<File_widget, File_widget, ox::HLine> {
   public:
    File_widget& import_btn = this->get<0>();
    File_widget& export_btn = this->get<1>();
    ox::HLine& break_       = this->get<2>() | fg(color::Teal);

   public:
    sl::Signal<void(std::string const&)>& import_request =
        import_btn.file_request;

    sl::Signal<void(std::string const&)>& export_request =
        export_btn.file_request;

   public:
    Files_box()
        : ox::VTuple<File_widget, File_widget, ox::HLine>{{U"Import"},
                                                          {U"Export"},
                                                          {}}
    {
        *this | ox::pipe::fixed_height(5);
    }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_FILES_BOX_HPP
