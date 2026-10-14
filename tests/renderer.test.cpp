#include <zzz/test.hpp>

#include <random>
#include <string>

// Rendering depends on encoding and frame storage, never a terminal session.
#include <ox/core/canvas.hpp>
#include <ox/core/renderer.hpp>

#include <esc/text.hpp>

using namespace ox;

namespace {
auto profile() -> esc::TerminalProfile
{
    auto result = esc::TerminalProfile{};
    result.control = true;
    result.colors = esc::ColorSupport::TrueColor;
    result.traits = Trait::Bold | Trait::Italic | Trait::Underline;
    return result;
}

auto at(int x, int y) -> std::string { return esc::encode(esc::Cursor{x, y}); }
auto style(Style value = {}) -> std::string { return esc::encode(profile(), value); }

// Apply the renderer's cursor and text output to a headless display. SGR behavior
// is checked separately against exact sequences, rather than duplicated here.
void display(Frame& screen, Point& cursor, std::string_view bytes)
{
    while (!bytes.empty()) {
        if (bytes.starts_with("\x1b[")) {
            auto const end = bytes.find_first_of("Hm");
            ASSERT(end != std::string_view::npos);
            if (bytes[end] == 'H') {
                auto const separator = bytes.find(';');
                ASSERT(separator < end);
                cursor.y = std::stoi(std::string{bytes.substr(2, separator - 2)}) - 1;
                cursor.x = std::stoi(std::string{
                               bytes.substr(separator + 1, end - separator - 1)}) -
                           1;
            }
            bytes.remove_prefix(end + 1);
        }
        else {
            auto const end = bytes.find('\x1b');
            auto const text = bytes.substr(0, end);
            for (auto cluster : esc::graphemes(text)) {
                auto const decoded = esc::utf8_to_utf32(cluster);
                ASSERT(screen.write(cursor, decoded));
                cursor.x += static_cast<int>(
                    *esc::grapheme_width(decoded, screen.width_policy()));
            }
            bytes.remove_prefix(text.size());
        }
    }
}

void check_text(Frame const& expected, Frame const& actual)
{
    ASSERT(expected.size() == actual.size());
    for (int y = 0; y < expected.size().height; ++y) {
        for (int x = 0; x < expected.size().width; ++x) {
            ASSERT((expected[Point{x, y}].text == actual[Point{x, y}].text));
            ASSERT((expected[Point{x, y}].columns == actual[Point{x, y}].columns));
        }
    }
}
}  // namespace

TEST(renderer_requires_cursor_addressing)
{ ASSERT_THROWS(Renderer{esc::TerminalProfile{}}, std::invalid_argument); }

TEST(renderer_initial_rows_are_runs_and_unchanged_frames_are_empty)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{4, 2}};
    ASSERT(renderer.render(frame) == at(0, 0) + style() + "    " + at(0, 1) + "    ");
    ASSERT(renderer.render(frame).empty());
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_single_adjacent_and_disjoint_edits_reuse_output_and_style)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{12, 2}};
    auto const storage = renderer.render(frame).data();
    frame.write({2, 0}, U"a");
    ASSERT(renderer.render(frame) == at(2, 0) + "a");
    // Already positioned at the next cell; even the run's initial move is omitted.
    frame.write({3, 0}, U"b");
    frame.write({4, 0}, U"c");
    ASSERT(renderer.render(frame) == "bc");
    frame.write({1, 1}, U"x");
    frame.write({3, 1}, U"y");
    auto const output = renderer.render(frame);
    ASSERT(output == at(1, 1) + "x" + at(3, 1) + "y");
    ASSERT(output.data() == storage);
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_style_changes_do_not_break_adjacent_runs_or_reset_each_frame)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{5, 1}};
    auto const red = Style{.foreground = colors::red, .traits = Trait::Bold};
    frame.write({0, 0}, U"a", red);
    frame.write({1, 0}, U"b", red);
    ASSERT(renderer.render(frame) == at(0, 0) + style(red) + "ab" + style() + "   ");
    frame.write({2, 0}, U"c", red);
    ASSERT(renderer.render(frame) == at(2, 0) + style(red) + "c");
    frame.write({3, 0}, U"d", red);
    ASSERT(renderer.render(frame) == "d");
    frame.write({4, 0}, U"e");
    ASSERT(renderer.render(frame) == style() + "e");
}

TEST(renderer_compares_styles_after_capability_adaptation)
{
    auto plain_colors = profile();
    plain_colors.colors = esc::ColorSupport::None;
    plain_colors.traits = {};
    auto renderer = Renderer{plain_colors};
    auto frame = Frame{{3, 1}};
    frame.write({0, 0}, U"a", {.foreground = colors::red});
    frame.write({1, 0}, U"b", {.foreground = colors::blue, .traits = Trait::Bold});
    ASSERT(renderer.render(frame) ==
           at(0, 0) + esc::encode(plain_colors, Style{}) + "ab ");
}

TEST(renderer_theme_changes_repaint_default_colors_and_preserve_explicit_colors)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{2, 1}};
    auto const explicit_style =
        Style{.background = colors::blue, .foreground = colors::red};
    frame.write({1, 0}, U"x", explicit_style);
    auto const theme = Style{.background = colors::green, .foreground = colors::yellow};
    ASSERT(renderer.render(frame, {}, theme.foreground, theme.background) ==
           at(0, 0) + style(theme) + " " + style(explicit_style) + "x");
    ASSERT(renderer.render(frame, {}, theme.foreground, theme.background).empty());
    ASSERT(renderer.render(frame) ==
           at(0, 0) + style() + " " + style(explicit_style) + "x");
}

TEST(renderer_cursor_only_changes_snap_to_leads_and_ignore_outside_points)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{5, 1}};
    frame.write({1, 0}, U"界");
    auto output = std::string{renderer.render(frame, Point{2, 0})};
    ASSERT(output.ends_with(at(1, 0)));
    ASSERT(renderer.render(frame, Point{2, 0}).empty());
    ASSERT(renderer.render(frame, Point{4, 0}) == at(4, 0));
    ASSERT(renderer.render(frame, Point{-1, 0}).empty());
    ASSERT(renderer.render(frame, Point{5, 0}).empty());
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_right_margin_requires_explicit_positioning)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{2, 2}};
    renderer.render(frame);
    frame.write({1, 0}, U"a");
    frame.write({0, 1}, U"b");
    ASSERT(renderer.render(frame) == at(1, 0) + "a" + at(0, 1) + "b");
    frame.write({1, 1}, U"c");
    ASSERT(renderer.render(frame, Point{1, 1}) == "c" + at(1, 1));
}

TEST(renderer_wide_grapheme_replacement_erases_old_footprints)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{6, 1}};
    auto screen = Frame{frame.size()};
    auto cursor = Point{};
    frame.write({1, 0}, U"界");
    display(screen, cursor, renderer.render(frame));
    check_text(frame, screen);
    frame.write({2, 0}, U"a");
    auto output = renderer.render(frame);
    ASSERT(output == at(1, 0) + " a");
    display(screen, cursor, output);
    check_text(frame, screen);
    frame.write({1, 0}, U"👩‍💻");
    display(screen, cursor, renderer.render(frame));
    check_text(frame, screen);
    frame.write({0, 0}, U"界");
    display(screen, cursor, renderer.render(frame));
    check_text(frame, screen);
    frame.write({3, 0}, U"é");
    display(screen, cursor, renderer.render(frame));
    check_text(frame, screen);
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_preserves_boundaries_between_clusters_that_would_join)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{6, 1}};
    ASSERT(frame.write({0, 0}, U"🇺"));
    auto const width = frame[Point{0, 0}].columns;
    ASSERT(frame.write({static_cast<int>(width), 0}, U"🇸"));
    auto const output = std::string{renderer.render(frame)};
    ASSERT(output.find(esc::utf32_to_utf8(U"🇺") + at(static_cast<int>(width), 0) +
                       esc::utf32_to_utf8(U"🇸")) != std::string::npos);
    // The same boundary must be preserved when the second glyph changes later.
    renderer.invalidate();
    frame.clear();
    frame.write({0, 0}, U"🇺");
    renderer.render(frame, Point{static_cast<int>(width), 0});
    // Move away and back to write the first glyph, leaving the cursor after it.
    frame.write({0, 0}, U"🇨");
    renderer.render(frame);
    frame.write({static_cast<int>(width), 0}, U"🇦");
    ASSERT(renderer.render(frame).starts_with(at(static_cast<int>(width), 0)));
}

TEST(renderer_scrolling_changes_rows_without_terminal_scroll_commands)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{4, 3}};
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 4; ++x) {
            frame.write({x, y}, std::u32string(1, static_cast<char32_t>(U'a' + y)));
        }
    }
    renderer.render(frame);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 4; ++x) {
            frame.write({x, y}, std::u32string(1, static_cast<char32_t>(U'b' + y)));
        }
    }
    ASSERT(renderer.render(frame) ==
           at(0, 0) + "bbbb" + at(0, 1) + "cccc" + at(0, 2) + "dddd");
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_resize_repaints_even_when_cell_count_is_unchanged)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{4, 2}};
    renderer.render(frame);
    frame.resize({2, 4});
    ASSERT(renderer.render(frame) ==
           at(0, 0) + "  " + at(0, 1) + "  " + at(0, 2) + "  " + at(0, 3) + "  ");
    frame.resize({1, 1});
    ASSERT(renderer.render(frame) == at(0, 0) + " ");
    frame.resize({0, 7});
    ASSERT(renderer.render(frame, Point{0, 0}).empty());
    frame.resize({3, 1});
    ASSERT(renderer.render(frame) == at(0, 0) + "   ");
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_invalidation_and_encoding_failure_force_complete_repaint)
{
    auto strict = profile();
    strict.fallback = esc::FallbackPolicy::Strict;
    auto renderer = Renderer{strict};
    auto frame = Frame{{3, 1}};
    renderer.render(frame);
    renderer.invalidate();
    ASSERT(renderer.render(frame) == at(0, 0) + style() + "   ");
    frame.write({0, 0}, U"a");
    frame.write({1, 0}, U"b", {.traits = Trait::Blink});
    ASSERT_THROWS(renderer.render(frame), esc::UnsupportedFeature);
    frame.clear();
    ASSERT(renderer.render(frame) == at(0, 0) + style() + "   ");
    ASSERT(renderer.render(frame).empty());
}

TEST(renderer_random_edits_and_resizes_match_headless_display)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{12, 4}};
    auto screen = Frame{frame.size()};
    auto cursor = Point{};
    auto random = std::mt19937{42};
    auto const glyphs =
        std::vector<std::u32string>{U" ", U"x", U"界", U"é", U"👩‍💻"};
    for (int step = 0; step < 500; ++step) {
        if (step % 47 == 0) {
            auto const size = Area{static_cast<int>(random() % 15 + 1),
                                   static_cast<int>(random() % 6 + 1)};
            frame.resize(size);
            screen.resize(size);
        }
        frame.write(
            {static_cast<int>(random() % static_cast<unsigned>(frame.size().width)),
             static_cast<int>(random() % static_cast<unsigned>(frame.size().height))},
            glyphs[random() % glyphs.size()]);
        display(screen, cursor, renderer.render(frame));
        check_text(frame, screen);
        ASSERT(renderer.render(frame).empty());
    }
}

TEST(renderer_row_tracking_handles_copies_moves_assignments_and_reused_storage)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{8, 3}};
    frame.write({1, 1}, U"x");
    renderer.render(frame);
    auto copy = frame;
    copy.write({1, 1}, U"y");
    ASSERT(renderer.render(copy).find('y') != std::string_view::npos);
    ASSERT(renderer.render(frame).find('x') != std::string_view::npos);
    auto moved = std::move(frame);
    ASSERT(renderer.render(moved).empty());
    frame = Frame{{8, 3}};
    frame.write({1, 1}, U"z");
    ASSERT(renderer.render(frame).find('z') != std::string_view::npos);
    frame = copy;
    ASSERT(renderer.render(frame).find('y') != std::string_view::npos);
    for (int i = 0; i < 10; ++i) {
        auto replacement = Frame{{8, 3}};
        replacement.write({1, 1}, i % 2 ? U"a" : U"b");
        ASSERT(!renderer.render(replacement).empty());
    }
}

TEST(renderer_row_tracking_handles_clear_resize_styles_wide_cells_and_invalidation)
{
    auto renderer = Renderer{profile()};
    auto frame = Frame{{8, 3}};
    renderer.render(frame);
    frame.write({2, 1}, U"日");
    ASSERT(!renderer.render(frame).empty());
    ASSERT(frame.write({2, 1}, U"日"));
    ASSERT(renderer.render(frame).empty());
    auto canvas = Canvas{frame};
    canvas.restyle({3, 1}, {.traits = Trait::Bold});
    ASSERT(!renderer.render(frame).empty());
    canvas.restyle({3, 1}, {.traits = Trait::Bold});
    ASSERT(renderer.render(frame).empty());
    frame.write({3, 1}, U"x");
    ASSERT(!renderer.render(frame).empty());
    frame.clear();
    ASSERT(!renderer.render(frame).empty());
    frame.clear();
    ASSERT(renderer.render(frame).empty());
    renderer.invalidate();
    ASSERT(!renderer.render(frame).empty());
    ASSERT(!renderer.render(frame, {}, colors::red).empty());
    frame.resize({8, 3});
    ASSERT(renderer.render(frame, {}, colors::red).empty());
    frame.resize({10, 4});
    ASSERT(!renderer.render(frame, {}, colors::red).empty());
}

TEST(renderer_repainted_row_preserves_equal_prefix_and_wide_footprints)
{
    auto frame = Frame{{24, 2}};
    auto screen = Frame{frame.size()};
    auto renderer = Renderer{profile()};
    auto cursor = Point{};
    auto paint = [&] {
        frame.clear();
        frame.write({0, 0}, U"é");
        frame.write({8, 0}, U"界");
        frame.write({20, 0}, U"x");
    };
    paint();
    display(screen, cursor, renderer.render(frame));
    paint();
    ASSERT(renderer.render(frame).empty());
    frame.write({20, 0}, U"y");
    auto const output = renderer.render(frame);
    ASSERT(output == at(20, 0) + "y");
    display(screen, cursor, output);
    // Replace a continuation near the first mismatch, preserving the prefix.
    frame.write({9, 0}, U"z");
    display(screen, cursor, renderer.render(frame));
    check_text(frame, screen);
    ASSERT(renderer.render(frame).empty());
}
