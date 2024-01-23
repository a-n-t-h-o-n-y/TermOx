#include <termox/terminal.hpp>

#include <catch2/catch_test_macros.hpp>

#include <esc/area.hpp>
#include <esc/point.hpp>

TEST_CASE("Construction", "[Terminal]")
{
    struct {
        esc::Point coordinates = {.x = 5, .y = 3};
        esc::Area size         = {.width = 40, .height = 20};
    } widget;

    auto sb = ox::ScreenBuffer{{.width = 20, .height = 10}};

    auto c = ox::Painter{widget};
}