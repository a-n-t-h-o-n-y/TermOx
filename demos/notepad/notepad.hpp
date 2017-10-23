#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <cppurses/cppurses.hpp>

using namespace cppurses;
namespace demo {

struct Attribute_select : Vertical_layout {
    // Widgets
    Checkbox& bold = this->make_child<Checkbox>("Bold");
    Checkbox& italic = this->make_child<Checkbox>("Italics");
    Checkbox& underline = this->make_child<Checkbox>("Underline");
    Checkbox& standout = this->make_child<Checkbox>("Standout");
    Checkbox& dim = this->make_child<Checkbox>("Dim");
    Checkbox& inverse = this->make_child<Checkbox>("Inverse");
    Checkbox& invisible = this->make_child<Checkbox>("Invisible");
};

class Attrs_and_colors : public Vertical_layout {
   public:
    Attrs_and_colors();

    // Widgets
    Label& fg_label{this->make_child<Label>("Foreground⤵")};
    Color_select& fg_select = this->make_child<Color_select>();
    Label& bg_label{this->make_child<Label>("Background⤵")};
    Color_select& bg_select{this->make_child<Color_select>()};
    Attribute_select& attr_select{this->make_child<Attribute_select>()};

   private:
    void initialize();
};

class Text_and_attributes : public Horizontal_layout {
   public:
    Text_and_attributes();

    // Widgets
    Textbox& textbox{this->make_child<Textbox>()};
    Attrs_and_colors& ac_select{this->make_child<Attrs_and_colors>()};

   private:
    void initialize();
};

class Save_area : public Horizontal_layout {
   public:
    Save_area();

    // Widgets
    Push_button& load_btn{this->make_child<Push_button>("Load")};
    Textbox& filename_edit{this->make_child<Textbox>()};
    Push_button& save_btn{this->make_child<Push_button>("Save")};

   private:
    void initialize();
};

class Notepad : public Vertical_layout {
   public:
    Notepad();

   protected:
    bool focus_in_event() override;

   private:
    Text_and_attributes& txt_attr{this->make_child<Text_and_attributes>()};
    Save_area& save_area{this->make_child<Save_area>()};
    void initialize();
};

}  // namespace demo

#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
