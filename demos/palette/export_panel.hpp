#ifndef CPPURSES_DEMOS_PALETTE_EXPORT_PANEL_HPP
#define CPPURSES_DEMOS_PALETTE_EXPORT_PANEL_HPP
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>

namespace palette {

/// Holds interface to export the Palette as a C++ header file.
class Export_panel : public cppurses::layout::Vertical {
    cppurses::Line_edit& name_edit_{
        this->make_child<cppurses::Line_edit>("Enter name...")};

    cppurses::Confirm_button& export_btn_{
        this->make_child<cppurses::Confirm_button>("Export Palette")};

   public:
    Export_panel();
};

}  // namespace palette
#endif  // CPPURSES_DEMOS_PALETTE_EXPORT_PANEL_HPP
