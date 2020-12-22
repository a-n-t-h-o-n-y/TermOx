#ifndef TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
#define TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/line_edit.hpp>

namespace palette {

/// Holds interface to export the Palette as a C++ header file.
class Export_panel : public ox::layout::Vertical<> {
   public:
    Export_panel();

   private:
    ox::Line_edit& name_edit_ = make_child<ox::Line_edit>(L"Enter name...");

    ox::Confirm_button& export_btn_ =
        make_child<ox::Confirm_button>(L"Export Palette");
};

}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
