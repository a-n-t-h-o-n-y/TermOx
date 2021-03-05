#include <termox/terminal/terminal.hpp>

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <esc/esc.hpp>

#include <termox/common/u32_to_mb.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/detail/is_paintable.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/system/detail/find_widget_at.hpp>
#include <termox/system/event.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/detail/canvas.hpp>
#include <termox/widget/widget.hpp>

extern "C" void handle_sigint(int /* sig*/)
{
    ox::Terminal::uninitialize();
    std::_Exit(0);
}

namespace {

using Color_store = std::map<ox::Color, std::string>;

auto fg_store = Color_store{};

auto bg_store = Color_store{};

/// Return the terminal escape sequence for the given Color \p c as foreground.
/** Returns the terminal default foreground color sequence if \p c is not in the
 *  currently set palette. */
[[nodiscard]] auto get_fg_sequence(ox::Color c) -> std::string
{
    if (auto const iter = fg_store.find(c); iter != std::cend(fg_store))
        return iter->second;
    else
        return esc::escape(foreground(esc::Default_color{}));
}

/// Return the terminal escape sequence for the given Color \p c as background.
/** Returns the terminal default background color sequence if \p c is not in the
 *  currently set palette. */
[[nodiscard]] auto get_bg_sequence(ox::Color c) -> std::string
{
    if (auto const iter = bg_store.find(c); iter != std::cend(bg_store))
        return iter->second;
    else
        return esc::escape(background(esc::Default_color{}));
}

/// Convert a Canvas::Diff into a terminal escape sequence.
[[nodiscard]] auto to_escape_sequence(ox::detail::Canvas::Diff const& diff)
    -> std::string
{
    auto sequence = std::string{};
    for (auto [point, glyph] : diff) {
        using esc::escape;
        sequence.append(escape(esc::Cursor_position{point}));
        if (::esc::traits() != glyph.brush.traits)
            sequence.append(escape(glyph.brush.traits));
        sequence.append(get_fg_sequence(glyph.brush.foreground));
        sequence.append(get_bg_sequence(glyph.brush.background));
        sequence.append(ox::u32_to_mb(glyph.symbol));
    }
    return sequence;
}

/// Used as the return type for color_sequences() functions.
struct Color_sequences {
    std::string fg, bg;
};

/// Return the terminal escape sequence to set the fg/bg to Color_index \p x.
[[nodiscard]] auto color_sequences(ox::Color_index x) -> Color_sequences
{
    return {esc::escape(foreground(x)), esc::escape(background(x))};
}

/// Return the terminal escape sequence to set the fg/bg to True_color \p x.
[[nodiscard]] auto color_sequences(ox::True_color x) -> Color_sequences
{
    return {esc::escape(foreground(x)), esc::escape(background(x))};
}

/// Return the terminal escape sequence to set the fg/bg to \p x.
/** This calls on x.get_value() to get the initial color value to set. */
[[nodiscard]] auto color_sequences(ox::Dynamic_color x) -> Color_sequences
{
    return color_sequences(x.get_value());
}

/// Turn an esc:: mouse event into a pair of Receiver and local Mouse object.
template <typename T>
[[nodiscard]] auto mouse_event_info(T& event)
    -> std::pair<ox::Widget&, ox::Mouse&>
{
    auto& receiver = [&]() -> ox::Widget& {
        auto* widg = ox::detail::find_widget_at(event.state.at);
        assert(widg != nullptr);
        return *widg;
    }();
    // Change to local Point.
    event.state.at = ox::Point{event.state.at.x - receiver.inner_x(),
                               event.state.at.y - receiver.inner_y()};
    return {receiver, event.state};
}

/// Tranform Mouse_press events to ox::Events.
[[nodiscard]] auto transform(::esc::Mouse_press x) -> ox::Event
{
    auto [receiver, mouse] = mouse_event_info(x);
    return ox::Mouse_press_event{receiver, mouse};
}

/// Tranform Mouse_release events to ox::Events.
[[nodiscard]] auto transform(::esc::Mouse_release x) -> ox::Event
{
    auto [receiver, mouse] = mouse_event_info(x);
    return ox::Mouse_release_event{receiver, mouse};
}

/// Tranform Scroll_wheel events to ox::Events.
[[nodiscard]] auto transform(::esc::Scroll_wheel x) -> ox::Event
{
    auto [receiver, mouse] = mouse_event_info(x);
    return ox::Mouse_wheel_event{receiver, mouse};
}

/// Tranform Mouse_move events to ox::Events.
[[nodiscard]] auto transform(::esc::Mouse_move x) -> ox::Event
{
    auto [receiver, mouse] = mouse_event_info(x);
    return ox::Mouse_move_event{receiver, mouse};
}

/// Tranform Key_press events to ox::Events.
[[nodiscard]] auto transform(::esc::Key_press x) -> ox::Event
{
    auto receiver = []() -> decltype(ox::Key_press_event::receiver) {
        if (ox::Widget* const fw = ox::System::focus_widget(); fw == nullptr)
            return std::nullopt;
        else
            return *fw;
    }();
    return ox::Key_press_event{receiver, x.key};
}

/// Tranform Window_resize events to ox::Events.
[[nodiscard]] auto transform(::esc::Window_resize x) -> ox::Event { return x; }

}  // namespace

namespace ox {

void Terminal::initialize(Mouse_mode mouse_mode, Signals signals)
{
    if (is_initialized_)
        return;
    ::esc::initialize_interactive_terminal(mouse_mode, signals);
    std::signal(SIGINT, &handle_sigint);
    Terminal::set_palette(dawn_bringer16::palette);
    screen_buffers.resize(Terminal::area());
    is_initialized_ = true;
}

void Terminal::uninitialize()
{
    if (!is_initialized_)
        return;
    ::esc::uninitialize_terminal();
    is_initialized_ = false;
}

void Terminal::refresh()
{
    if (full_repaint_) {
        screen_buffers.merge();
        esc::write(to_escape_sequence(screen_buffers.current_screen_as_diff()));
        full_repaint_ = false;
    }
    else
        esc::write(to_escape_sequence(screen_buffers.merge_and_diff()));
    esc::flush();
    screen_buffers.next.reset();
}

void Terminal::update_color_stores(Color c, True_color tc)
{
    fg_store[c] = esc::escape(foreground(tc));
    bg_store[c] = esc::escape(background(tc));
}

void Terminal::repaint_color(Color c)
{
    esc::write(to_escape_sequence(screen_buffers.generate_color_diff(c)));
    esc::flush();
}

void Terminal::set_palette(Palette colors)
{
    dynamic_color_engine_.clear();
    palette_ = std::move(colors);
    for (auto const& [color, color_type] : palette_) {
        auto [fg, bg] = std::visit(
            [&](auto const& x) { return color_sequences(x); }, color_type);
        fg_store[color] = fg;
        bg_store[color] = bg;
        if (std::holds_alternative<Dynamic_color>(color_type)) {
            dynamic_color_engine_.start();  // no-op if already running
            dynamic_color_engine_.register_color(
                color, std::get<Dynamic_color>(color_type));
        }
    }
    Terminal::flag_full_repaint();
    palette_changed(palette_);
}

auto Terminal::palette_append(Color_definition::Value_t value) -> Color
{
    auto pal = Terminal::current_palette();
    if (pal.empty())
        pal.push_back({Color{0}, value});
    else {
        pal.push_back(
            {Color{static_cast<Color::Value_t>(pal.back().color.value + 1)},
             value});
    }
    Terminal::set_palette(pal);
    return pal.back().color;
}

auto Terminal::read_input() -> Event
{
    return std::visit([](auto const& event) { return transform(event); },
                      ::esc::read());
}

void Terminal::flush_screen()
{
    Terminal::show_cursor(false);
    Terminal::refresh();
    // Cursor
    Widget const* const fw = System::focus_widget();
    if (fw != nullptr && detail::is_paintable(*fw)) {
        assert(is_within(fw->cursor.position(), fw->area()));
        System::set_cursor(fw->cursor, fw->inner_top_left());
    }
}

}  // namespace ox
