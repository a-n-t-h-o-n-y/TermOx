#include "export_panel.hpp"

#include <cctype>
#include <fstream>
#include <string>

// #include <termox/painter/rgb.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/border.hpp>

namespace {
// auto rgb_to_str(ox::RGB const& rgb) -> std::string
// {
//     auto result = std::string{"{"};
//     result += std::to_string(rgb.red);
//     result += ", ";
//     result += std::to_string(rgb.green);
//     result += ", ";
//     result += std::to_string(rgb.blue);
//     result += "}";
//     return result;
// }

/// Overwrites name.hpp with functions returning the current palette.
void export_current_palette(std::string const& /* name */)
{
    // auto file = std::ofstream{name + ".hpp"};
    // auto pal  = ox::System::terminal.current_palette();

    // file << R"(#include <termox/painter/color.hpp>)" << '\n'
    //      << R"(#include <termox/painter/palette.hpp>)"
    //      << "\n\n"
    //      << "ox::Palette " << name << "() {" << '\n';

    // auto const prefix = std::string{"             {ox::Color::"};
    // file << "    return {{{ox::Color::Black, "
    //      << rgb_to_str(pal[0].values) << "},\n"
    //      << prefix << "Dark_red, " << rgb_to_str(pal[1].values) << "},\n"
    //      << prefix << "Dark_blue, " << rgb_to_str(pal[2].values) << "},\n"
    //      << prefix << "Dark_gray, " << rgb_to_str(pal[3].values) << "},\n"
    //      << prefix << "Brown, " << rgb_to_str(pal[4].values) << "},\n"
    //      << prefix << "Green, " << rgb_to_str(pal[5].values) << "},\n"
    //      << prefix << "Red, " << rgb_to_str(pal[6].values) << "},\n"
    //      << prefix << "Gray, " << rgb_to_str(pal[7].values) << "},\n"
    //      << prefix << "Blue, " << rgb_to_str(pal[8].values) << "},\n"
    //      << prefix << "Orange, " << rgb_to_str(pal[9].values) << "},\n"
    //      << prefix << "Light_gray, " << rgb_to_str(pal[10].values) << "},\n"
    //      << prefix << "Light_green, " << rgb_to_str(pal[11].values) << "},\n"
    //      << prefix << "Violet, " << rgb_to_str(pal[12].values) << "},\n"
    //      << prefix << "Light_blue, " << rgb_to_str(pal[13].values) << "},\n"
    //      << prefix << "Yellow, " << rgb_to_str(pal[14].values) << "},\n"
    //      << prefix << "White, " << rgb_to_str(pal[15].values) << "}}};\n"
    //      << "}\n";
}

}  // namespace

namespace palette {

Export_panel::Export_panel()
{
    using namespace ox;
    this->height_policy.maximum(3);
    this->border.enable();
    this->border.segments.disable_all();
    this->border.segments.north.enable();

    name_edit_.brush.background = Color::Dark_gray;
    name_edit_.set_ghost_color(Color::Light_gray);
    name_edit_.set_validator(
        [](char c) { return !std::iscntrl(c) or !std::isspace(c); });

    export_btn_.brush.background = Color::White;
    export_btn_.brush.foreground = Color::Black;
    export_btn_.pressed.connect(
        [this]() { export_current_palette(name_edit_.contents().str()); });
}
}  // namespace palette
