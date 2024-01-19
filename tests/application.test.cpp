#include <termox/application.hpp>

#include <catch2/catch_test_macros.hpp>

#include <esc/area.hpp>
#include <esc/point.hpp>

TEST_CASE("Construction", "[Application]")
{
    struct {
        esc::Point coordinates;
        esc::Area size;
        auto handle_key_press(esc::Key) -> ox::EventResponse
        {
            return std::nullopt;
        }
        auto handle_key_release(esc::Key) -> ox::EventResponse
        {
            return std::nullopt;
        }
    } widget;

    auto app = ox::Application{};
    if (false) {  // causes inf loop right now, so compile but don't run.
        app.run(widget);
    }
}