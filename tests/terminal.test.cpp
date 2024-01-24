#include <termox/terminal.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Construction", "[Terminal]")
{
    struct {
        ox::Point coordinates = {.x = 5, .y = 3};
        ox::Area size         = {.width = 40, .height = 20};
    } widget;

    auto sb = ox::ScreenBuffer{{.width = 20, .height = 10}};

    auto c = ox::Painter{widget};
}