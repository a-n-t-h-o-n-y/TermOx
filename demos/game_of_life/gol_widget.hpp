#ifndef TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

#include "colors.hpp"
#include "game_of_life_engine.hpp"
#include "pattern.hpp"
#include "rule.hpp"

namespace gol {

class GoL_widget : public ox::Widget {
   public:
    using Period_t = std::chrono::milliseconds;

   public:
    GoL_widget()
    {
        *this | ox::pipe::strong_focus();
        this->set_rules("B3/S23");
    }

   public:
    /// Change the animation rate, in milliseconds.
    void set_period(Period_t period)
    {
        period_ = period;
        this->update_period();
    }

    /// Start animation, no-op if already running.
    void start() { this->enable_animation(period_); }

    /// Pause auto generation increment.
    void pause() { this->disable_animation(); }

    /// Progress to the next iteration of the game.
    void step()
    {
        if (this->is_animated())
            return;
        engine_.get_next_generation();
        this->update();
    }

    /// Set Hi-Resolution Mode.
    void enable_hi_res(bool enabled = true)
    {
        hi_res_ = enabled;
        if (enabled)
            this->set_offset({offset_.x / 2, offset_.y / 4});
        else
            this->set_offset({offset_.x * 2, offset_.y * 4});
        this->update();
    }

    /// Alternate between Hi-Resolution mode and Low-Resolution Mode
    void toggle_hi_res() { this->enable_hi_res(!hi_res_); }

    /// Set the birth/survivor neighbor counts.
    /** conwaylife.com/wiki/Rulestring
     *  B/S Format: B\d*\/S\d*
     *  S/B Format: \d*\/\d*
     *  Throws std::invalid_argument if string is not in proper format.
     */
    void set_rules(std::string_view rule_string)
    {
        auto const r = parse_rule_string(rule_string);
        engine_.set_rules(r);
        rule_changed(r);
    }

    /// Directly set the Game of Life rules.
    void set_rules(Rule r)
    {
        engine_.set_rules(r);
        rule_changed(r);
    }

    // TODO this and import pattern should be the same.
    /// Directly set the Pattern, rule; doesn't clear the screen before setting.
    void set_pattern(Pattern const& pattern)
    {
        engine_.import(pattern);
        rule_changed(pattern.rule);
        this->update();
    }

    /// Clear the screen and reset the pattern to be all dead cells.
    void clear()
    {
        engine_.kill_all();
        this->update();
    }

    /// Enable/disable a background grid for easier pattern creation.
    void toggle_grid()
    {
        grid_ = !grid_;
        if (grid_)
            this->set_wallpaper(L'┼' | fg(color::Teal));
        else
            this->set_wallpaper(dead_cell);
        this->update();
    }

    /// Import and reset the pattern to the file contents.
    /** File extension and headers are used to determine the filetype. Can
     * parse Life 1.05, Life 1.06, Plaintext, and RLE files. */
    void import_file(std::string const& filename);

    /// Import a pattern, set the rules, and reset the generation count.
    /** This does not clear the current alive cells. */
    void import_pattern(Pattern pattern);

    /// Export the current state as file.
    /** File extension is used to determine the filetype to export as. Can
     *  export as Life 1.05(.lif), Life 1.06(.life), Plaintext(.cell), and
     *  RLE(.rle) */
    void export_to_file(std::string const& filename);

    /// Set the engine coordinates that are at the center of the screen.
    void set_offset(Coordinate offset)
    {
        offset_ = offset;
        offset_changed(offset_);
        this->update();
    }

    /// Return the engine coordinates that are at the center of the screen.
    auto offset() const -> Coordinate { return offset_; }

    void enable_rainbow()
    {
        cell_color_ = color::Rainbow;
        this->update();
    }

    void disable_rainbow()
    {
        cell_color_ = color::White;
        this->update();
    }

    void toggle_rainbow()
    {
        if (cell_color_ == color::White)
            this->enable_rainbow();
        else
            this->disable_rainbow();
    }

   protected:
    auto paint_event() -> bool override
    {
        if (hi_res_)
            this->paint_hi_res();
        else
            this->paint_low_res();
        return Widget::paint_event();
    }

    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        auto const c = transform_to_engine(m.local);
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
        return Widget::mouse_press_event(m);
    }

    auto mouse_wheel_event(ox::Mouse const& m) -> bool override
    {
        auto const c = transform_to_engine(m.local);
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

    auto timer_event() -> bool override
    {
        engine_.get_next_generation();
        this->update();
        return Widget::timer_event();
    }

    auto key_press_event(ox::Key k) -> bool override
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

    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override
    {
        // Can't use new_size, since width takes Border into account.
        half_width_  = static_cast<int>(this->width() / 2);
        half_height_ = static_cast<int>(this->height() / 2);
        return Widget::resize_event(new_size, old_size);
    }

   private:
    Game_of_life_engine engine_;
    bool hi_res_ = true;
    bool grid_   = false;
    Period_t period_{40};
    Coordinate offset_{0, 0};
    ox::Color cell_color_ = color::White;

    int half_width_  = 0;
    int half_height_ = 0;

    static auto constexpr dead_cell = ox::Glyph{L' '};

   private:
    /// Update the period if currently running.
    void update_period()
    {
        if (this->is_animated()) {
            this->disable_animation();
            this->enable_animation(period_);
        }
    }

    /// Convert unsigned screen Point to engine Coordinate.
    /** Engine coordinate (0, 0) is at the center of the display. */
    auto transform_to_engine(ox::Point p) const -> Coordinate
    {
        auto x = static_cast<std::int16_t>(p.x - half_width_ + offset_.x);
        auto y = static_cast<std::int16_t>(p.y - half_height_ + offset_.y);
        if (hi_res_) {
            x *= 2;
            y *= 4;
        }
        return {x, y};
    }

    void paint_low_res()
    {
        auto const width  = this->width();
        auto const height = this->height();
        auto p            = ox::Painter{*this};
        for (auto x = 0uL; x < width; ++x) {
            for (auto y = 0uL; y < height; ++y) {
                auto const coords = transform_to_engine({x, y});
                if (engine_.alive_at(coords))
                    p.put(L'█' | fg(cell_color_), {x, y});
            }
        }
    }

    void paint_hi_res()
    {
        auto const width  = this->width();
        auto const height = this->height();
        auto p            = ox::Painter{*this};
        for (auto x = 0uL; x < width; ++x) {
            for (auto y = 0uL; y < height; ++y) {
                auto const braille = braille_at(ox::Point{x, y});
                if (braille != L'\0')
                    p.put(braille | fg(cell_color_), {x, y});
            }
        }
    }

    auto braille_at(ox::Point p) const -> wchar_t
    {
        return braille_at(transform_to_engine(p));
    }

    /// Find the braille representation of the Coordinates at \p p.
    auto braille_at(Coordinate c) const -> wchar_t
    {
        auto result = L'\0';
        if (engine_.alive_at(c))
            result |= L'⠁';
        if (engine_.alive_at({c.x, c.y + 1}))
            result |= L'⠂';
        if (engine_.alive_at({c.x, c.y + 2}))
            result |= L'⠄';
        if (engine_.alive_at({c.x, c.y + 3}))
            result |= L'⡀';
        if (engine_.alive_at({c.x + 1, c.y}))
            result |= L'⠈';
        if (engine_.alive_at({c.x + 1, c.y + 1}))
            result |= L'⠐';
        if (engine_.alive_at({c.x + 1, c.y + 2}))
            result |= L'⠠';
        if (engine_.alive_at({c.x + 1, c.y + 3}))
            result |= L'⢀';
        return result;
    }

    void increment_at(Coordinate c)
    {
        auto b = this->braille_at(c);
        // wrapping increment
        if (b == L'⣿')
            b = L'⠀';
        else
            ++b;
        change_state(c, b);
    }

    void decrement_at(Coordinate c)
    {
        auto b = this->braille_at(c);
        // wrapping decrement
        if (b == L'⠀')
            b = L'⣿';
        else
            --b;
        change_state(c, b);
    }

    auto has_bit_set(wchar_t subject, wchar_t bit) -> bool
    {
        return (subject - L'⠀') & (bit - L'⠀');
    }

    void change_state(Coordinate c, wchar_t bits)
    {
        if (has_bit_set(bits, L'⠁'))
            engine_.create_life(c);
        else
            engine_.kill(c);

        if (has_bit_set(bits, L'⠂'))
            engine_.create_life({c.x, c.y + 1});
        else
            engine_.kill({c.x, c.y + 1});

        if (has_bit_set(bits, L'⠄'))
            engine_.create_life({c.x, c.y + 2});
        else
            engine_.kill({c.x, c.y + 2});

        if (has_bit_set(bits, L'⡀'))
            engine_.create_life({c.x, c.y + 3});
        else
            engine_.kill({c.x, c.y + 3});

        if (has_bit_set(bits, L'⠈'))
            engine_.create_life({c.x + 1, c.y});
        else
            engine_.kill({c.x + 1, c.y});

        if (has_bit_set(bits, L'⠐'))
            engine_.create_life({c.x + 1, c.y + 1});
        else
            engine_.kill({c.x + 1, c.y + 1});

        if (has_bit_set(bits, L'⠠'))
            engine_.create_life({c.x + 1, c.y + 2});
        else
            engine_.kill({c.x + 1, c.y + 2});

        if (has_bit_set(bits, L'⢀'))
            engine_.create_life({c.x + 1, c.y + 3});
        else
            engine_.kill({c.x + 1, c.y + 3});
    }

   public:
    sl::Signal<void(Coordinate)> offset_changed;
    sl::Signal<void(Rule)> rule_changed;
    sl::Signal<void(std::uint32_t)>& generation_count_changed{
        engine_.generation_count_changed};
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_GOL_WIDGET_HPP
