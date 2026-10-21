#include <algorithm>
#include <array>
#include <ox/ox.hpp>
#include <zzz/test.hpp>

using namespace ox;
namespace {
void same_frame(Frame const& a, Frame const& b)
{
    ASSERT(a.size() == b.size());
    for (int y = 0; y < a.size().height; ++y)
        ASSERT(std::ranges::equal(a.row(y), b.row(y)));
}
void seed_wide(Frame& frame)
{
    for (int y = 0; y < frame.size().height; ++y)
        for (int x = 0; x + 1 < frame.size().width; x += 3)
            frame.write({x, y}, U"界", {.background = colors::red});
}
}  // namespace

TEST(single_column_writes_preserve_aliases_and_renderer_revisions)
{
    auto frame = Frame{{6, 1}};
    auto canvas = Canvas{frame};
    auto profile = esc::TerminalProfile{};
    profile.control = true;
    auto renderer = Renderer{profile};
    auto text = std::u32string{U'e'} + std::u32string(40, U'\u0301');
    ASSERT(canvas.write({0, 0}, text));
    renderer.render(frame);
    ASSERT(canvas.write({0, 0}, frame[{0, 0}].text));
    ASSERT(renderer.render(frame).empty());
    auto borrowed = std::u32string_view{frame[{0, 0}].text}.substr(0, 1);
    ASSERT(canvas.write({0, 0}, borrowed, {.foreground = colors::green}));
    ASSERT((frame[Point{0, 0}].text == U"e"));
    ASSERT(!renderer.render(frame).empty());
    ASSERT(renderer.render(frame).empty());
    for (auto glyph : {U"⣿", U"▄", U"á", U"👩‍💻", U"🇺🇸", U"界", U"x"}) {
        ASSERT(canvas.write({1, 0}, glyph));
        ASSERT(frame.write({2, 0}, U"z"));
        for (int x = 0; x < 6; ++x) {
            auto const& cell = frame[{x, 0}];
            if (cell.columns)
                ASSERT(esc::grapheme_width(cell.text, frame.width_policy()) ==
                       cell.columns);
        }
    }
    auto before = frame;
    for (auto invalid : {U"", U"ab", U"\n", U"\u0301", U"\x1b"}) {
        ASSERT_THROWS(canvas.write({0, 0}, invalid), std::invalid_argument);
        ASSERT_THROWS(canvas.write({-1, 0}, invalid), std::invalid_argument);
        ASSERT_THROWS(frame.write({20, 0}, invalid), std::invalid_argument);
        same_frame(frame, before);
    }
}

TEST(canvas_fast_path_honors_ascii_width_overrides_and_partial_grapheme_clips)
{
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    auto frame = Frame{{6, 1}, {.overrides = overrides}};
    auto canvas = Canvas{frame};
    ASSERT(canvas.write({1, 0}, U"a"));
    ASSERT((frame[Point{2, 0}].columns == 0));
    auto before = frame;
    ASSERT(!canvas.subcanvas({2, 0}, {3, 1}).write({0, 0}, U"x"));
    same_frame(frame, before);
    ASSERT(canvas.write({2, 0}, U"x"));
    ASSERT((frame[Point{1, 0}].text == U" "));
    ASSERT(!canvas.write({5, 0}, U"a"));
}

TEST(grid_painters_match_general_writes_over_wide_cells_and_nested_clips)
{
    for (int offset = -3; offset <= 3; ++offset) {
        auto actual = Frame{{12, 4}};
        seed_wide(actual);
        auto expected = actual;
        auto a =
            Canvas{actual}.subcanvas({1, 0}, {9, 4}).subcanvas({offset, 0}, {8, 4});
        auto b =
            Canvas{expected}.subcanvas({1, 0}, {9, 4}).subcanvas({offset, 0}, {8, 4});
        auto grid = DotGrid{{16, 16},
                            {.foreground = colors::green, .background = colors::blue}};
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 8; ++x) {
                bool on = (x + y) % 2;
                grid.set_dot({x * 2, y * 4}, on);
                b.write({x, y}, on ? U"⠁" : U" ",
                        {.background = colors::blue, .foreground = colors::green});
            }
        grid.paint(a);
        same_frame(actual, expected);
        seed_wide(actual);
        expected = actual;
        auto pixels = std::vector<RGB>(8 * 7, RGB{0x123456});
        paint_pixels(a, {8, 7}, pixels, colors::yellow);
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 8; ++x)
                b.write({x, y}, U"▄",
                        {.background = RGB{0x123456},
                         .foreground =
                             y == 3 ? Color{colors::yellow} : Color{RGB{0x123456}}});
        same_frame(actual, expected);
    }
}

TEST(application_frame_clear_matches_direct_paint_for_themes_and_generic_loops)
{
    struct Root : Widget {
        auto paint(Canvas c) -> Cursor override
        {
            c.write({1, 0}, U"界", {.foreground = colors::green});
            return Point{1, 0};
        }
    } root;
    auto app = Application{root};
    app.dispatch(esc::Resize{{8, 3}});
    for (auto theme : {Theme::dark(), Theme::light(), Theme::plain()}) {
        app.set_theme(theme);
        ASSERT(app.paint());
        auto expected = Frame{{8, 3}};
        auto cursor = app.paint(Canvas{expected});
        same_frame(app.loop().screen, expected);
        ASSERT(app.loop().cursor == cursor);
        app.request_repaint();
        ASSERT(app.loop().paint(app));
        same_frame(app.loop().screen, expected);
        ASSERT(!app.loop().paint(app));
    }
    // Direct painting into an inherited clip must still apply the theme locally.
    app.set_theme(Theme::dark());
    auto external = Frame{{10, 4}};
    external.clear({.background = colors::red});
    app.paint(Canvas{external}.subcanvas({2, 1}, {5, 2}));
    ASSERT((external[Point{0, 0}].style.background == Color{colors::red}));
    ASSERT((external[Point{2, 1}].style == theme_style({}, Theme::dark())));
    struct Handler {
        auto paint(Canvas) -> Cursor { return {}; }
    } handler;
    auto loop = EventLoop{};
    loop.screen = Frame{{2, 1}};
    loop.screen.write({0, 0}, U"界", {.background = colors::red});
    loop.request_repaint();
    ASSERT(loop.paint(handler));
    ASSERT((loop.screen[Point{0, 0}] == Cell{}));
    ASSERT((loop.screen[Point{1, 0}] == Cell{}));
}

TEST(dotgrid_batch_validates_all_visible_masks_before_writing)
{
    auto grid = DotGrid{{64, 32}};
    constexpr int bits[2][4] = {{0, 1, 2, 6}, {3, 4, 5, 7}};
    for (int mask = 0; mask < 256; ++mask)
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 4; ++y)
                grid.set_dot({(mask % 32) * 2 + x, (mask / 32) * 4 + y},
                             mask & (1 << bits[x][y]));
    auto ordinary = Frame{{32, 8}};
    grid.paint(Canvas{ordinary});
    for (int mask = 0; mask < 256; ++mask)
        ASSERT((ordinary[Point{mask % 32, mask / 32}].text ==
                std::u32string(1, mask ? U'\u2800' + mask : U' ')));
    auto overrides = std::array{esc::ScalarWidthOverride{U'⣿', 2}};
    auto incompatible = Frame{{32, 8}, {.overrides = overrides}};
    seed_wide(incompatible);
    auto before = incompatible;
    ASSERT_THROWS(grid.paint(Canvas{incompatible}), std::invalid_argument);
    same_frame(incompatible, before);
    // An incompatible glyph outside the clip must not reject the visible paint.
    grid.paint(Canvas{incompatible}.subcanvas({}, {2, 1}));
    ASSERT((incompatible[Point{0, 0}].text == U" "));
    ASSERT((incompatible[Point{1, 0}].text == U"⠁"));
    auto empty = Frame{{0, 0}, {.overrides = overrides}};
    grid.paint(Canvas{empty});
}
