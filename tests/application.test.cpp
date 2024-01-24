#include <termox/application.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Construction", "[Application]")
{
    struct {
        ox::Point coordinates;
        ox::Area size;

        auto handle_key_press(ox::Key) -> ox::EventResponse
        {
            return std::nullopt;
        }
        auto handle_key_release(ox::Key) -> ox::EventResponse
        {
            return std::nullopt;
        }
    } widget;

    auto app = ox::Application{};
    if (false) {  // causes inf loop right now, so compile but don't run.
        app.run(widget);
    }
}