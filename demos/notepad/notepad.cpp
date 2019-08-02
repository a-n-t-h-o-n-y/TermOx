#include "notepad.hpp"

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/system/focus.hpp>
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
    if (ifs.fail() && !ifs.eof())
        return "";
    return oss.str();
}

}  // namespace

using namespace cppurses;

namespace demos {

// Textbox and Attribute/Color Selection
Text_and_attributes::Text_and_attributes() { this->initialize(); }

void Text_and_attributes::initialize()
{
    textbox.brush.set_background(Color::Dark_gray);
    textbox.border.enable();
    textbox.border.segments.north_west = L'╭';
    textbox.border.segments.north_east = L'╮';
    textbox.border.segments.south_west = L'╰';
    textbox.border.segments.south_east = L'╯';

    // Signals -- Colors
    ac_select.fg_select.color_changed.connect(slot::set_foreground(textbox));
    ac_select.bg_select.color_changed.connect(slot::set_background(textbox));

    // Add Attributes
    ac_select.attr_select.bold.checked.connect(
        [this]() { textbox.insert_brush.add_attributes(Attribute::Bold); });
    ac_select.attr_select.italic.checked.connect(
        [this]() { textbox.insert_brush.add_attributes(Attribute::Italic); });
    ac_select.attr_select.underline.checked.connect([this]() {
        textbox.insert_brush.add_attributes(Attribute::Underline);
    });
    ac_select.attr_select.standout.checked.connect(
        [this]() { textbox.insert_brush.add_attributes(Attribute::Standout); });
    ac_select.attr_select.dim.checked.connect(
        [this]() { textbox.insert_brush.add_attributes(Attribute::Dim); });
    ac_select.attr_select.inverse.checked.connect(
        [this]() { textbox.insert_brush.add_attributes(Attribute::Inverse); });
    ac_select.attr_select.invisible.checked.connect([this]() {
        textbox.insert_brush.add_attributes(Attribute::Invisible);
    });

    // Signals -- Remove Attributes
    ac_select.attr_select.bold.unchecked.connect(
        [this]() { textbox.insert_brush.remove_attributes(Attribute::Bold); });
    ac_select.attr_select.italic.unchecked.connect([this]() {
        textbox.insert_brush.remove_attributes(Attribute::Italic);
    });
    ac_select.attr_select.underline.unchecked.connect([this]() {
        textbox.insert_brush.remove_attributes(Attribute::Underline);
    });
    ac_select.attr_select.standout.unchecked.connect([this]() {
        textbox.insert_brush.remove_attributes(Attribute::Standout);
    });
    ac_select.attr_select.dim.unchecked.connect(
        [this]() { textbox.insert_brush.remove_attributes(Attribute::Dim); });
    ac_select.attr_select.inverse.unchecked.connect([this]() {
        textbox.insert_brush.remove_attributes(Attribute::Inverse);
    });
    ac_select.attr_select.invisible.unchecked.connect([this]() {
        textbox.insert_brush.remove_attributes(Attribute::Invisible);
    });
}

// Attribute and Color Selections
Attrs_and_colors::Attrs_and_colors()
{
    this->initialize();
    fg_label.brush.add_attributes(Attribute::Bold);
    fg_label.set_alignment(Alignment::Center);
    bg_label.brush.add_attributes(Attribute::Bold);
    bg_label.set_alignment(Alignment::Center);
}

void Attrs_and_colors::initialize() { this->width_policy.fixed(16); }

// Save Area
Save_area::Save_area()
{
    this->initialize();
    // names
    this->set_name("Save_area");
    load_btn.set_name("Save_area - load_btn");
    filename_edit.set_name("Save_area - filename_edit");
    save_btn.set_name("Save_area - save_btn");
}

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
    this->set_name("Notepad - main demo widget");
}

bool Notepad::focus_in_event()
{
    Focus::set_focus_to(&txt_attr.textbox);
    return true;
}

void Notepad::initialize()
{
    // Signals
    save_area.load_btn.clicked.connect([this] {
        txt_attr.textbox.set_contents(
            ::load_file(save_area.filename_edit.contents().str()));
    });

    save_area.save_btn.clicked.connect([this] {
        ::save_file(save_area.filename_edit.contents().str(),
                    txt_attr.textbox.contents().str());
    });
}
}  // namespace demos
