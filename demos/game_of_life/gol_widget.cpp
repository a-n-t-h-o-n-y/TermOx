#include "gol_widget.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

#include <termox/termox.hpp>

#include "colors.hpp"
#include "coordinate.hpp"
#include "exporters.hpp"
#include "filetype.hpp"
#include "get_life_1_05.hpp"
#include "get_life_1_06.hpp"
#include "get_plaintext.hpp"
#include "get_rle.hpp"
#include "pattern.hpp"

namespace {
using gol::Coordinate;

auto constexpr dead_cell = ox::Glyph{U' '};

/// Applies the given \p offset to each cell Coordinate in \p cells.
template <typename Container_t>
void apply(bool hi_res, Coordinate offset, Container_t& cells)
{
    if (hi_res) {
        offset.x *= 2;
        offset.y *= 4;
    }
    std::transform(std::begin(cells), std::end(cells), std::begin(cells),
                   [offset](Coordinate cell) -> Coordinate {
                       return {cell.x + offset.x, cell.y + offset.y};
                   });
}

}  // namespace

using namespace ox;
namespace gol {

GoL_widget::GoL_widget()
{
    *this | ox::pipe::strong_focus();
    this->set_rules("B3/S23");
}

void GoL_widget::set_period(Period_t period)
{
    period_ = period;
    this->update_period();
}

void GoL_widget::start()
{
    this->enable_animation(period_);
    running_ = true;
}

void GoL_widget::pause()
{
    this->disable_animation();
    running_ = false;
}

void GoL_widget::step()
{
    if (this->is_animated())
        return;
    engine_.step_to_next_generation();
    this->update();
}

void GoL_widget::enable_hi_res(bool enabled)
{
    hi_res_ = enabled;
    if (enabled)
        this->set_offset({offset_.x / 2, offset_.y / 4});
    else
        this->set_offset({offset_.x * 2, offset_.y * 4});
    this->update();
}

void GoL_widget::toggle_hi_res() { this->enable_hi_res(!hi_res_); }

void GoL_widget::set_rules(std::string_view rule_string)
{
    try {
        auto const r = parse_rule_string(rule_string);
        engine_.set_rules(r);
        rule_changed(r);
    }
    catch (std::invalid_argument const&) {
    }
}

void GoL_widget::set_rules(Rule r)
{
    engine_.set_rules(r);
    rule_changed(r);
}

void GoL_widget::set_pattern(Pattern const& pattern)
{
    engine_.import(pattern);
    rule_changed(pattern.rule);
    this->update();
}

void GoL_widget::clear()
{
    engine_.kill_all();
    this->update();
}

void GoL_widget::toggle_grid()
{
    grid_ = !grid_;
    if (grid_)
        this->set_wallpaper(U'┼' | fg(color::Teal));
    else
        this->set_wallpaper(dead_cell);
    this->update();
}

void GoL_widget::import_file(std::string const& filename)
{
    auto pattern = Pattern{};
    switch (get_filetype(filename)) {
        case FileType::Life_1_05: pattern = get_life_1_05(filename); break;
        case FileType::Life_1_06: pattern = get_life_1_06(filename); break;
        case FileType::Plaintext: pattern = get_plaintext(filename); break;
        case FileType::RLE: pattern = get_RLE(filename); break;
        case FileType::Unknown: return;
    }
    this->import_pattern(pattern);
}

void GoL_widget::import_pattern(Pattern pattern)
{
    // This centers the cells to the current screen display
    apply(hi_res_, offset_, pattern.cells);
    this->set_pattern(pattern);
}

void GoL_widget::export_to_file(std::string const& filename)
{
    // TODO apply offset backwards, negative? but you have to do it in the
    // files.. you could just pass on a container or something?
    const auto ext = get_extension(filename);
    if (ext == "lif")
        export_as_life_1_05(filename, engine_);
    else if (ext == "life")
        export_as_life_1_06(filename, engine_);
    else if (ext == "cell")
        export_as_plaintext(filename, engine_);
    else if (ext == "rle")
        export_as_rle(filename, engine_);
}

void GoL_widget::set_offset(Coordinate offset)
{
    offset_ = offset;
    offset_changed(offset_);
    this->update();
}

auto GoL_widget::offset() const -> Coordinate { return offset_; }

void GoL_widget::enable_rainbow()
{
    cell_color_ = color::Rainbow;
    this->update();
}

void GoL_widget::disable_rainbow()
{
    cell_color_ = color::White;
    this->update();
}

void GoL_widget::toggle_rainbow()
{
    if (cell_color_ == color::White)
        this->enable_rainbow();
    else
        this->disable_rainbow();
}

auto GoL_widget::paint_event(ox::Painter& p) -> bool
{
    if (hi_res_)
        this->paint_hi_res(p);
    else
        this->paint_low_res(p);
    return Widget::paint_event(p);
}

auto GoL_widget::mouse_press_event(ox::Mouse const& m) -> bool
{
    this->handle_mouse_event(m);
    return Widget::mouse_press_event(m);
}

auto GoL_widget::mouse_move_event(ox::Mouse const& m) -> bool
{
    this->handle_mouse_event(m);
    return Widget::mouse_move_event(m);
}

auto GoL_widget::mouse_wheel_event(ox::Mouse const& m) -> bool
{
    auto const c = transform_to_engine(m.at);
    if (m.button == ox::Mouse::Button::ScrollUp) {
        if (m.modifiers.ctrl)
            this->set_offset({offset_.x, offset_.y - 1});
        else if (m.modifiers.shift)
            this->set_offset({offset_.x - 1, offset_.y});
        else if (hi_res_)
            this->increment_at(c);
        else
            engine_.create_life(c);
        this->update();
    }
    else if (m.button == ox::Mouse::Button::ScrollDown) {
        if (m.modifiers.ctrl)
            this->set_offset({offset_.x, offset_.y + 1});
        else if (m.modifiers.shift)
            this->set_offset({offset_.x + 1, offset_.y});
        else if (hi_res_)
            this->decrement_at(c);
        else
            engine_.kill(c);
        this->update();
    }
    return Widget::mouse_wheel_event(m);
}

auto GoL_widget::timer_event() -> bool
{
    engine_.step_to_next_generation();
    this->update();
    return Widget::timer_event();
}

auto GoL_widget::key_press_event(ox::Key k) -> bool
{
    auto new_offset = this->offset();
    switch (k) {
        using ox::Key;
        case Key::Arrow_left: --new_offset.x; break;
        case Key::Arrow_right: ++new_offset.x; break;
        case Key::Arrow_up: --new_offset.y; break;
        case Key::Arrow_down: ++new_offset.y; break;
        default: break;
    }
    this->set_offset(new_offset);
    return Widget::key_press_event(k);
}

auto GoL_widget::resize_event(ox::Area new_size, ox::Area old_size) -> bool
{
    half_width_  = (int)(new_size.width / 2);
    half_height_ = (int)(new_size.height / 2);
    return Widget::resize_event(new_size, old_size);
}

auto GoL_widget::enable_event() -> bool
{
    if (running_)
        this->enable_animation(period_);
    return Widget::enable_event();
}

auto GoL_widget::disable_event() -> bool
{
    this->disable_animation();
    return Widget::disable_event();
}

void GoL_widget::update_period()
{
    if (this->is_animated()) {
        this->disable_animation();
        this->enable_animation(period_);
    }
}

auto GoL_widget::transform_to_engine(ox::Point p) const -> Coordinate
{
    auto x = static_cast<std::int16_t>(p.x - half_width_ + offset_.x);
    auto y = static_cast<std::int16_t>(p.y - half_height_ + offset_.y);
    if (hi_res_) {
        x *= 2;
        y *= 4;
    }
    return {x, y};
}

void GoL_widget::paint_low_res(ox::Painter& p)
{
    auto const a = this->area();
    for (auto x = 0; x < a.width; ++x) {
        for (auto y = 0; y < a.height; ++y) {
            auto const coords = transform_to_engine({x, y});
            if (engine_.is_alive_at(coords))
                p.put(U'█' | fg(cell_color_), {x, y});
        }
    }
}

void GoL_widget::paint_hi_res(ox::Painter& p)
{
    auto const a = this->area();
    for (auto x = 0; x < a.width; ++x) {
        for (auto y = 0; y < a.height; ++y) {
            auto const braille = braille_at(ox::Point{x, y});
            if (braille != U'\0')
                p.put(braille | fg(cell_color_), {x, y});
        }
    }
}

auto GoL_widget::braille_at(ox::Point p) const -> char32_t
{
    return braille_at(transform_to_engine(p));
}

auto GoL_widget::braille_at(Coordinate c) const -> char32_t
{
    auto result = U'\0';
    if (engine_.is_alive_at(c))
        result |= U'⠁';
    if (engine_.is_alive_at({c.x, c.y + 1}))
        result |= U'⠂';
    if (engine_.is_alive_at({c.x, c.y + 2}))
        result |= U'⠄';
    if (engine_.is_alive_at({c.x, c.y + 3}))
        result |= U'⡀';
    if (engine_.is_alive_at({c.x + 1, c.y}))
        result |= U'⠈';
    if (engine_.is_alive_at({c.x + 1, c.y + 1}))
        result |= U'⠐';
    if (engine_.is_alive_at({c.x + 1, c.y + 2}))
        result |= U'⠠';
    if (engine_.is_alive_at({c.x + 1, c.y + 3}))
        result |= U'⢀';
    return result;
}

void GoL_widget::increment_at(Coordinate c)
{
    auto b = this->braille_at(c);
    // wrapping increment
    if (b == U'⣿')
        b = U'⠀';
    else
        ++b;
    change_state(c, b);
}

void GoL_widget::decrement_at(Coordinate c)
{
    auto b = this->braille_at(c);
    // wrapping decrement
    if (b == U'⠀')
        b = U'⣿';
    else
        --b;
    change_state(c, b);
}

auto GoL_widget::has_bit_set(char32_t subject, char32_t bit) -> bool
{
    return (subject - U'⠀') & (bit - U'⠀');
}

void GoL_widget::change_state(Coordinate c, char32_t bits)
{
    if (has_bit_set(bits, U'⠁'))
        engine_.create_life(c);
    else
        engine_.kill(c);

    if (has_bit_set(bits, U'⠂'))
        engine_.create_life({c.x, c.y + 1});
    else
        engine_.kill({c.x, c.y + 1});

    if (has_bit_set(bits, U'⠄'))
        engine_.create_life({c.x, c.y + 2});
    else
        engine_.kill({c.x, c.y + 2});

    if (has_bit_set(bits, U'⡀'))
        engine_.create_life({c.x, c.y + 3});
    else
        engine_.kill({c.x, c.y + 3});

    if (has_bit_set(bits, U'⠈'))
        engine_.create_life({c.x + 1, c.y});
    else
        engine_.kill({c.x + 1, c.y});

    if (has_bit_set(bits, U'⠐'))
        engine_.create_life({c.x + 1, c.y + 1});
    else
        engine_.kill({c.x + 1, c.y + 1});

    if (has_bit_set(bits, U'⠠'))
        engine_.create_life({c.x + 1, c.y + 2});
    else
        engine_.kill({c.x + 1, c.y + 2});

    if (has_bit_set(bits, U'⢀'))
        engine_.create_life({c.x + 1, c.y + 3});
    else
        engine_.kill({c.x + 1, c.y + 3});
}

void GoL_widget::handle_mouse_event(ox::Mouse const& m)
{
    auto const c = transform_to_engine(m.at);
    if (m.button == ox::Mouse::Button::Right) {
        if (hi_res_)
            this->decrement_at(c);
        else
            engine_.kill(c);
        this->update();
    }
    else if (m.button == ox::Mouse::Button::Left) {
        if (hi_res_)
            this->increment_at(c);
        else
            engine_.create_life(c);
        this->update();
    }
}

}  // namespace gol
