#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <chrono>
#include <cmath>

#include <termox/termox.hpp>

namespace demo {

void write_file(std::string const& filename, std::string const& output);

auto read_file(std::string const& filename) -> std::string;

/// Checkbox to enable/disable a Trait. Has a Signal from it.
class Trait_checkbox : public ox::HCheckbox_label {
   public:
    sl::Signal<void(ox::Trait)> trait_enabled;
    sl::Signal<void(ox::Trait)> trait_disabled;

   public:
    Trait_checkbox(ox::Trait t) : ox::HCheckbox_label{{to_string(t)}}, t_{t}
    {
        this->padding | ox::pipe::fixed_width(2);
        this->checkbox.checked.connect([this] { this->trait_enabled(t_); });
        this->checkbox.unchecked.connect([this] { this->trait_disabled(t_); });
    }

   private:
    ox::Trait const t_;
};

/// Holds a Trait_checkbox for each Trait, emits Signals.
class Trait_boxes : public ox::Passive<ox::layout::Vertical<Trait_checkbox>> {
   public:
    sl::Signal<void(ox::Trait)> trait_enabled;
    sl::Signal<void(ox::Trait)> trait_disabled;

   public:
    Trait_boxes(Parameters = {})
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
    Labeled_color_select(Parameters parameters)
        : Label_t{std::move(parameters.label)}
    {
        using namespace ox::pipe;

        ox::Terminal::palette_changed.connect(
            [this](auto const& pal) { this->set_heights(pal); });

        this->set_heights(ox::Terminal::current_palette());
        this->label | ox::Trait::Bold | align_center();
    }

   private:
    /// Sets Widget heights based on number of colors in palette.
    void set_heights(ox::Palette const& pal)
    {
        using namespace ox::pipe;
        auto const height = std::ceil(pal.size() / 8.);
        *this | fixed_height(height + 1);
        this->wrapped | fixed_height(height);
    }
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
    Side_pane()
        : ox::VTuple<Labeled_color_select,
                     Labeled_color_select,
                     Trait_boxes,
                     ox::Widget>{{U"Foreground⤵"}, {U"Background⤵"}, {}, {}}
    {
        *this | ox::pipe::fixed_width(16);
    }
};

class Text_and_scroll : public ox::HPair<ox::VScrollbar, ox::Textbox> {
   public:
    ox::VScrollbar& scrollbar = this->first;
    ox::Textbox& textbox      = this->second;

   public:
    Text_and_scroll()
    {
        using namespace ox::pipe;
        textbox | bordered() | rounded_corners() | bg(ox::Color::Dark_gray);
        link(scrollbar, textbox);
    }
};

using Side_pane_accordion = ox::HAccordion<Side_pane, ox::Bar_position::Last>;

class Text_and_side_pane : public ox::layout::Horizontal<> {
   public:
    Text_and_scroll& text_and_scroll = this->make_child<Text_and_scroll>();
    Side_pane& side_pane =
        this->make_child<Side_pane_accordion>({U"Settings", ox::Align::Center})
            .wrapped();

   public:
    Text_and_side_pane()
    {
        using namespace ox;
        using namespace ox::pipe;

        side_pane.fg_select.color_selected.connect(
            ox::slot::set_foreground(text_and_scroll.textbox));
        side_pane.bg_select.color_selected.connect(
            ox::slot::set_background(text_and_scroll.textbox));

        side_pane.trait_boxes.trait_enabled.connect([this](Trait t) {
            text_and_scroll.textbox.insert_brush.traits.insert(t);
        });
        side_pane.trait_boxes.trait_disabled.connect([this](Trait t) {
            text_and_scroll.textbox.insert_brush.traits.remove(t);
        });
    }
};

class Filename_edit : public ox::Textbox {
   public:
    Filename_edit(Parameters = {})
    {
        using namespace ox;
        using namespace ox::pipe;
        *this | bg(Color::White) | fg(Color::Black);
        this->disable_scrollwheel();
    }

   protected:
    auto key_press_event(ox::Key k) -> bool override
    {
        if (k == ox::Key::Enter)
            return true;
        return Textbox::key_press_event(k);
    }
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
    Save_area()
        : ox::HTuple<ox::Button, Filename_edit, ox::Button>{{U"Load"},
                                                            {},
                                                            {U"Save"}}
    {
        using namespace ox::pipe;

        *this | fixed_height(1);

        load_btn | fixed_width(6) | bg(ox::Color::Blue) | on_press([this] {
            this->load_request.emit(filename_edit.contents().str());
        });

        save_btn | fixed_width(6) | bg(ox::Color::Blue) | on_press([this] {
            this->save_request.emit(filename_edit.contents().str());
        });
    }
};

class File_status_bar : public ox::Unscramble_banner {
   public:
    File_status_bar()
        : ox::Unscramble_banner{U"", std::chrono::milliseconds{40}}
    {}

   public:
    void fail(ox::Glyph_string message)
    {
        using namespace ox;
        this->set_text(std::move(message | fg(Color::Red)));
    }

    void success(ox::Glyph_string message)
    {
        using namespace ox;
        this->set_text(std::move(message | fg(Color::Light_green)));
    }
};

class Notepad
    : public ox::VTuple<Text_and_side_pane, File_status_bar, Save_area> {
   public:
    Text_and_side_pane& txt_trait = this->get<0>();
    File_status_bar& status_bar   = this->get<1>();
    Save_area& save_area          = this->get<2>();

   public:
    Notepad()
    {
        this->initialize();
        this->focus_policy = ox::Focus_policy::Strong;
    }

   protected:
    auto focus_in_event() -> bool override
    {
        ox::Terminal::set_palette(ox::dawn_bringer16::palette);
        ox::System::set_focus(txt_trait.text_and_scroll.textbox);
        return true;
    }

   private:
    void initialize();
};

// TODO Accordion side pane is only thing missing.

/// Notepad recreated with pipe operator and unique_ptr constructors.
auto inline notepad()
{
    using namespace ox;
    using namespace ox::pipe;
    // clang-format off
    auto np =
    vtuple(
        hpair(
            hpair(
                vscrollbar(),
                textbox(U"A Textbox...") | bordered() | rounded_corners()
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

    auto& sb = np->get<0>().first.first;
    auto& tb = np->get<0>().first.second;

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

    bg.color_selected.connect([&tb](Color c) { tb | ox::bg(c); });
    fg.color_selected.connect([&tb](Color c) { tb | ox::fg(c); });

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
            tb.set_contents(read_file(name));
            banner.set_text((name + " Loaded") | ox::fg(Color::Light_green));
        }
        catch (std::runtime_error const& e) {
            banner.set_text(e.what() | ox::fg(Color::Red));
        }
    });
    save.pressed.connect([&filename, &tb, &banner] {
        auto const name = filename.text().str();
        try {
            write_file(filename.text().str(), tb.contents().str());
            banner.set_text((name + " Saved") | ox::fg(Color::Light_green));
        }
        catch (std::runtime_error const& e) {
            banner.set_text(e.what() | ox::fg(Color::Red));
        }
    });
    return np;
}

}  // namespace demo
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
