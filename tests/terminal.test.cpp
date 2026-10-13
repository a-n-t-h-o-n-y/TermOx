#include <zzz/test.hpp>

#include <ox/core/core.hpp>
#include <ox/put.hpp>

using namespace ox;

TEST(canvas_preserves_styles_and_clips_glyph_ranges)
{
    auto buffer = ScreenBuffer{{4, 2}};
    auto canvas = Canvas{buffer, {1, 0}, {2, 2}};
    auto const glyphs = esc::glyphs(U"éabc") | fg(colors::cyan) | Trait::Bold;
    put(canvas, {0, 1}, glyphs);
    ASSERT((buffer[Point{1, 1}] == glyphs[0]));
    ASSERT((buffer[Point{2, 1}] == glyphs[1]));
    ASSERT((buffer[Point{3, 1}] == Glyph{}));
    fill(canvas, bg(colors::blue));
    ASSERT((buffer[Point{1, 1}].style.background == Color{colors::blue}));
    ASSERT((buffer[Point{1, 1}].style.foreground == Color{colors::cyan}));
}

TEST(resize_reinitializes_the_screen_buffer)
{
    struct Handler {
        Area size;
        auto handle_resize(Area a) -> EventResponse
        {
            size = a;
            return {};
        }
    } handler;
    auto buffer = ScreenBuffer{{1, 1}};
    buffer[{0, 0}] = U'x' | fg(colors::red);
    ASSERT((apply_event(esc::Resize{{2, 3}}, handler, buffer).has_value()));
    ASSERT(((handler.size == Area{2, 3})));
    ASSERT(((buffer.size() == Area{2, 3})));
    ASSERT((buffer[Point{0, 0}] == Glyph{}));
    ASSERT((buffer[Point{1, 2}] == Glyph{}));
}
