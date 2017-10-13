#include "widget/widgets/line_edit.hpp"
#include "painter/color.hpp"
#include "painter/glyph_string.hpp"
#include "painter/attribute.hpp"
#include "widget/size_policy.hpp"
#include "widget/widgets/textbox.hpp"
#include "system/key.hpp"
#include "painter/glyph.hpp"
#include "system/mouse_button.hpp"

#include <cstdint>
#include <cstddef>
#include <utility>

namespace cppurses {

Line_edit::Line_edit(Glyph_string initial_text)
    : Textbox{std::move(initial_text)} {
    this->set_ghost_color(Color::Light_gray);
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);
    this->disable_word_wrap();
}

void Line_edit::set_validator(std::function<bool(char)> validator) {
    validator_ = validator;
}

void Line_edit::clear_on_enter(bool enable) {
    clear_on_enter_ = enable;
}

void Line_edit::invisible_input(bool enabled) {
    if (enabled) {
        this->add_new_text_attribute(Attribute::Invisible);
    } else {
        this->remove_new_text_attribute(Attribute::Invisible);
    }
    if (on_initial_) {
        return;
    }
    if (enabled) {
        Glyph_string invisible_text{this->contents()};
        invisible_text.add_attributes(Attribute::Invisible);
        this->set_text(std::move(invisible_text));

    } else {
        Glyph_string visible_text{this->contents()};
        visible_text.remove_attribute(Attribute::Invisible);
        this->set_text(std::move(visible_text));
    }
    this->update();
}

void Line_edit::underline(bool enabled) {
    if (enabled) {
        this->background_tile = Glyph{" ", Attribute::Underline};
        Glyph_string underlined_text{this->contents()};
        underlined_text.add_attributes(Attribute::Underline);
        this->set_text(std::move(underlined_text));
        this->add_new_text_attribute(Attribute::Underline);
    } else {
        this->background_tile = Glyph{" "};
        Glyph_string non_underlined{this->contents()};
        non_underlined.remove_attribute(Attribute::Underline);
        this->set_text(std::move(non_underlined));
        this->remove_new_text_attribute(Attribute::Underline);
    }
    this->update();
}

void Line_edit::set_ghost_color(Color c) {
    if (on_initial_) {
        Glyph_string ghost_text{this->contents()};
        ghost_text.add_attributes(foreground(c));
        this->set_text(std::move(ghost_text));
        this->update();
    }
}

bool Line_edit::key_press_event(Key key, char symbol) {
    if (key == Key::Enter) {
        if (!on_initial_) {
            editing_finished(this->contents().str());
            if (clear_on_enter_) {
                this->clear();
            }
        }
        return true;
    }
    if (key == Key::Arrow_up || key == Key::Arrow_down) {
        return true;
    }

    // Validate
    if (!validator_(symbol)) {
        return true;
    }

    // First alph-num input
    if (symbol != '\0' && on_initial_) {
        this->clear();
        on_initial_ = false;
    }
    return Textbox::key_press_event(key, symbol);
}

bool Line_edit::mouse_press_event(Mouse_button button,
                                  std::size_t global_x,
                                  std::size_t global_y,
                                  std::size_t local_x,
                                  std::size_t local_y,
                                  std::uint8_t device_id) {
    if (button == Mouse_button::ScrollUp ||
        button == Mouse_button::ScrollDown) {
        return true;
    }
    return Textbox::mouse_press_event(button, global_x, global_y, local_x,
                                      local_y, device_id);
}

bool Line_edit::focus_in_event() {
    if (on_initial_) {
        this->clear();
    }
    return Textbox::focus_in_event();
}

}  // namespace cppurses
