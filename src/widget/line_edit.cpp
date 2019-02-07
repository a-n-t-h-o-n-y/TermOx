#include <cppurses/widget/widgets/line_edit.hpp>

#include <cctype>
#include <cstdint>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace cppurses {

Line_edit::Line_edit(Glyph_string initial_text)
    : Textbox{std::move(initial_text)} {
    this->set_ghost_color(Color::Light_gray);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    this->disable_word_wrap();
}

void Line_edit::set_validator(std::function<bool(char)> validator) {
    validator_ = std::move(validator);
}

void Line_edit::clear_on_enter(bool enable) {
    clear_on_enter_ = enable;
}

void Line_edit::invisible_input(bool enabled) {
    if (enabled) {
        this->insert_brush.add_attributes(Attribute::Invisible);
    } else {
        this->insert_brush.remove_attribute(Attribute::Invisible);
    }
    if (on_initial_) {
        return;
    }
    if (enabled) {
        Glyph_string invisible_text{this->contents()};
        invisible_text.add_attributes(Attribute::Invisible);
        this->set_contents(std::move(invisible_text));

    } else {
        Glyph_string visible_text{this->contents()};
        visible_text.remove_attribute(Attribute::Invisible);
        this->set_contents(std::move(visible_text));
    }
    this->update();
}

void Line_edit::underline(bool enabled) {
    if (enabled) {
        this->wallpaper = Glyph{L' ', Attribute::Underline};
        Glyph_string underlined_text{this->contents()};
        underlined_text.add_attributes(Attribute::Underline);
        this->set_contents(std::move(underlined_text));
        this->insert_brush.add_attributes(Attribute::Underline);
    } else {
        this->wallpaper = L' ';
        Glyph_string non_underlined{this->contents()};
        non_underlined.remove_attribute(Attribute::Underline);
        this->set_contents(std::move(non_underlined));
        this->insert_brush.remove_attribute(Attribute::Underline);
    }
    this->update();
}

void Line_edit::set_ghost_color(Color c) {
    if (on_initial_) {
        Glyph_string ghost_text{this->contents()};
        ghost_text.add_attributes(foreground(c));
        this->set_contents(std::move(ghost_text));
        this->update();
    }
}

bool Line_edit::key_press_event(const Keyboard_data& keyboard) {
    if (keyboard.key == Key::Enter) {
        editing_finished(this->contents().str());
        if (clear_on_enter_) {
            this->clear();
        }
        if (on_initial_) {
            on_initial_ = false;
        }
        return true;
    }
    if (keyboard.key == Key::Arrow_up || keyboard.key == Key::Arrow_down) {
        return true;
    }
    // Validate
    if ((std::isprint(keyboard.symbol) || std::isspace(keyboard.symbol)) &&
        !validator_(keyboard.symbol)) {
        return true;
    }
    // First alph-num input
    if (keyboard.symbol != '\0' && on_initial_) {
        this->clear();
        on_initial_ = false;
    }
    return Textbox::key_press_event(keyboard);
}

bool Line_edit::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::ScrollUp ||
        mouse.button == Mouse_button::ScrollDown) {
        return true;
    }
    return Textbox::mouse_press_event(mouse);
}

bool Line_edit::focus_in_event() {
    if (on_initial_) {
        this->clear();
    }
    return Textbox::focus_in_event();
}

}  // namespace cppurses
