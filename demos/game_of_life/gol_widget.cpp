#include "gol_widget.hpp"

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

#include "cell.hpp"
#include "coordinate.hpp"
#include "exporters.hpp"
#include "filetype.hpp"
#include "importers.hpp"

namespace {
/// Convert char single digit to int value.
int to_int(char c) {
    return c - '0';
}

/// Parses a string of single digit integers into a vector of integers.
std::vector<int> to_vec_int(const std::string& values) {
    std::vector<int> result;
    result.reserve(values.size());
    for (char c : values) {
        result.push_back(to_int(c));
    }
    return result;
}

}  // namespace

namespace gol {

GoL_widget::GoL_widget() {
    this->set_dead(L' ');
    this->focus_policy = cppurses::Focus_policy::Strong;
}

void GoL_widget::set_period(Period_t period) {
    period_ = period;
    this->update_period();
}

void GoL_widget::start() {
    if (!running_) {
        this->enable_animation(period_);
        running_ = true;
    }
}

void GoL_widget::stop() {
    if (running_) {
        this->disable_animation();
        running_ = false;
    }
}

void GoL_widget::step() {
    if (running_) {
        return;
    }
    engine_.next_iteration();
    this->update();
}

void GoL_widget::set_dead(const cppurses::Glyph& dead_look) {
    this->wallpaper = dead_look;
    dead_look_ = dead_look;
    this->update();
}

void GoL_widget::enable_fade(bool enabled) {
    fade_ = enabled;
    this->update();
}

void GoL_widget::set_rules(const std::string& birth,
                           const std::string& survival) {
    engine_.set_birth_rule(to_vec_int(birth));
    engine_.set_survival_rule(to_vec_int(survival));
}

void GoL_widget::clear() {
    engine_.kill_all();
    this->update();
}

void GoL_widget::toggle_grid() {
    grid_ = !grid_;
    if (grid_) {
        this->wallpaper = cppurses::Glyph{
            L'┼', cppurses::foreground(cppurses::Color::Dark_gray)};
    } else {
        this->set_dead(dead_look_);
    }
    this->update();
}

void GoL_widget::import(const std::string& filename) {
    const auto ft = get_filetype(filename);
    // std::set<Coordinate> get_as_life_1_05(filename);
    // Apply offset to each.
    // set in engine with iter import function.
    if (ft == FileType::Life_1_05) {
        import_as_life_1_05(filename, engine_);
    } else if (ft == FileType::Life_1_06) {
        import_as_life_1_06(filename, engine_);
    } else if (ft == FileType::Plaintext) {
        import_as_plaintext(filename, engine_);
    } else if (ft == FileType::RLE) {
        import_as_rle(filename, engine_);
    }
    this->update();
}

void GoL_widget::export_as(const std::string& filename) {
    const auto ext = get_extension(filename);
    if (ext == "lif") {
        export_as_life_1_05(filename, engine_);
    } else if (ext == "life") {
        export_as_life_1_06(filename, engine_);
    } else if (ext == "cell") {
        export_as_plaintext(filename, engine_);
    } else if (ext == "rle") {
        export_as_rle(filename, engine_);
    }
}

void GoL_widget::set_offset(Coordinate offset) {
    offset_ = offset;
    this->update();
}

bool GoL_widget::paint_event() {
    cppurses::Painter p{*this};
    for (const auto& coord_cell : engine_) {
        p.put(this->get_look(coord_cell.second.age),
              transform_from_engine(coord_cell.first));
    }
    return Widget::paint_event();
}

bool GoL_widget::mouse_press_event(const cppurses::Mouse_data& mouse) {
    const Coordinate engine_position = transform_from_display(mouse.local);
    if (mouse.button == cppurses::Mouse_button::Right) {
        engine_.kill(engine_position);
    } else {
        engine_.give_life(engine_position);
    }
    this->update();
    return Widget::mouse_press_event(mouse);
}

bool GoL_widget::timer_event() {
    engine_.next_iteration();
    this->update();
    return Widget::timer_event();
}

bool GoL_widget::key_press_event(const cppurses::Keyboard_data& keyboard) {
    auto new_offset = this->offset();
    if (keyboard.key == cppurses::Key::Arrow_left) {
        --new_offset.x;
    } else if (keyboard.key == cppurses::Key::Arrow_right) {
        ++new_offset.x;
    } else if (keyboard.key == cppurses::Key::Arrow_up) {
        --new_offset.y;
    } else if (keyboard.key == cppurses::Key::Arrow_down) {
        ++new_offset.y;
    }
    this->set_offset(new_offset);
    return Widget::key_press_event(keyboard);
}

void GoL_widget::update_period() {
    if (this->running_) {
        this->disable_animation();
        this->enable_animation(period_);
    }
}

cppurses::Point GoL_widget::transform_from_engine(Coordinate position) const {
    const int x{position.x + static_cast<int>(this->width() / 2) - offset_.x};
    const int y{position.y + static_cast<int>(this->height() / 2) - offset_.y};
    if (x >= 0 && x < this->width() && y >= 0 && y < this->height()) {
        return cppurses::Point{static_cast<std::size_t>(x),
                               static_cast<std::size_t>(y)};
    }
    return cppurses::Point{std::size_t(-1), std::size_t(-1)};
}

Coordinate GoL_widget::transform_from_display(cppurses::Point p) const {
    const int x{static_cast<int>(p.x) - static_cast<int>(this->width() / 2) +
                offset_.x};
    const int y{static_cast<int>(p.y) - static_cast<int>(this->height() / 2) +
                offset_.y};
    return {x, y};
}

cppurses::Glyph GoL_widget::get_look(typename Cell::Age_t age) const {
    cppurses::Glyph look{L'█'};
    if (fade_) {
        if (age == 1) {
            look = L'▓';
        } else if (age == 2) {
            look = L'▒';
        } else if (age > 2) {
            look = L'░';
        }
    }
    return look;
}

}  // namespace gol
