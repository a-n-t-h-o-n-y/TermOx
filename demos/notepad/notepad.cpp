#include "notepad.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widget_slots.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

namespace {

void save_file(const std::string& filename, const std::string& output)
{
    std::ofstream ofs(filename);
    if (!ofs.fail())
        ofs << output;
}

std::string load_file(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (ifs.fail())
        return "";
    std::ostringstream oss;
    ifs >> oss.rdbuf();
    if (ifs.fail() and !ifs.eof())
        return "";
    return oss.str();
}

}  // namespace

using namespace cppurses;

namespace demos {

// Textbox and Trait/Color Selection
Text_and_traits::Text_and_traits() { this->initialize(); }

void Text_and_traits::initialize()
{
    textbox.brush.set_background(Color::Dark_gray);
    textbox.border.enable();
    textbox.border.segments.north_west = L'╭';
    textbox.border.segments.north_east = L'╮';
    textbox.border.segments.south_west = L'╰';
    textbox.border.segments.south_east = L'╯';

    // Signals -- Colors
    ac_select.fg_select.color_selected.connect(slot::set_foreground(textbox));
    ac_select.bg_select.color_selected.connect(slot::set_background(textbox));

    // Add Traits
    ac_select.trait_select.bold.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Bold); });
    ac_select.trait_select.italic.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Italic); });
    ac_select.trait_select.underline.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Underline); });
    ac_select.trait_select.standout.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Standout); });
    ac_select.trait_select.dim.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Dim); });
    ac_select.trait_select.inverse.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Inverse); });
    ac_select.trait_select.invisible.checked.connect(
        [this]() { textbox.insert_brush.add_traits(Trait::Invisible); });

    // Signals -- Remove Traits
    ac_select.trait_select.bold.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Bold); });
    ac_select.trait_select.italic.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Italic); });
    ac_select.trait_select.underline.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Underline); });
    ac_select.trait_select.standout.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Standout); });
    ac_select.trait_select.dim.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Dim); });
    ac_select.trait_select.inverse.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Inverse); });
    ac_select.trait_select.invisible.unchecked.connect(
        [this]() { textbox.insert_brush.remove_traits(Trait::Invisible); });
}

// Trait and Color Selections
Traits_and_colors::Traits_and_colors()
{
    this->initialize();
    fg_select.width_policy.fixed(16);
    fg_select.height_policy.fixed(2);
    fg_label.brush.add_traits(Trait::Bold);
    fg_label.set_alignment(Align::Center);

    bg_select.width_policy.fixed(16);
    bg_select.height_policy.fixed(2);
    bg_label.brush.add_traits(Trait::Bold);
    bg_label.set_alignment(Align::Center);
}

void Traits_and_colors::initialize() { this->width_policy.fixed(16); }

// Save Area
Save_area::Save_area() { this->initialize(); }

void Save_area::initialize()
{
    this->height_policy.fixed(1);

    // Load Button
    load_btn.width_policy.fixed(6);
    load_btn.brush.set_background(Color::Blue);

    // Filename Text Edit
    filename_edit.disable_scrollwheel();
    filename_edit.brush.set_background(Color::White);
    filename_edit.brush.set_foreground(Color::Black);

    // Save Button
    save_btn.width_policy.fixed(6);
    save_btn.brush.set_background(Color::Blue);
}

Notepad::Notepad()
{
    this->initialize();
    this->focus_policy = Focus_policy::Strong;
}

bool Notepad::focus_in_event()
{
    System::set_focus(txt_trait.textbox);
    return true;
}

void Notepad::initialize()
{
    // Signals
    save_area.load_btn.pressed.connect([this] {
        txt_trait.textbox.set_contents(
            ::load_file(save_area.filename_edit.contents().str()));
    });

    save_area.save_btn.pressed.connect([this] {
        ::save_file(save_area.filename_edit.contents().str(),
                    txt_trait.textbox.contents().str());
    });
}
}  // namespace demos
