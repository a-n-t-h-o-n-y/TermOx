#include <zzz/test.hpp>

#include <limits>
#include <random>
#include <type_traits>

#include <ox/ox.hpp>

using namespace ox;

TEST(pixel_dimensions_indices_and_storage_are_checked_before_use)
{
    ASSERT(pixel_count({2, 3}) == 6);
    ASSERT(pixel_count({0, 3}) == 0);
    ASSERT((pixel_cell_size({2, 3}) == Area{2, 2}));
    ASSERT((pixel_cell_size({2, std::numeric_limits<int>::max()}) ==
            Area{2, std::numeric_limits<int>::max() / 2 + 1}));
    ASSERT(pixel_index({2, 3}, {1, 2}) == 5);
    ASSERT_THROWS(pixel_count({-1, 2}), std::invalid_argument);
    ASSERT_THROWS(pixel_cell_size({2, -1}), std::invalid_argument);
    ASSERT_THROWS(pixel_index({2, 3}, {-1, 0}), std::out_of_range);
    ASSERT_THROWS(pixel_index({2, 3}, {2, 0}), std::out_of_range);
    ASSERT_THROWS(pixel_index({2, 3}, {0, 3}), std::out_of_range);
    ASSERT_THROWS(pixel_index({0, 3}, {0, 0}), std::out_of_range);
    ASSERT_THROWS(pixel_index({2, 3}, {std::numeric_limits<int>::max(),
                                       std::numeric_limits<int>::min()}),
                  std::out_of_range);
    ASSERT_THROWS(
        pixel_count({std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}),
        std::length_error);
    ASSERT_THROWS((PixelGrid{Area{2, 3}, std::vector<RGB>(5, RGB{0})}),
                  std::invalid_argument);
    auto grid = PixelGrid{Area{2, 3}, RGB{0x123456}};
    ASSERT(grid.pixels().size() == 6);
    for (auto color : grid.pixels()) {
        ASSERT(color == RGB{0x123456});
    }
}

TEST(pixel_grid_commands_keep_shape_and_schedule_paint_only_when_needed)
{
    auto grid = PixelGrid{Area{2, 3}};
    arrange(grid, {{}, {2, 2}});
    auto app = Application{grid};
    app.dispatch(esc::Resize{grid.size()});
    app.paint();
    ASSERT(!grid.set_pixel({0, 0}, RGB{0}));
    ASSERT(!app.loop().needs_repaint());
    ASSERT(grid.set_pixel({0, 0}, RGB{0xff0000}));
    ASSERT(app.loop().needs_repaint());
    ASSERT((std::as_const(grid).at({0, 0}) == RGB{0xff0000}));
    grid.at({1, 2}) = RGB{0x0000ff};
    auto before = std::vector<RGB>{grid.pixels().begin(), grid.pixels().end()};
    ASSERT_THROWS(grid.set_image({2, 3}, std::vector<RGB>(7, RGB{0})),
                  std::invalid_argument);
    ASSERT_THROWS(grid.reset({-1, 0}), std::invalid_argument);
    ASSERT((grid.pixel_size() == Area{2, 3}));
    ASSERT(std::ranges::equal(grid.pixels(), before));
    grid.set_image({1, 2}, {RGB{0x010203}, RGB{0x040506}});
    ASSERT((grid.pixel_size() == Area{1, 2}));
    ASSERT(grid.at({0, 1}) == RGB{0x040506});
    grid.reset({2, 1}, RGB{0xabcdef});
    ASSERT(grid.pixels().size() == 2 && grid.at({1, 0}) == RGB{0xabcdef});
    grid.reset({});
    ASSERT(grid.pixels().empty());
}

TEST(pixel_painting_maps_upper_and_lower_colors_and_custom_odd_padding)
{
    auto colors = std::array{RGB{0xff0000}, RGB{0x00ff00}, RGB{0x0000ff},
                             RGB{0xffffff}, RGB{0x123456}, RGB{0x654321}};
    auto frame = Frame{{3, 3}};
    fill(Canvas{frame}, U'x' | Style{});
    paint_pixels(Canvas{frame}, {2, 3}, colors, RGB{0xabcdef});
    ASSERT((frame[Point{0, 0}].text == U"▄"));
    ASSERT((frame[Point{0, 0}].style.background == Color{RGB{0xff0000}}));
    ASSERT((frame[Point{0, 0}].style.foreground == Color{RGB{0x0000ff}}));
    ASSERT((frame[Point{1, 0}].style.background == Color{RGB{0x00ff00}}));
    ASSERT((frame[Point{1, 0}].style.foreground == Color{RGB{0xffffff}}));
    ASSERT((frame[Point{0, 1}].style.background == Color{RGB{0x123456}}));
    ASSERT((frame[Point{0, 1}].style.foreground == Color{RGB{0xabcdef}}));
    ASSERT((frame[Point{2, 0}].text == U"x"));
    ASSERT((frame[Point{0, 2}].text == U"x"));
}

TEST(pixel_painting_validates_storage_and_block_width_without_mutating_the_frame)
{
    auto pixels = std::array{RGB{0x123456}, RGB{0x654321}};
    auto overrides = std::array{esc::ScalarWidthOverride{U'▄', 2}};
    auto frame = Frame{{2, 1}, {.overrides = overrides}};
    ASSERT_THROWS(paint_pixels(Canvas{frame}, {1, 2}, pixels), std::invalid_argument);
    ASSERT((frame[Point{0, 0}].text == U" "));
    auto ordinary = Frame{{2, 1}};
    ordinary.write({0, 0}, U"x");
    ASSERT_THROWS(paint_pixels(Canvas{ordinary}, {2, 2}, pixels),
                  std::invalid_argument);
    ASSERT((ordinary[Point{0, 0}].text == U"x"));
    paint_pixels(Canvas{frame}, {0, 1}, {});  // Empty output needs no block glyph.
}

TEST(pixel_widget_and_free_painter_match_across_random_views_and_inherited_clips)
{
    auto random = std::mt19937{2026};
    for (int step = 0; step < 200; ++step) {
        auto const image = Area{(int)(random() % 12), (int)(random() % 13)};
        auto pixels = std::vector<RGB>{};
        for (std::size_t i = 0; i < pixel_count(image); ++i) {
            pixels.push_back(RGB{random() & 0xffffff});
        }
        auto const origin = Point{(int)(random() % 11) - 5, (int)(random() % 9) - 4};
        auto const viewport = Area{(int)(random() % 9), (int)(random() % 7)};
        auto actual = Frame{{10, 8}};
        auto expected = Frame{{10, 8}};
        auto view =
            Canvas{actual}.subcanvas({2, 1}, {6, 5}).subcanvas(origin, viewport);
        auto grid = PixelGrid{image, pixels};
        grid.paint(view);
        auto reference =
            Canvas{expected}.subcanvas({2, 1}, {6, 5}).subcanvas(origin, viewport);
        // Deliberately walk every source cell for an independent clipping reference.
        for (int y = 0; y < pixel_cell_size(image).height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                auto const top = pixels[pixel_index(image, {x, y * 2})];
                auto const bottom = y * 2 + 1 < image.height
                                        ? pixels[pixel_index(image, {x, y * 2 + 1})]
                                        : RGB{0};
                reference.write({x, y}, U"▄",
                                {.background = top, .foreground = bottom});
            }
        }
        for (int y = 0; y < actual.size().height; ++y) {
            ASSERT(std::ranges::equal(actual.row(y), expected.row(y)));
        }
    }
}

TEST(pixel_moves_preserve_image_identity_and_leave_a_reusable_empty_source)
{
    auto grid = PixelGrid{Area{2, 3}, RGB{0x123456}};
    auto app = Application{grid};
    app.dispatch(esc::Resize{{2, 2}});
    app.paint();
    auto moved = PixelGrid{std::move(grid)};
    ASSERT((moved.pixel_size() == Area{2, 3}));
    ASSERT(moved.at({1, 2}) == RGB{0x123456});
    ASSERT(grid.pixels().empty() && grid.pixel_size() == Area{});
    moved.set_pixel({1, 2}, RGB{0xabcdef});
    ASSERT(app.loop().needs_repaint());
    grid.reset({1, 1}, RGB{0});
    grid = std::move(moved);
    ASSERT((grid.pixel_size() == Area{2, 3}));
    ASSERT(grid.at({1, 2}) == RGB{0xabcdef});
    ASSERT(moved.pixels().empty() && moved.pixel_size() == Area{});
    auto* same = &grid;
    grid = std::move(*same);
    ASSERT((grid.pixel_size() == Area{2, 3}));
    moved.reset({1, 1}, RGB{0});
    app.paint();
}
