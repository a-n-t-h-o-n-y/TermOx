#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace demos {

struct Attribute_select : cppurses::Vertical_layout {
    // Widgets
    cppurses::Checkbox& bold = this->make_child<cppurses::Checkbox>("Bold");
    cppurses::Checkbox& italic =
        this->make_child<cppurses::Checkbox>("Italics");
    cppurses::Checkbox& underline =
        this->make_child<cppurses::Checkbox>("Underline");
    cppurses::Checkbox& standout =
        this->make_child<cppurses::Checkbox>("Standout");
    cppurses::Checkbox& dim = this->make_child<cppurses::Checkbox>("Dim");
    cppurses::Checkbox& inverse =
        this->make_child<cppurses::Checkbox>("Inverse");
    cppurses::Checkbox& invisible =
        this->make_child<cppurses::Checkbox>("Invisible");
};

class Attrs_and_colors : public cppurses::Vertical_layout {
   public:
    Attrs_and_colors();

    // Widgets
    cppurses::Label& fg_label{this->make_child<cppurses::Label>("Foreground⤵")};
    cppurses::Color_select& fg_select =
        this->make_child<cppurses::Color_select>();
    cppurses::Label& bg_label{this->make_child<cppurses::Label>("Background⤵")};
    cppurses::Color_select& bg_select{
        this->make_child<cppurses::Color_select>()};
    Attribute_select& attr_select{this->make_child<Attribute_select>()};

   private:
    void initialize();
};

class Text_and_attributes : public cppurses::Horizontal_layout {
   public:
    Text_and_attributes();

    // Widgets
    cppurses::Textbox& textbox{this->make_child<cppurses::Textbox>()};
    Attrs_and_colors& ac_select{this->make_child<Attrs_and_colors>()};

   private:
    void initialize();
};

class Save_area : public cppurses::Horizontal_layout {
   public:
    Save_area();

    // Widgets
    cppurses::Push_button& load_btn{
        this->make_child<cppurses::Push_button>("Load")};
    cppurses::Textbox& filename_edit{this->make_child<cppurses::Textbox>()};
    cppurses::Push_button& save_btn{
        this->make_child<cppurses::Push_button>("Save")};

   private:
    void initialize();
};

class Notepad : public cppurses::Vertical_layout {
   public:
    Notepad();

   protected:
    bool focus_in_event() override;

   private:
    Text_and_attributes& txt_attr{this->make_child<Text_and_attributes>()};
    Save_area& save_area{this->make_child<Save_area>()};
    void initialize();
};
}  // namespace demos
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
