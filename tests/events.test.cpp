#define TEST_MAIN
#include <zzz/test.hpp>

#include <ox/core/core.hpp>

using namespace ox;

TEST(key_actions_preserve_event_data)
{
    struct Handler {
        KeyEvent received;
        bool released = false;

        auto handle_key_press(KeyEvent e) -> EventResponse
        {
            received = e;
            return {};
        }

        auto handle_key_release(KeyEvent e) -> EventResponse
        {
            received = e;
            released = true;
            return {};
        }
    } handler;
    auto buffer = ScreenBuffer{{0, 0}};
    auto key = KeyEvent{.key = U'a',
                        .modifiers = {.shift = true},
                        .action = KeyAction::Repeat,
                        .text = U"A"};
    ASSERT((apply_event(key, handler, buffer).has_value()));
    ASSERT((handler.received == key));
    ASSERT((!handler.released));
    key.action = KeyAction::Release;
    ASSERT((apply_event(key, handler, buffer).has_value()));
    ASSERT((handler.received == key));
    ASSERT((handler.released));
}

TEST(mouse_events_preserve_buttons_modifiers_and_wheel_steps)
{
    struct Handler {
        MouseRelease release;
        MouseMove move;
        MouseWheel wheel;

        auto handle_mouse_release(MouseRelease e) -> EventResponse
        {
            release = e;
            return {};
        }
        auto handle_mouse_move(MouseMove e) -> EventResponse
        {
            move = e;
            return {};
        }
        auto handle_mouse_wheel(MouseWheel e) -> EventResponse
        {
            wheel = e;
            return {};
        }
    } handler;
    auto buffer = ScreenBuffer{{0, 0}};
    auto const release = MouseRelease{{2, 3}, std::nullopt, {.ctrl = true}};
    auto const move = MouseMove{{4, 5}, MouseButton::Left, {.shift = true}};
    auto const wheel = MouseWheel{{6, 7}, -2, 3, {.alt = true}};
    ASSERT((apply_event(release, handler, buffer).has_value()));
    ASSERT((apply_event(move, handler, buffer).has_value()));
    ASSERT((apply_event(wheel, handler, buffer).has_value()));
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
        auto handle_paste(esc::PasteChunk const& chunk) -> EventResponse
        {
            bytes += chunk.bytes;
            return {};
        }
    } handler;
    auto buffer = ScreenBuffer{{0, 0}};
    ASSERT(
        (apply_event(esc::PasteChunk{"abc", true, true}, handler, buffer).has_value()));
    ASSERT((handler.bytes == "abc"));
    ASSERT((!apply_event(esc::Focus{true}, handler, buffer).has_value()));
    ASSERT((!apply_event(KeyEvent{.key = U'a'}, handler, buffer).has_value()));
    auto result = apply_event(event::Interrupt{}, handler, buffer);
    ASSERT((result.has_value()));
    ASSERT((result->has_value()));
    ASSERT(((*result)->return_code == 1));
}
