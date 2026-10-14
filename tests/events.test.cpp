#define TEST_MAIN
#include <zzz/test.hpp>

#include <ox/core/core.hpp>

using namespace ox;

TEST(key_actions_preserve_event_data)
{
    struct Handler {
        KeyEvent received;
        bool released = false;

        auto handle(KeyEvent const& e) -> EventResponse
        {
            received = e;
            released = e.action == KeyAction::Release;
            return {.handled = true};
        }

    } handler;
    auto buffer = Frame{{0, 0}};
    auto key = KeyEvent{.key = U'a',
                        .modifiers = {.shift = true},
                        .action = KeyAction::Repeat,
                        .text = U"A"};
    ASSERT((dispatch(key, handler, buffer).handled));
    ASSERT((handler.received == key));
    ASSERT((!handler.released));
    key.action = KeyAction::Release;
    ASSERT((dispatch(key, handler, buffer).handled));
    ASSERT((handler.received == key));
    ASSERT((handler.released));
}

TEST(mouse_events_preserve_buttons_modifiers_and_wheel_steps)
{
    struct Handler {
        MouseRelease release;
        MouseMove move;
        MouseWheel wheel;

        auto handle(MouseRelease const& e) -> EventResponse
        {
            release = e;
            return {.handled = true};
        }
        auto handle(MouseMove const& e) -> EventResponse
        {
            move = e;
            return {.handled = true};
        }
        auto handle(MouseWheel const& e) -> EventResponse
        {
            wheel = e;
            return {.handled = true};
        }
    } handler;
    auto buffer = Frame{{0, 0}};
    auto const release = MouseRelease{{2, 3}, std::nullopt, {.ctrl = true}};
    auto const move = MouseMove{{4, 5}, MouseButton::Left, {.shift = true}};
    auto const wheel = MouseWheel{{6, 7}, -2, 3, {.alt = true}};
    ASSERT((dispatch(release, handler, buffer).handled));
    ASSERT((dispatch(move, handler, buffer).handled));
    ASSERT((dispatch(wheel, handler, buffer).handled));
    ASSERT((handler.release == release));
    ASSERT((handler.move == move));
    ASSERT((handler.wheel == wheel));
}

TEST(input_text_accepts_scalar_and_associated_text)
{
    ASSERT((input_text(KeyEvent{.key = U'é'}) == U"é"));
    ASSERT((input_text(KeyEvent{.text = U"日本"}) == U"日本"));
    ASSERT((input_text(KeyEvent{
                .key = U'a', .modifiers = {.shift = true}, .text = U"A"}) == U"A"));
    ASSERT((input_text(KeyEvent{.key = U'a', .modifiers = {.ctrl = true}}).empty()));
    ASSERT((input_text(KeyEvent{.key = U'a', .action = KeyAction::Release}).empty()));
    ASSERT((input_text(KeyEvent{.key = KeyCode::Enter}).empty()));
    ASSERT((input_text(KeyEvent{.text = U"a\x1b\x7f"}) == U"a"));
}

TEST(paste_dispatch_and_unhandled_events)
{
    struct Handler {
        std::string bytes;
        auto handle(esc::PasteChunk const& chunk) -> EventResponse
        {
            bytes += chunk.bytes;
            return {.handled = true};
        }
    } handler;
    auto buffer = Frame{{0, 0}};
    ASSERT((dispatch(esc::PasteChunk{"abc", true, true}, handler, buffer).handled));
    ASSERT((handler.bytes == "abc"));
    ASSERT((!dispatch(esc::Focus{true}, handler, buffer).handled));
    ASSERT((!dispatch(KeyEvent{.key = U'a'}, handler, buffer).handled));
    auto result = dispatch(event::Interrupt{}, handler, buffer);
    ASSERT((result.handled));
    ASSERT((result.quit == 1));
}
