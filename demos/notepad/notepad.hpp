#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <memory>
#include <string>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/accordion.hpp>
#include <termox/widget/widgets/banner.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/color_select.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/scrollbar.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace demo {

/// Checkbox to enable/disable a Trait. Has a Signal from it.
class Trait_checkbox : public ox::HCheckbox_label {
   public:
    sl::Signal<void(ox::Trait)> trait_enabled;
    sl::Signal<void(ox::Trait)> trait_disabled;

   public:
    explicit Trait_checkbox(ox::Trait t);

   private:
    ox::Trait const t_;
};

/// Holds a Trait_checkbox for each Trait, emits Signals.
class Trait_boxes : public ox::Passive<ox::layout::Vertical<Trait_checkbox>> {
   public:
    sl::Signal<void(ox::Trait)> trait_enabled;
    sl::Signal<void(ox::Trait)> trait_disabled;

   public:
    explicit Trait_boxes(Parameters = {});
};

class Labeled_color_select : public ox::HLabel_top<ox::Color_select> {
   private:
    using Label_t = ox::HLabel_top<ox::Color_select>;

   public:
    struct Parameters {
        ox::Glyph_string label;
    };

   public:
    sl::Signal<void(ox::Color)>& color_selected = wrapped.color_selected;

   public:
    Labeled_color_select(Parameters p);

   private:
    /// Sets Widget heights based on number of colors in palette.
    void set_heights(ox::Palette const& pal);
};

class Side_pane : public ox::VTuple<Labeled_color_select,
                                    Labeled_color_select,
                                    Trait_boxes,
                                    ox::Widget> {
   public:
    Labeled_color_select& fg_select = this->get<0>();
    Labeled_color_select& bg_select = this->get<1>();
    Trait_boxes& trait_boxes        = this->get<2>();
    Widget& buffer                  = this->get<3>();

   public:
    Side_pane();
};

class Text_and_scroll : public ox::HPair<ox::VScrollbar, ox::Textbox> {
   public:
    ox::VScrollbar& scrollbar = this->first;
    ox::Textbox& textbox      = this->second;

   public:
    Text_and_scroll();
};

using Side_pane_accordion = ox::HAccordion<Side_pane, ox::Bar_position::Last>;

class Text_and_side_pane : public ox::layout::Horizontal<> {
   public:
    Text_and_scroll& text_and_scroll = this->make_child<Text_and_scroll>();
    Side_pane& side_pane =
        this->make_child<Side_pane_accordion>({U"Settings", ox::Align::Center})
            .wrapped();

   public:
    Text_and_side_pane();
};

class Filename_edit : public ox::Textbox {
   public:
    explicit Filename_edit(Parameters = {});

   protected:
    auto key_press_event(ox::Key k) -> bool override;
};

class Save_area : public ox::HTuple<ox::Button, Filename_edit, ox::Button> {
   private:
    ox::Button& load_btn         = this->get<0>();
    Filename_edit& filename_edit = this->get<1>();
    ox::Button& save_btn         = this->get<2>();

   public:
    sl::Signal<void(std::string const&)> save_request;
    sl::Signal<void(std::string const&)> load_request;

   public:
    Save_area();
};

class File_status_bar : public ox::Unscramble_banner {
   public:
    File_status_bar();

   public:
    void fail(ox::Glyph_string message);
    void success(ox::Glyph_string message);
};

class Notepad
    : public ox::VTuple<Text_and_side_pane, File_status_bar, Save_area> {
   public:
    Text_and_side_pane& txt_trait = this->get<0>();
    File_status_bar& status_bar   = this->get<1>();
    Save_area& save_area          = this->get<2>();

   public:
    Notepad();

   private:
    void initialize();
};

/// Notepad recreated with pipe operator and unique_ptr constructors.
[[nodiscard]] auto notepad() -> std::unique_ptr<ox::Widget>;

}  // namespace demo
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
