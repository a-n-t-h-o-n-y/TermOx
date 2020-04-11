#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace demos {

class Trait_select
    : public cppurses::layout::Vertical<cppurses::Labeled_checkbox> {
   public:
    cppurses::Labeled_checkbox& bold      = this->make_child("Bold");
    cppurses::Labeled_checkbox& italic    = this->make_child("Italics");
    cppurses::Labeled_checkbox& underline = this->make_child("Underline");
    cppurses::Labeled_checkbox& standout  = this->make_child("Standout");
    cppurses::Labeled_checkbox& dim       = this->make_child("Dim");
    cppurses::Labeled_checkbox& inverse   = this->make_child("Inverse");
    cppurses::Labeled_checkbox& invisible = this->make_child("Invisible");

   public:
    Trait_select()
    {
        using namespace cppurses::pipe;
        *this | children() |
            for_each([](auto& c) { c.padding | fixed_width(2); });
    }
};

class Traits_and_colors : public cppurses::layout::Vertical<> {
   public:
    Traits_and_colors();

    // Widgets
    cppurses::Label& fg_label{this->make_child<cppurses::Label>("Foreground⤵")};
    cppurses::Color_select& fg_select =
        this->make_child<cppurses::Color_select>();
    cppurses::Label& bg_label{this->make_child<cppurses::Label>("Background⤵")};
    cppurses::Color_select& bg_select{
        this->make_child<cppurses::Color_select>()};
    Trait_select& trait_select{this->make_child<Trait_select>()};

   private:
    void initialize();
};

class Text_and_traits : public cppurses::layout::Horizontal<> {
   public:
    Text_and_traits();

    // Widgets
    cppurses::Textbox& textbox{this->make_child<cppurses::Textbox>()};
    Traits_and_colors& ac_select{this->make_child<Traits_and_colors>()};

   private:
    void initialize();
};

class Save_area : public cppurses::layout::Horizontal<> {
   public:
    Save_area();

    // Widgets
    cppurses::Button& load_btn{this->make_child<cppurses::Button>("Load")};
    cppurses::Textbox& filename_edit{this->make_child<cppurses::Textbox>()};
    cppurses::Button& save_btn{this->make_child<cppurses::Button>("Save")};

   private:
    void initialize();
};

class Notepad : public cppurses::layout::Vertical<> {
   public:
    Notepad();

   protected:
    bool focus_in_event() override;

   private:
    Text_and_traits& txt_trait{this->make_child<Text_and_traits>()};
    Save_area& save_area{this->make_child<Save_area>()};
    void initialize();
};
}  // namespace demos
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
