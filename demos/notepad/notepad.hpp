#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <chrono>
#include <cmath>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget_slots.hpp>
#include <termox/widget/widgets/accordion.hpp>
#include <termox/widget/widgets/banner.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/color_select.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace demos {

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
    Trait_boxes()
    {
        // TODO update to new esc:: Traits(crossout, etc..)
        using ox::Trait;
        for (auto trait :
             {Trait::Bold, Trait::Italic, Trait::Underline, Trait::Standout,
              Trait::Dim, Trait::Inverse, Trait::Invisible, Trait::Blink}) {
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
    sl::Signal<void(ox::Color)>& color_selected = wrapped.color_selected;

   public:
    Labeled_color_select(ox::Glyph_string label) : Label_t{std::move(label)}
    {
        using namespace ox;
        using namespace ox::pipe;

        Terminal::palette_changed.connect(
            [this](auto const& pal) { this->set_heights(pal); });

        this->set_heights(Terminal::current_palette());
        this->label | Trait::Bold | align_center();
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

class Side_pane : public ox::layout::Vertical<> {
   public:
    Labeled_color_select& fg_select =
        this->make_child<Labeled_color_select>("Foreground⤵");

    Labeled_color_select& bg_select =
        this->make_child<Labeled_color_select>("Background⤵");

    Trait_boxes& trait_boxes = this->make_child<Trait_boxes>();

    Widget& buffer = this->make_child();

   public:
    Side_pane() { *this | ox::pipe::fixed_width(16); }
};

using Side_pane_accordion = ox::HAccordion<Side_pane, ox::Bar_position::Last>;

class Text_and_side_pane : public ox::layout::Horizontal<> {
   public:
    ox::Textbox& textbox = this->make_child<ox::Textbox>();
    Side_pane& side_pane =
        this->make_child<Side_pane_accordion>({U"Settings", ox::Align::Center})
            .wrapped();

   public:
    Text_and_side_pane()
    {
        using namespace ox;
        using namespace ox::pipe;

        textbox | bordered() | rounded_corners() | bg(Color::Dark_gray);

        side_pane.fg_select.color_selected.connect(
            ox::slot::set_foreground(textbox));
        side_pane.bg_select.color_selected.connect(
            ox::slot::set_background(textbox));

        side_pane.trait_boxes.trait_enabled.connect(
            [this](Trait t) { textbox.insert_brush.traits.insert(t); });
        side_pane.trait_boxes.trait_disabled.connect(
            [this](Trait t) { textbox.insert_brush.traits.remove(t); });
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
        ox::System::set_focus(txt_trait.textbox);
        return true;
    }

   private:
    void initialize();
};

}  // namespace demos
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
