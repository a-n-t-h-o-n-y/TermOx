#include "notepad.hpp"

#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <termox/termox.hpp>

namespace {

void write_file(std::string const& filename, std::string const& output)
{
    auto ofs = std::ofstream{filename};
    if (ofs.fail())
        throw std::runtime_error{"Cannot open " + filename + " for writing."};
    ofs << output;
    if (!ofs)
        throw std::runtime_error{"Write to " + filename + " failed."};
}

auto read_file(std::string const& filename) -> std::string
{
    auto ifs = std::ifstream{filename};
    if (ifs.fail())
        throw std::runtime_error{filename + " Could Not Be Opened"};
    auto oss = std::ostringstream{};
    ifs >> oss.rdbuf();
    if (ifs.fail() && !ifs.eof())
        throw std::runtime_error{"Read From " + filename + " Failed"};
    return oss.str();
}

}  // namespace

namespace demo {

using namespace ox;

Trait_checkbox::Trait_checkbox(ox::Trait t)
    : ox::HCheckbox_label{{to_string(t)}}, t_{t}
{
    this->padding | ox::pipe::fixed_width(2);
    this->checkbox.checked.connect([this] { this->trait_enabled(t_); });
    this->checkbox.unchecked.connect([this] { this->trait_disabled(t_); });
}

Trait_boxes::Trait_boxes(Parameters)
{
    using ox::Trait;
    for (auto trait :
         {Trait::Bold, Trait::Italic, Trait::Underline, Trait::Standout,
          Trait::Dim, Trait::Inverse, Trait::Invisible, Trait::Blink,
          Trait::Crossed_out, Trait::Underline}) {
        auto& w = this->make_child(static_cast<ox::Trait>(trait));
        w.trait_enabled.connect([this](auto t) { this->trait_enabled(t); });
        w.trait_disabled.connect(
            [this](ox::Trait t) { this->trait_disabled(t); });
    }
}

Labeled_color_select::Labeled_color_select(Parameters p)
    : Label_t{std::move(p.label)}
{
    using namespace ox::pipe;

    ox::Terminal::palette_changed.connect(
        [this](auto const& pal) { this->set_heights(pal); });

    this->set_heights(ox::Terminal::current_palette());
    this->label | ox::Trait::Bold | align_center();
}

void Labeled_color_select::set_heights(ox::Palette const& pal)
{
    using namespace ox::pipe;
    auto const height = std::ceil(pal.size() / 8.);
    *this | fixed_height(height + 1);
    this->wrapped | fixed_height(height);
}

Side_pane::Side_pane()
    : ox::VTuple<Labeled_color_select,
                 Labeled_color_select,
                 Trait_boxes,
                 ox::Widget>{{U"Foreground⤵"}, {U"Background⤵"}, {}, {}}
{
    *this | ox::pipe::fixed_width(16);
}

Text_and_scroll::Text_and_scroll()
    : Base_t{{}, {take_west(ox::border::rounded()), {U"Type Here..."}}}
{
    using namespace ox::pipe;
    textbox | bg(ox::Color::Dark_gray);
    border.set(border.segments() | bg(ox::Color::Dark_gray));
    link(scrollbar, textbox);
}

Text_and_side_pane::Text_and_side_pane()
{
    using namespace ox;
    using namespace ox::pipe;

    side_pane.fg_select.color_selected.connect([this](ox::Color c) {
        text_and_scroll.border | fg(c) | wrapped() | fg(c);
    });
    side_pane.bg_select.color_selected.connect([this](ox::Color c) {
        text_and_scroll.border | bg(c) | wrapped() | bg(c);
    });

    side_pane.trait_boxes.trait_enabled.connect([this](Trait t) {
        text_and_scroll.textbox.insert_brush.traits.insert(t);
    });
    side_pane.trait_boxes.trait_disabled.connect([this](Trait t) {
        text_and_scroll.textbox.insert_brush.traits.remove(t);
    });
}

Filename_edit::Filename_edit(Parameters)
{
    using namespace ox;
    using namespace ox::pipe;
    *this | bg(Color::White) | fg(Color::Black);
    this->set_scroll_speed(0);
}

auto Filename_edit::key_press_event(ox::Key k) -> bool
{
    if (k == ox::Key::Enter)
        return true;
    return Textbox::key_press_event(k);
}

Save_area::Save_area()
    : ox::HTuple<ox::Button, Filename_edit, ox::Button>{{U"Load"},
                                                        {},
                                                        {U"Save"}}
{
    using namespace ox::pipe;

    *this | fixed_height(1);

    load_btn | fixed_width(6) | bg(ox::Color::Blue) | on_press([this] {
        this->load_request.emit(filename_edit.text().str());
    });

    save_btn | fixed_width(6) | bg(ox::Color::Blue) | on_press([this] {
        this->save_request.emit(filename_edit.text().str());
    });
}

File_status_bar::File_status_bar()
    : ox::Unscramble_banner{U"", std::chrono::milliseconds{40}}
{}

void File_status_bar::fail(ox::Glyph_string message)
{
    using namespace ox;
    this->set_text(std::move(message | fg(Color::Red)));
}

void File_status_bar::success(ox::Glyph_string message)
{
    using namespace ox;
    this->set_text(std::move(message | fg(Color::Light_green)));
}

Notepad::Notepad()
{
    using namespace ox::pipe;
    this->initialize();
    *this | strong_focus() | forward_focus(txt_trait.text_and_scroll.textbox) |
        on_focus_in(
            [] { ox::Terminal::set_palette(ox::dawn_bringer16::palette); });
}

// Save Area
void Notepad::initialize()
{
    // Signals
    save_area.load_request.connect([this](std::string const& filename) {
        try {
            txt_trait.text_and_scroll.textbox.set_text(::read_file(filename));
            status_bar.success(filename + " Loaded");
        }
        catch (std::runtime_error const& e) {
            status_bar.fail(e.what());
        }
    });

    save_area.save_request.connect([this](std::string const& filename) {
        try {
            ::write_file(filename,
                         txt_trait.text_and_scroll.textbox.text().str());
            status_bar.success("Saved to " + filename);
        }
        catch (std::runtime_error const& e) {
            status_bar.fail(e.what());
        }
    });
}

// TODO Accordion side pane is only thing missing.
auto notepad() -> std::unique_ptr<Widget>
{
    using namespace ox;
    using namespace ox::pipe;
    // clang-format off
    auto np =
    vtuple(
        hpair(
            hpair(
                vscrollbar(),
                bordered(textbox(U"Type Here..."))
                    | ox::border::rounded() | bg(ox::Color::Dark_gray)
            ),
            vtuple(
                cycle_stack<Color_select>(
                    std::pair{Glyph_string{U"Background"}, color_select()},
                    std::pair{Glyph_string{U"Foreground"}, color_select()}
                ) | fixed_height(3),
                hlabel(U"- Traits -" | Trait::Bold) | align_center(),
                hcheckbox_label({U"Bold"},{}),
                hcheckbox_label({U"Italic"},{}),
                hcheckbox_label({U"Underline"},{}),
                hcheckbox_label({U"Standout"},{}),
                hcheckbox_label({U"Dim"},{}),
                hcheckbox_label({U"Inverse"},{}),
                hcheckbox_label({U"Invisible"},{}),
                hcheckbox_label({U"Blink"},{}),
                hcheckbox_label({U"Cross-out"},{}),
                hcheckbox_label({U"Dbl Underline"},{}),
                widget()
            ) | fixed_width(16)
        ),
        unscramble_banner(),
        htuple(
            button(U"Load") | bg(Color::Blue) | fixed_width(6),
            line_edit() | bg(Color::White) | fg(Color::Black),
            button(U"Save") | bg(Color::Blue) | fixed_width(6)
        ) | fixed_height(1)
    ) | strong_focus()
      | on_focus_in([] { Terminal::set_palette(dawn_bringer16::palette); });
    // clang-format on

    auto& sb  = np->get<0>().first.first;
    auto& tbb = np->get<0>().first.second;
    auto& tb  = tbb | wrapped() | bg(ox::Color::Dark_gray);

    auto& bg = np->get<0>().second.get<0>().stack.get_children()[0];
    auto& fg = np->get<0>().second.get<0>().stack.get_children()[1];

    // TODO Trait_select to complement color_select part of widget library.
    auto& bold          = np->get<0>().second.get<2>().checkbox;
    auto& italic        = np->get<0>().second.get<3>().checkbox;
    auto& underline     = np->get<0>().second.get<4>().checkbox;
    auto& standout      = np->get<0>().second.get<5>().checkbox;
    auto& dim           = np->get<0>().second.get<6>().checkbox;
    auto& inverse       = np->get<0>().second.get<7>().checkbox;
    auto& invisible     = np->get<0>().second.get<8>().checkbox;
    auto& blink         = np->get<0>().second.get<9>().checkbox;
    auto& cross_out     = np->get<0>().second.get<10>().checkbox;
    auto& dbl_underline = np->get<0>().second.get<11>().checkbox;

    auto& banner = np->get<1>();

    auto& load     = np->get<2>().get<0>();
    auto& filename = np->get<2>().get<1>();
    auto& save     = np->get<2>().get<2>();

    link(sb, tb);

    bg.color_selected.connect(
        [&](Color c) { tbb | ox::bg(c) | wrapped() | ox::bg(c); });
    fg.color_selected.connect(
        [&](Color c) { tbb | ox::fg(c) | wrapped() | ox::fg(c); });

    auto const connect = [&tb](auto& cb, Trait t) {
        cb.checked.connect([&tb, t] { tb.insert_brush | t; });
        cb.unchecked.connect([&tb, t] { tb.insert_brush | discard(t); });
    };
    connect(bold, Trait::Bold);
    connect(italic, Trait::Italic);
    connect(underline, Trait::Underline);
    connect(standout, Trait::Standout);
    connect(dim, Trait::Dim);
    connect(inverse, Trait::Inverse);
    connect(invisible, Trait::Invisible);
    connect(blink, Trait::Blink);
    connect(cross_out, Trait::Crossed_out);
    connect(dbl_underline, Trait::Double_underline);

    load.pressed.connect([&filename, &tb, &banner] {
        auto const name = filename.text().str();
        try {
            tb.set_text(::read_file(name));
            banner.set_text((name + " Loaded") | ox::fg(Color::Light_green));
        }
        catch (std::runtime_error const& e) {
            banner.set_text(e.what() | ox::fg(Color::Red));
        }
    });
    save.pressed.connect([&filename, &tb, &banner] {
        auto const name = filename.text().str();
        try {
            ::write_file(filename.text().str(), tb.text().str());
            banner.set_text((name + " Saved") | ox::fg(Color::Light_green));
        }
        catch (std::runtime_error const& e) {
            banner.set_text(e.what() | ox::fg(Color::Red));
        }
    });
    return np;
}

}  // namespace demo
