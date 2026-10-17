#include <algorithm>
#include <cmath>
#include <ox/theme.hpp>
#include <stdexcept>
namespace ox {
auto Theme::dark() -> Theme
{
    return {.palette = {.background = RGB{0x141917},
                        .surface = RGB{0x1f2924},
                        .foreground = RGB{0xe8e6db},
                        .muted = RGB{0xa4b0a5},
                        .accent = RGB{0xe7af7d},
                        .secondary = RGB{0x83c8bb},
                        .positive = RGB{0x9bca98},
                        .negative = RGB{0xf09285},
                        .border = RGB{0x53655a},
                        .selection = RGB{0x303c32}},
            .motion = {}};
}
auto Theme::light() -> Theme
{
    return {.palette = {.background = RGB{0xf4f1e8},
                        .surface = RGB{0xe9e5d8},
                        .foreground = RGB{0x28362f},
                        .muted = RGB{0x5f6b61},
                        .accent = RGB{0x905024},
                        .secondary = RGB{0x236e66},
                        .positive = RGB{0x3b6c3a},
                        .negative = RGB{0xa23232},
                        .border = RGB{0x8a9586},
                        .selection = RGB{0xd9dfce}},
            .motion = {}};
}
auto Theme::ocean() -> Theme
{
    return {.palette = {.background = RGB{0x0f172a},
                        .surface = RGB{0x17233b},
                        .foreground = RGB{0xe2e8f0},
                        .muted = RGB{0xa3b8d0},
                        .accent = RGB{0x67d5f5},
                        .secondary = RGB{0xa5b4fc},
                        .positive = RGB{0x8bd49c},
                        .negative = RGB{0xff9b9b},
                        .border = RGB{0x5d789a},
                        .selection = RGB{0x223954}},
            .motion = {}};
}
auto Theme::ember() -> Theme
{
    return {.palette = {.background = RGB{0x211916},
                        .surface = RGB{0x30231e},
                        .foreground = RGB{0xf4e6d8},
                        .muted = RGB{0xc4a998},
                        .accent = RGB{0xffbd75},
                        .secondary = RGB{0xedaa9c},
                        .positive = RGB{0xb4ce86},
                        .negative = RGB{0xff9a8b},
                        .border = RGB{0x947566},
                        .selection = RGB{0x493329}},
            .motion = {}};
}
auto Theme::violet() -> Theme
{
    return {.palette = {.background = RGB{0x1c1828},
                        .surface = RGB{0x292238},
                        .foreground = RGB{0xeee6f7},
                        .muted = RGB{0xbcb0ce},
                        .accent = RGB{0xd6b0ff},
                        .secondary = RGB{0x8bd5ca},
                        .positive = RGB{0xa8d995},
                        .negative = RGB{0xff9eb5},
                        .border = RGB{0x827293},
                        .selection = RGB{0x3b2c50}},
            .motion = {}};
}
auto Theme::paper() -> Theme
{
    return {.palette = {.background = RGB{0xfaf8f2},
                        .surface = RGB{0xeeeae0},
                        .foreground = RGB{0x262d38},
                        .muted = RGB{0x5d6570},
                        .accent = RGB{0x2458a6},
                        .secondary = RGB{0x7a4385},
                        .positive = RGB{0x2c6b42},
                        .negative = RGB{0xa0343d},
                        .border = RGB{0x858174},
                        .selection = RGB{0xdce5f0}},
            .motion = {}};
}
auto Theme::high_contrast() -> Theme
{
    return {.palette = {.background = RGB{0x000000},
                        .surface = RGB{0x141414},
                        .foreground = RGB{0xffffff},
                        .muted = RGB{0xc6c6c6},
                        .accent = RGB{0xffff00},
                        .secondary = RGB{0x00ffff},
                        .positive = RGB{0x66ff66},
                        .negative = RGB{0xff9090},
                        .border = RGB{0xaaaaaa},
                        .selection = RGB{0x303030}},
            .motion = {}};
}
auto Theme::terminal() -> Theme
{
    return {.palette = {.background = default_color,
                        .surface = default_color,
                        .foreground = default_color,
                        .muted = colors::bright_black,
                        .accent = colors::yellow,
                        .secondary = colors::cyan,
                        .positive = colors::green,
                        .negative = colors::red,
                        .border = colors::bright_black,
                        .selection = default_color},
            .motion = {},
            .ascii = true};
}
auto Theme::plain() -> Theme { return {.palette = {}, .motion = {}, .styled = false}; }
void validate(Theme const& t)
{
    if (t.motion.frames_per_second < 1 || t.motion.frames_per_second > 240 ||
        t.motion.feedback.count() < 0 || t.motion.selection.count() < 0 ||
        t.motion.progress.count() < 0)
        throw std::invalid_argument{"Invalid theme motion policy"};
    for (auto const* c : {&t.palette.background, &t.palette.surface,
                          &t.palette.foreground, &t.palette.muted, &t.palette.accent,
                          &t.palette.secondary, &t.palette.positive,
                          &t.palette.negative, &t.palette.border, &t.palette.selection})
        if (c->valueless_by_exception())
            throw std::invalid_argument{"Invalid theme color"};
}
auto theme_style(Style s, Theme const& t, Surface role) -> Style
{
    if (!t.styled) return s;
    auto const& p = t.palette;
    if (s.background == Color{default_color})
        s.background = role == Surface::Raised ? p.surface : p.background;
    if (s.foreground == Color{default_color}) {
        switch (role) {
            case Surface::Muted: s.foreground = p.muted; break;
            case Surface::Accent: s.foreground = p.accent; break;
            case Surface::Positive: s.foreground = p.positive; break;
            case Surface::Negative: s.foreground = p.negative; break;
            case Surface::Border: s.foreground = p.border; break;
            default: s.foreground = p.foreground;
        }
    }
    return s;
}
auto theme_selection(Style s, StylePatch const& custom, Theme const& t, bool focused)
    -> Style
{
    if (!t.styled) return s | custom;
    s.background = t.palette.selection;
    s.foreground = focused ? t.palette.accent : t.palette.muted;
    // The legacy default inverse patch becomes semantic selection; custom patches
    // (including explicitly configured colors) still take precedence.
    auto stock = StylePatch{
        .background = {}, .foreground = {}, .enabled = Trait::Inverse, .disabled = {}};
    if (custom != stock) s = s | custom;
    if (t.ascii && t.palette.selection == Color{default_color})
        s.traits.insert(Trait::Bold);
    return s;
}
auto mix_color(Color const& from, Color const& to, double amount) -> Color
{
    if (!std::isfinite(amount)) throw std::invalid_argument{"Nonfinite color mix"};
    auto t = std::clamp(amount, 0., 1.);
    auto* a = std::get_if<RGB>(&from);
    auto* b = std::get_if<RGB>(&to);
    if (!a || !b) return t <= 0 ? from : to;
    auto channel = [t](int x, int y) {
        return (std::uint8_t)std::lround(x + (y - x) * t);
    };
    return RGB{channel(a->red, b->red), channel(a->green, b->green),
               channel(a->blue, b->blue)};
}
}  // namespace ox
