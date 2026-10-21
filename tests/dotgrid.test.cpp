#include <zzz/test.hpp>
#include <ox/ox.hpp>
#include <limits>
using namespace ox;

TEST(dotgrid_encodes_all_braille_masks_and_clears_individual_dots)
{
    constexpr Point positions[] = {{0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{0,3},{1,3}};
    auto grid = DotGrid{Area{2,4}};
    auto frame = Frame{{1,1}};
    for (int mask = 0; mask < 256; ++mask) {
        for (int bit = 0; bit < 8; ++bit) {
            grid.set_dot(positions[bit], (mask & (1 << bit)) != 0);
            ASSERT(grid.dot(positions[bit]) == ((mask & (1 << bit)) != 0));
        }
        grid.paint(Canvas{frame});
        auto expected = char32_t(mask ? 0x2800 + mask : ' ');
        ASSERT((frame[Point{0,0}].text == std::u32string(1, expected)));
    }
}
TEST(dotgrid_partial_cells_colors_bounds_reset_and_moves)
{
    auto colors = DotColors{RGB{0x123456}, RGB{0x654321}};
    auto grid = DotGrid{Area{3,5}, colors};
    ASSERT((grid.dot_size() == Area{3,5}));
    ASSERT((grid.cell_size() == Area{2,2}));
    grid.set_dot({2,4});
    auto frame = Frame{{3,3}};
    fill(Canvas{frame}, U'x' | Style{});
    grid.paint(Canvas{frame});
    ASSERT((frame[Point{1,1}].text == U"⠁"));
    ASSERT((frame[Point{1,1}].style.foreground == colors.foreground));
    ASSERT((frame[Point{1,1}].style.background == colors.background));
    ASSERT((frame[Point{0,0}].text == U" "));
    ASSERT((frame[Point{2,2}].text == U"x"));
    ASSERT_THROWS(grid.set_dot({3,0}), std::out_of_range);
    ASSERT_THROWS(grid.set_dot({2,5}), std::out_of_range);
    ASSERT_THROWS(grid.dot({-1,0}), std::out_of_range);
    ASSERT_THROWS(grid.set_cell_colors({2,0}, {}), std::out_of_range);
    ASSERT_THROWS(grid.reset({-1,4}), std::invalid_argument);
    ASSERT_THROWS(grid.reset({std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}), std::length_error);
    ASSERT(grid.dot({2,4}));
    auto identity = track(grid);
    auto moved = DotGrid{std::move(grid)};
    ASSERT(&identity.get() == &moved);
    ASSERT((grid.dot_size() == Area{}));
    ASSERT(moved.clear());
    ASSERT(!moved.clear());
    ASSERT(moved.cell_colors({1,1}) == colors);
    grid.reset({1,1});
    grid = std::move(moved);
    ASSERT(&identity.get() == &grid);
    ASSERT((moved.cell_size() == Area{}));
    grid.reset({0,5});
    grid.paint(Canvas{frame});
    ASSERT_THROWS(grid.dot({0,0}), std::out_of_range);
}
TEST(dotgrid_repaint_and_cell_color_independence)
{
    auto grid = DotGrid{Area{4,4}};
    auto app = Application{grid};
    app.dispatch(esc::Resize{{2,1}}); app.paint();
    ASSERT(!grid.set_dot({0,0}, false));
    ASSERT(!grid.set_cell_colors({0,0}, {}));
    ASSERT(!app.loop().needs_repaint());
    ASSERT(grid.set_dot({0,0}));
    ASSERT(app.loop().needs_repaint());
    app.paint();
    ASSERT(grid.set_cell_colors({0,0}, {RGB{0xff0000}, RGB{0x112233}}));
    ASSERT(app.loop().needs_repaint());
    ASSERT(grid.dot({0,0}));
    ASSERT(grid.cell_colors({1,0}) == DotColors{});
    app.paint();
    ASSERT((app.loop().screen[Point{0,0}].style.foreground == Color{RGB{0xff0000}}));
    ASSERT((app.loop().screen[Point{0,0}].style.background == Color{RGB{0x112233}}));
    ASSERT(!grid.set_dot({0,0}));
    ASSERT(!app.loop().needs_repaint());
}
TEST(dotgrid_clips_and_checks_used_glyph_width_before_writing)
{
    auto grid = DotGrid{Area{6,8}};
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 6; ++x) grid.set_dot({x,y});
    auto frame = Frame{{4,3}};
    fill(Canvas{frame}, U'x' | Style{});
    grid.paint(Canvas{frame}.subcanvas({1,1},{2,1}).subcanvas({-1,0},{3,2}));
    ASSERT((frame[Point{0,1}].text == U"x"));
    ASSERT((frame[Point{1,1}].text == U"⣿"));
    ASSERT((frame[Point{2,1}].text == U"⣿"));
    ASSERT((frame[Point{3,1}].text == U"x"));
    ASSERT((frame[Point{1,2}].text == U"x"));
    auto overrides = std::array{esc::ScalarWidthOverride{U'⣿', 2}};
    auto wide = Frame{{3,2}, {.overrides = overrides}};
    grid.set_dot({0,0}, false); // First glyph is valid; a later one must fail atomically.
    ASSERT_THROWS(grid.paint(Canvas{wide}), std::invalid_argument);
    ASSERT((wide[Point{0,0}].text == U" "));
    grid.paint(Canvas{wide}.subcanvas({}, {1,1})); // Invalid widths outside the clip do not matter.
    grid.reset({});
    grid.paint(Canvas{wide});
}
