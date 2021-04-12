#ifndef TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
#define TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/textline.hpp>

namespace palette {

/// Holds interface to export the Palette as a C++ header file.
class Export_panel : public ox::layout::Vertical<> {
   public:
    Export_panel();

   private:
    ox::Textline& name_edit_ = make_child<ox::Textline>(U"Enter name...");

    ox::Confirm_button& export_btn_ =
        make_child<ox::Confirm_button>(U"Export Palette");
};

}  // namespace palette
#endif  // TERMOX_DEMOS_PALETTE_EXPORT_PANEL_HPP
