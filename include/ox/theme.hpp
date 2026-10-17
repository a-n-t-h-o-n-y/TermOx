#pragma once
#include <chrono>
#include <ox/core/glyph.hpp>

namespace ox {
/** Shared motion policy. Input and committed values never wait for presentation. */
struct MotionPolicy {
    bool enabled = true;
    int frames_per_second = 60;
    std::chrono::milliseconds feedback{120};
    std::chrono::milliseconds selection{240};
    std::chrono::milliseconds progress{240};
};
struct Palette {
    Color background = default_color;
    Color surface = default_color;
    Color foreground = default_color;
    Color muted = colors::bright_black;
    Color accent = colors::yellow;
    Color secondary = colors::cyan;
    Color positive = colors::green;
    Color negative = colors::red;
    Color border = colors::bright_black;
    Color selection = colors::bright_black;
};
/** Application-wide semantic colors and glyph policy. Explicit widget styles win. */
struct Theme {
    Palette palette;
    MotionPolicy motion;
    bool styled = true;
    bool ascii = false;
    static auto dark() -> Theme;
    static auto light() -> Theme;
    /** Deep blue surfaces with cyan and periwinkle accents. */
    static auto ocean() -> Theme;
    /** Warm charcoal surfaces with amber and rose accents. */
    static auto ember() -> Theme;
    /** Aubergine surfaces with lavender and mint accents. */
    static auto violet() -> Theme;
    /** Ivory surfaces with dark ink and blue accents. */
    static auto paper() -> Theme;
    /** Black surfaces with bright text, yellow accents, and clear borders. */
    static auto high_contrast() -> Theme;
    static auto terminal() -> Theme;
    /** Preserve unthemed drawing and existing explicit styling. */
    static auto plain() -> Theme;
};
enum class Surface { Canvas, Raised, Muted, Accent, Positive, Negative, Border };
void validate(Theme const&);
auto theme_style(Style, Theme const&, Surface = Surface::Canvas) -> Style;
auto theme_selection(Style, StylePatch const&, Theme const&, bool focused = true)
    -> Style;
/** RGB interpolation; palette/default colors switch directly rather than guessing RGB.
 */
auto mix_color(Color const&, Color const&, double amount) -> Color;
}  // namespace ox
