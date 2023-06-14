#include <clocale>

#include <catch2/catch_test_macros.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>
#include <termox/terminal/detail/canvas.hpp>

void init() { std::setlocale(LC_ALL, "en_US.UTF-8"); }

TEST_CASE("Canvas: Everything", "[Canvas]")
{
    init();

    auto a = ox::detail::Canvas{{15, 20}};
    auto b = ox::detail::Canvas{{15, 20}};

    a.at({1, 1})   = ox::Glyph{U'y', bg(ox::Color::Blue)};
    a.at({14, 19}) = ox::Glyph{U'z', fg(ox::Color::Orange), ox::Trait::Bold};

    b.at({10, 2}) = ox::Glyph{U'a', fg(ox::Color::Blue), ox::Trait::Underline};
    b.at({5, 19}) = ox::Glyph{U'b', ox::Trait::Standout};

    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 300);
    REQUIRE(std::distance(std::cbegin(b), std::cend(b)) == 300);
    CHECK(a.at({1, 1}) == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(a.at({10, 5}) == ox::Glyph{});
    CHECK(a.at({14, 19}) ==
          ox::Glyph{U'z', fg(ox::Color::Orange), ox::Trait::Bold});
    CHECK(b.at({10, 2}) ==
          ox::Glyph{U'a', fg(ox::Color::Blue), ox::Trait::Underline});
    CHECK(b.at({5, 19}) == ox::Glyph{U'b', ox::Trait::Standout});

    auto diff = ox::detail::Canvas::Diff{};
    merge_and_diff(a, b, diff);

    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 300);
    REQUIRE(std::distance(std::cbegin(b), std::cend(b)) == 300);
    CHECK(b.at({1, 1}) == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(b.at({14, 19}) ==
          ox::Glyph{U'z', fg(ox::Color::Orange), ox::Trait::Bold});
    CHECK(b.at({10, 2}) ==
          ox::Glyph{U'a', fg(ox::Color::Blue), ox::Trait::Underline});
    CHECK(b.at({5, 19}) == ox::Glyph{U'b', ox::Trait::Standout});
    CHECK(b.at({10, 5}) == ox::Glyph{});

    REQUIRE(diff.size() == 2);

    CHECK(diff.at(0).first == ox::Point{1, 1});
    CHECK(diff.at(0).second == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(diff.at(1).first == ox::Point{14, 19});
    CHECK(diff.at(1).second ==
          ox::Glyph{U'z', fg(ox::Color::Orange), ox::Trait::Bold});

    a.resize({100, 200});
    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 20'000);
    CHECK(a.at({1, 1}) == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(a.at({10, 20}) == ox::Glyph{});
    CHECK(a.at({14, 19}) ==
          ox::Glyph{U'z', fg(ox::Color::Orange), ox::Trait::Bold});

    a.resize({5, 5});
    CHECK(a.area() == ox::Area{5, 5});
    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 25);
    CHECK(a.at({1, 1}) == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(a.at({4, 3}) == ox::Glyph{});

    b.at({3, 16}) = ox::Glyph{U'x', bg(ox::Color::Blue)};

    generate_color_diff(ox::Color::Blue, b, diff);
    REQUIRE(diff.size() == 3);
    CHECK(diff.at(0).first == ox::Point{1, 1});
    CHECK(diff.at(0).second == ox::Glyph{U'y', bg(ox::Color::Blue)});
    CHECK(diff.at(1).first == ox::Point{10, 2});
    CHECK(diff.at(1).second ==
          ox::Glyph{U'a', fg(ox::Color::Blue), ox::Trait::Underline});
    CHECK(diff.at(2).first == ox::Point{3, 16});
    CHECK(diff.at(2).second == ox::Glyph{U'x', bg(ox::Color::Blue)});
}
