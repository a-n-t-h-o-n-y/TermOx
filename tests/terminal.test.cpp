#include <zzz/test.hpp>

#include <ox/core/core.hpp>
#include <ox/put.hpp>

using namespace ox;

TEST(canvas_preserves_styles_and_clips_glyph_ranges)
{
    auto buffer = Frame{{4, 2}};
    auto canvas = Canvas{buffer, {1, 0}, {2, 2}};
    auto const glyphs = esc::glyphs(U"éabc") | fg(colors::cyan) | Trait::Bold;
    put(canvas, {0, 1}, glyphs);
    ASSERT((buffer[Point{1, 1}].text == U"é"));
    ASSERT((buffer[Point{2, 1}].text == U"a"));
    ASSERT((buffer[Point{3, 1}] == Cell{}));
    fill(canvas, bg(colors::blue));
    ASSERT((buffer[Point{1, 1}].style.background == Color{colors::blue}));
    ASSERT((buffer[Point{1, 1}].style.foreground == Color{colors::cyan}));
}

TEST(resize_reinitializes_the_screen_buffer)
{
    struct Handler {
        Area size;
        auto handle(esc::Resize const& event) -> EventResponse
        {
            size = event.size;
            return {.handled = true};
        }
    } handler;
    auto buffer = Frame{{1, 1}};
    buffer.write({0, 0}, U"x", {.foreground = colors::red});
    ASSERT((dispatch(esc::Resize{{2, 3}}, handler, buffer).handled));
    ASSERT(((handler.size == Area{2, 3})));
    ASSERT(((buffer.size() == Area{2, 3})));
    ASSERT((buffer[Point{0, 0}] == Cell{}));
    ASSERT((buffer[Point{1, 2}] == Cell{}));
}
