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

    a.at({1, 1})   = U'y' | bg(ox::Color::Blue);
    a.at({14, 19}) = U'z' | fg(ox::Color::Orange) | ox::Trait::Bold;

    b.at({10, 2}) = U'a' | fg(ox::Color::Blue) | ox::Trait::Underline;
    b.at({5, 19}) = U'b' | ox::Trait::Standout;

    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 300);
    REQUIRE(std::distance(std::cbegin(b), std::cend(b)) == 300);
    REQUIRE(a.at({1, 1}) == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(a.at({10, 5}) == ox::Glyph{});
    REQUIRE(a.at({14, 19}) == (U'z' | fg(ox::Color::Orange) | ox::Trait::Bold));
    REQUIRE(b.at({10, 2}) ==
            (U'a' | fg(ox::Color::Blue) | ox::Trait::Underline));
    REQUIRE(b.at({5, 19}) == (U'b' | ox::Trait::Standout));

    auto diff = ox::detail::Canvas::Diff{};
    merge_and_diff(a, b, diff);

    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 300);
    REQUIRE(std::distance(std::cbegin(b), std::cend(b)) == 300);
    REQUIRE(b.at({1, 1}) == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(b.at({14, 19}) == (U'z' | fg(ox::Color::Orange) | ox::Trait::Bold));
    REQUIRE(b.at({10, 2}) ==
            (U'a' | fg(ox::Color::Blue) | ox::Trait::Underline));
    REQUIRE(b.at({5, 19}) == (U'b' | ox::Trait::Standout));
    REQUIRE(b.at({10, 5}) == ox::Glyph{});

    REQUIRE(diff.size() == 2);

    REQUIRE(diff.at(0).first == ox::Point{1, 1});
    REQUIRE(diff.at(0).second == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(diff.at(1).first == ox::Point{14, 19});
    REQUIRE(diff.at(1).second ==
            (U'z' | fg(ox::Color::Orange) | ox::Trait::Bold));

    a.resize({100, 200});
    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 20'000);
    REQUIRE(a.at({1, 1}) == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(a.at({10, 20}) == ox::Glyph{});
    REQUIRE(a.at({14, 19}) == (U'z' | fg(ox::Color::Orange) | ox::Trait::Bold));

    a.resize({5, 5});
    REQUIRE(a.area() == ox::Area{5, 5});
    REQUIRE(std::distance(std::cbegin(a), std::cend(a)) == 25);
    REQUIRE(a.at({1, 1}) == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(a.at({4, 3}) == ox::Glyph{});

    b.at({3, 16}) = U'x' | bg(ox::Color::Blue);

    generate_color_diff(ox::Color::Blue, b, diff);
    REQUIRE(diff.size() == 3);
    REQUIRE(diff.at(0).first == ox::Point{1, 1});
    REQUIRE(diff.at(0).second == (U'y' | bg(ox::Color::Blue)));
    REQUIRE(diff.at(1).first == ox::Point{10, 2});
    REQUIRE(diff.at(1).second ==
            (U'a' | fg(ox::Color::Blue) | ox::Trait::Underline));
    REQUIRE(diff.at(2).first == ox::Point{3, 16});
    REQUIRE(diff.at(2).second == (U'x' | bg(ox::Color::Blue)));
}
