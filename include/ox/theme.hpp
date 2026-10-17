/** @file
 * Semantic palettes, built-in themes, and shared motion policy.
 */
#pragma once
#include <chrono>
#include <ox/core/glyph.hpp>

namespace ox {
/** Shared motion policy. Input and committed values never wait for presentation. */
struct MotionPolicy {
    /** Enable presentation transitions; false settles them immediately without stopping
     * application timers.
     */
    bool enabled = true;
    /** Shared animation frame limit, from 1 through 240; idle animations schedule no frames. */
    int frames_per_second = 60;
    /** Default hover/press feedback duration; nonnegative, with zero settling immediately. */
    std::chrono::milliseconds feedback{120};
    /** Default selection-indicator transition duration; nonnegative. */
    std::chrono::milliseconds selection{240};
    /** Default progress transition duration; nonnegative. */
    std::chrono::milliseconds progress{240};
};
/** Semantic colors used by styled widgets; explicit widget colors take precedence. */
struct Palette {
    /** Page and canvas background. */
    Color background = default_color;
    /** Raised controls, menus, and popup backgrounds. */
    Color surface = default_color;
    /** Ordinary readable text on canvas and raised surfaces. */
    Color foreground = default_color;
    /** Secondary explanatory text and subdued indicators. */
    Color muted = colors::bright_black;
    /** Active controls, focus marks, and primary actions. */
    Color accent = colors::yellow;
    /** Additional visual emphasis distinct from the primary accent. */
    Color secondary = colors::cyan;
    /** Successful or positive status information. */
    Color positive = colors::green;
    /** Errors and negative status information. */
    Color negative = colors::red;
    /** Rules, outlines, and inactive tracks. */
    Color border = colors::bright_black;
    /** Selected rows and active-control background feedback. */
    Color selection = colors::bright_black;
};
/** Application-wide semantic colors and glyph policy. Explicit widget styles win. */
struct Theme {
    /** Semantic color roles resolved by theme_style(). */
    Palette palette;
    /** Shared presentation timing; input and committed model values remain immediate. */
    MotionPolicy motion;
    /** Enable semantic widget styling and themed geometry; plain() disables it. */
    bool styled = true;
    /** Use ASCII decorations; application text and pixel graphics are not transliterated. */
    bool ascii = false;
    /** Graphite-green surfaces with copper and teal accents; motion enabled and Unicode
     * decorations.
     */
    static auto dark() -> Theme;
    /** Warm light surfaces with copper and teal accents; motion enabled and Unicode
     * decorations.
     */
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
    /** Use terminal-configured palette colors and ASCII decorations; motion remains enabled. */
    static auto terminal() -> Theme;
    /** Preserve unthemed drawing and existing explicit styling. */
    static auto plain() -> Theme;
};
/** Role used to resolve a widget's default foreground and background colors. */
enum class Surface {
    Canvas, ///< Ordinary text on the page background.
    Raised, ///< Ordinary text on a raised-surface background.
    Muted, ///< Secondary text on the page background.
    Accent, ///< Primary accent text on the page background.
    Positive, ///< Success text on the page background.
    Negative, ///< Error text on the page background.
    Border ///< Border text on the page background.
};
/** Validate palette colors and motion limits. @throws std::invalid_argument Invalid
 * policy values.
 */
void validate(Theme const&);
/** Fill default colors from a semantic role; preserve explicit colors and all traits.
 * Plain themes leave styles unchanged.
 */
auto theme_style(Style, Theme const&, Surface = Surface::Canvas) -> Style;
/** Resolve focused/inactive selection colors, then apply the explicit selection patch. */
auto theme_selection(Style, StylePatch const&, Theme const&, bool focused = true)
    -> Style;
/** RGB interpolation; palette/default colors switch directly rather than guessing RGB.
 */
auto mix_color(Color const&, Color const&, double amount) -> Color;
}  // namespace ox
