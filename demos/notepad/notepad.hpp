#ifndef DEMOS_NOTEPAD_NOTEPAD_HPP
#define DEMOS_NOTEPAD_NOTEPAD_HPP
#include <chrono>
#include <cmath>

#include <signals_light/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/align.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget_slots.hpp>
#include <cppurses/widget/widgets/accordion.hpp>
#include <cppurses/widget/widgets/banner.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace demos {

/// Checkbox to enable/disable a Trait. Has a Signal from it.
class Trait_checkbox : public cppurses::Labeled_checkbox {
   public:
    sl::Signal<void(cppurses::Trait)> trait_enabled;
    sl::Signal<void(cppurses::Trait)> trait_disabled;

   public:
    Trait_checkbox(cppurses::Trait t) : Labeled_checkbox{to_string(t)}, t_{t}
    {
        this->padding | cppurses::pipe::fixed_width(2);
        this->checked.connect([this] { this->trait_enabled(t_); });
        this->unchecked.connect([this] { this->trait_disabled(t_); });
    }

   private:
    cppurses::Trait const t_;
};

/// Holds a Trait_checkbox for each Trait, emits Signals.
class Trait_boxes : public cppurses::layout::Vertical<Trait_checkbox> {
   public:
    sl::Signal<void(cppurses::Trait)> trait_enabled;
    sl::Signal<void(cppurses::Trait)> trait_disabled;

   public:
    Trait_boxes()
    {
        for (auto i = 0; i < cppurses::Trait_count; ++i) {
            auto& w = this->make_child(static_cast<cppurses::Trait>(i));
            w.trait_enabled.connect([this](auto t) { this->trait_enabled(t); });
            w.trait_disabled.connect(
                [this](cppurses::Trait t) { this->trait_disabled(t); });
        }
    }
};

class Labeled_color_select
    : public cppurses::Label_top<cppurses::layout::Horizontal<>,
                                 cppurses::Color_select> {
   public:
    sl::Signal<void(cppurses::Color)>& color_selected = wrapped.color_selected;

   public:
    Labeled_color_select(cppurses::Glyph_string label)
        : Label_top{{std::move(label)}}
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        System::terminal.palette_changed.connect(
            [this](auto const& pal) { this->set_heights(pal); });

        this->set_heights(System::terminal.current_palette());
        this->label | Trait::Bold | align_center();
    }

   private:
    /// Sets Widget heights based on number of colors in palette.
    void set_heights(cppurses::Palette const& pal)
    {
        using namespace cppurses::pipe;
        auto const height = std::ceil(pal.size() / 8.) + 1;
        *this | fixed_height(height);
        this->wrapped | fixed_height(height);
    }
};

class Side_pane : public cppurses::layout::Vertical<> {
   public:
    Labeled_color_select& fg_select =
        this->make_child<Labeled_color_select>("Foreground⤵");

    Labeled_color_select& bg_select =
        this->make_child<Labeled_color_select>("Background⤵");

    Trait_boxes& trait_boxes = this->make_child<Trait_boxes>();

   public:
    Side_pane() { *this | cppurses::pipe::fixed_width(16); }
};

using Side_pane_accordion =
    cppurses::HAccordion<Side_pane, cppurses::Bar_position::Last>;

class Text_and_side_pane : public cppurses::layout::Horizontal<> {
   public:
    cppurses::Textbox& textbox = this->make_child<cppurses::Textbox>();
    Side_pane& side_pane       = this->make_child<Side_pane_accordion>(
                                   {L"Settings", cppurses::Align::Center})
                               .wrapped();

   public:
    Text_and_side_pane()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        textbox | bordered() | rounded_corners() | bg(Color::Dark_gray);

        side_pane.fg_select.color_selected.connect(
            cppurses::slot::set_foreground(textbox));
        side_pane.bg_select.color_selected.connect(
            cppurses::slot::set_background(textbox));

        side_pane.trait_boxes.trait_enabled.connect(
            [this](Trait t) { textbox.insert_brush.add_traits(t); });
        side_pane.trait_boxes.trait_disabled.connect(
            [this](Trait t) { textbox.insert_brush.remove_traits(t); });
    }
};

class Filename_edit : public cppurses::Textbox {
   public:
    Filename_edit()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;
        *this | bg(Color::White) | fg(Color::Black);
        this->disable_scrollwheel();
    }

   protected:
    auto key_press_event(cppurses::Key k) -> bool override
    {
        if (k == cppurses::Key::Enter)
            return true;
        return Textbox::key_press_event(k);
    }
};

class Save_area : public cppurses::layout::Horizontal<> {
   public:
    sl::Signal<void(std::string)> save_request;
    sl::Signal<void(std::string)> load_request;

   public:
    Save_area()
    {
        using namespace cppurses;
        using namespace cppurses::pipe;

        *this | fixed_height(1);

        load_btn | fixed_width(6) | bg(Color::Blue) | on_press([this] {
            this->load_request(filename_edit.contents().str());
        });

        save_btn | fixed_width(6) | bg(Color::Blue) | on_press([this] {
            this->save_request(filename_edit.contents().str());
        });
    }

   private:
    cppurses::Button& load_btn   = this->make_child<cppurses::Button>("Load");
    Filename_edit& filename_edit = this->make_child<Filename_edit>();
    cppurses::Button& save_btn   = this->make_child<cppurses::Button>("Save");
};

class File_status_bar
    : public cppurses::Banner<cppurses::animator::Unscramble> {
   public:
    File_status_bar() : Banner{std::chrono::milliseconds{50}} {}

   public:
    void fail(cppurses::Glyph_string message)
    {
        using namespace cppurses;
        this->set_text(std::move(message | fg(Color::Red)));
    }

    void success(cppurses::Glyph_string message)
    {
        using namespace cppurses;
        this->set_text(std::move(message | fg(Color::Light_green)));
    }
};

class Notepad : public cppurses::layout::Vertical<> {
   public:
    Text_and_side_pane& txt_trait = this->make_child<Text_and_side_pane>();
    File_status_bar& status_bar   = this->make_child<File_status_bar>();
    Save_area& save_area          = this->make_child<Save_area>();

   public:
    Notepad()
    {
        this->initialize();
        this->focus_policy = cppurses::Focus_policy::Strong;
    }

   protected:
    auto focus_in_event() -> bool override
    {
        cppurses::System::set_focus(txt_trait.textbox);
        return true;
    }

   private:
    void initialize();
};
}  // namespace demos
#endif  // DEMOS_NOTEPAD_NOTEPAD_HPP
