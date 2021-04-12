#include <clocale>

#include <catch2/catch.hpp>

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/pipe.hpp>

static void init() { std::setlocale(LC_ALL, "en_US.UTF-8"); }

TEST_CASE("Empty Glyph_string function calls", "[Glyph_string]")
{
    init();
    using ox::Color;
    using ox::Trait;

    auto g = ox::Glyph_string{};
    CHECK(g.empty());
    CHECK(g.size() == 0);
    CHECK(g.length() == 0);
    CHECK(g.u32str().empty());
    CHECK(g.str().empty());
    g.add_traits(Trait::Bold);
    g.add_traits(Trait::Italic);
    g.add_traits(Trait::Standout | Trait::Underline);
    g.add_color(bg(Color::Blue));
    g.add_color(fg(Color::Yellow));
    g.remove_traits(Trait::Bold);
    g.remove_traits(Trait::Invisible);
    g.remove_foreground();
    g.remove_background();
}

TEST_CASE("std::string cstr Glyph_string and function calls", "[Glyph_string]")
{
    init();

    using ox::Color;
    using ox::Trait;
    auto gs = ox::Glyph_string{"hello,ӵ World!🌎~"};
    REQUIRE(!gs.empty());
    CHECK(gs.size() == 16);
    CHECK(gs.length() == 16);
    CHECK(gs.u32str().size() == 16);
    CHECK(gs.u32str() == U"hello,ӵ World!🌎~");
    CHECK(gs.str().size() == 20);
    CHECK(gs.str() == "hello,ӵ World!🌎~");
    gs.add_traits(Trait::Bold);
    gs.add_traits(Trait::Italic);
    gs.add_traits(Trait::Standout | Trait::Underline);
    gs.add_color(bg(Color::Blue));
    gs.add_color(fg(Color::Yellow));
    gs.remove_traits(Trait::Bold);
    gs.remove_traits(Trait::Invisible);
    CHECK(gs[4].symbol == U'o');
    CHECK(gs[6].symbol == U'ӵ');
    CHECK(gs.front().symbol == U'h');
    CHECK(gs.back().symbol == U'~');

    for (auto glyph : gs) {
        CHECK(glyph.brush.background == Color::Blue);
        CHECK(glyph.brush.foreground == Color::Yellow);
        CHECK(glyph.brush.traits ==
              (Trait::Italic | Trait::Standout | Trait::Underline));
    }

    gs.remove_foreground();
    gs.remove_background();
    for (auto glyph : gs) {
        CHECK(glyph.brush.background == Color::Background);
        CHECK(glyph.brush.foreground == Color::Foreground);
    }

    auto gs2 = ox::Glyph_string{"Hello, World!", Trait::Bold | Trait::Underline,
                                bg(Color::Red)};
    REQUIRE(!gs2.empty());
    CHECK(gs2.front().brush.background == Color::Red);
    CHECK(gs2.front().brush.foreground == Color::Foreground);
    CHECK(gs2.front().brush.traits == (Trait::Bold | Trait::Underline));

    auto gs3 = ox::Glyph_string{"Sine"};
    CHECK(gs3.size() == 4);
    CHECK(gs3.str().size() == 4);
    CHECK(gs3.u32str().size() == 4);
}

TEST_CASE("std::u32string cstr Glyph_string and function calls",
          "[Glyph_string]")
{
    init();
    using ox::Color;
    using ox::Trait;
    auto gs = ox::Glyph_string{U"Ɣѝ₪⌛aǺӜ🥬▚😳ㅎꂆ"};
    REQUIRE(!gs.empty());
    CHECK(gs.size() == 12);
    CHECK(gs.length() == 12);
    CHECK(gs.u32str().size() == 12);
    CHECK(gs.u32str() == U"Ɣѝ₪⌛aǺӜ🥬▚😳ㅎꂆ");
    CHECK(gs.str().size() == 32);
    CHECK(gs.str() == "Ɣѝ₪⌛aǺӜ🥬▚😳ㅎꂆ");
    gs.add_traits(Trait::Bold);
    gs.add_traits(Trait::Italic);
    gs.add_traits(Trait::Standout | Trait::Underline);
    gs.add_color(bg(Color::Blue));
    gs.add_color(fg(Color::Yellow));
    gs.remove_traits(Trait::Bold);
    gs.remove_traits(Trait::Invisible);
    CHECK(gs[5].symbol == U'Ǻ');
    CHECK(gs.front().symbol == U'Ɣ');
    CHECK(gs.back().symbol == U'ꂆ');

    for (auto glyph : gs) {
        CHECK(glyph.brush.background == Color::Blue);
        CHECK(glyph.brush.foreground == Color::Yellow);
        CHECK(glyph.brush.traits ==
              (Trait::Italic | Trait::Standout | Trait::Underline));
    }

    gs.remove_foreground();
    gs.remove_background();
    for (auto glyph : gs) {
        CHECK(glyph.brush.background == Color::Background);
        CHECK(glyph.brush.foreground == Color::Foreground);
    }

    auto gs2 = ox::Glyph_string{U"Hello, World!",
                                Trait::Bold | Trait::Underline, bg(Color::Red)};
    REQUIRE(!gs2.empty());
    CHECK(gs2.front().brush.background == Color::Red);
    CHECK(gs2.front().brush.foreground == Color::Foreground);
    CHECK(gs2.front().brush.traits == (Trait::Bold | Trait::Underline));
}

TEST_CASE("Traits pipe Glyph_string construction", "[Glyph_string]")
{
    init();
    using ox::Trait;

    {
        // Glyph_string r-value
        auto gs =
            (ox::Glyph_string{U"Foo"} | Trait::Underline) | Trait::Standout;
        CHECK(gs.u32str() == U"Foo");
        auto const traits = Trait::Underline | Trait::Standout;
        for (auto g : gs)
            CHECK(g.brush.traits == traits);

        // L-Value reference
        gs | Trait::Inverse;
        auto const traits2 = traits | Trait::Inverse;
        for (auto g : gs)
            CHECK(g.brush.traits == traits2);

        gs | ox::pipe::discard(Trait::Standout | Trait::Inverse);
        for (auto g : gs)
            CHECK(g.brush.traits == Trait::Underline);
    }
    {
        // String Literal
        auto gs  = "Foo😳" | Trait::Underline;
        auto gs2 = "Foo😳" | (Trait::Underline | Trait::Blink);

        CHECK(gs.str() == "Foo😳");
        for (auto g : gs)
            CHECK(g.brush.traits == Trait::Underline);

        CHECK(gs2.str() == "Foo😳");
        auto const traits = Trait::Underline | Trait::Blink;
        for (auto g : gs2)
            CHECK(g.brush.traits == traits);
    }
    {
        // U32String Literal
        auto gs  = U"Foo😳" | Trait::Underline;
        auto gs2 = U"Foo😳" | (Trait::Underline | Trait::Blink);

        CHECK(gs.u32str() == U"Foo😳");
        for (auto g : gs)
            CHECK(g.brush.traits == Trait::Underline);

        CHECK(gs2.u32str() == U"Foo😳");
        auto const traits = Trait::Underline | Trait::Blink;
        for (auto g : gs2)
            CHECK(g.brush.traits == traits);
    }
}

TEST_CASE("Color pipe Glyph_string construction", "[Glyph_string]")
{
    init();
    using ox::Color;
    {
        // Glyph_string r-value
        auto gs = (ox::Glyph_string{U"Foo"} | bg(Color::Brown) |
                   fg(Color::Dark_blue));
        CHECK(gs.u32str() == U"Foo");
        for (auto g : gs) {
            CHECK(g.brush.background == Color::Brown);
            CHECK(g.brush.foreground == Color::Dark_blue);
        }

        // L-Value reference
        gs | bg(Color::Background);
        gs | fg(Color::Foreground);
        for (auto g : gs) {
            CHECK(g.brush.background == Color::Background);
            CHECK(g.brush.foreground == Color::Foreground);
        }
    }

    {
        // String Literal
        auto gs = "Foo😳" | fg(Color::Orange);

        CHECK(gs.str() == "Foo😳");
        for (auto g : gs)
            CHECK(g.brush.foreground == Color::Orange);

        auto gs2 = "Foo😳" | bg(Color::Light_green);
        CHECK(gs2.str() == "Foo😳");
        for (auto g : gs2)
            CHECK(g.brush.background == Color::Light_green);
    }
    {
        // U32String Literal
        auto gs = U"Foo😳" | fg(Color::Orange);

        CHECK(gs.u32str() == U"Foo😳");
        for (auto g : gs)
            CHECK(g.brush.foreground == Color::Orange);

        auto gs2 = U"Foo😳" | bg(Color::Light_green);
        CHECK(gs2.u32str() == U"Foo😳");
        for (auto g : gs2)
            CHECK(g.brush.background == Color::Light_green);
    }
}

TEST_CASE("Brush pipe Glyph_string construction", "[Glyph_string]")
{
    init();
    using ox::Brush;
    using ox::Color;
    using ox::Trait;
    auto const brush  = Brush{bg(Color::Brown), Trait::Bold | Trait::Inverse};
    auto const brush2 = Brush{Trait::Underline, fg(Color::Violet), Trait::Dim,
                              bg(Color::Green)};

    {
        // Glyph_string r-value
        auto gs = ox::Glyph_string{U"Foo"} | brush;
        CHECK(gs.u32str() == U"Foo");
        for (auto g : gs)
            CHECK(g.brush == brush);

        // L-Value reference
        gs | brush2;
        for (auto g : gs)
            CHECK(g.brush == brush2);
    }
    {
        // String Literal
        auto gs = "Foo😳" | brush;

        CHECK(gs.str() == "Foo😳");
        for (auto g : gs)
            CHECK(g.brush == brush);
    }
    {
        // U32String Literal
        auto gs = U"Foo😳" | brush;

        CHECK(gs.u32str() == U"Foo😳");
        for (auto g : gs)
            CHECK(g.brush == brush);
    }
}
