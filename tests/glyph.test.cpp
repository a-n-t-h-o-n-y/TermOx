#include <deque>
#include <list>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <termox/glyph.hpp>

using namespace ox;

TEST_CASE(
    "Operator|(Character, ColorBG) works with all character literal types",
    "[Glyph]")
{
    SECTION("char")
    {
        auto const glyph = 'A' | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'A');
    }

    SECTION("signed char")
    {
        auto const glyph = static_cast<signed char>('B') | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'B');
    }

    SECTION("unsigned char")
    {
        auto const glyph =
            static_cast<unsigned char>('C') | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'C');
    }

    SECTION("wchar_t")
    {
        auto const glyph = L'D' | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'D');
    }

    SECTION("char8_t")
    {
        auto const glyph = u8'E' | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'E');
    }

    SECTION("char16_t")
    {
        auto const glyph = u'F' | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'F');
    }

    SECTION("char32_t")
    {
        auto const glyph = U'G' | bg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'G');
    }
}

TEST_CASE(
    "Operator|(Character, ColorFG) works with all character literal types",
    "[Glyph]")
{
    SECTION("char")
    {
        auto const glyph = 'A' | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'A');
    }

    SECTION("signed char")
    {
        auto const glyph = static_cast<signed char>('B') | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'B');
    }

    SECTION("unsigned char")
    {
        auto const glyph =
            static_cast<unsigned char>('C') | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'C');
    }

    SECTION("wchar_t")
    {
        auto const glyph = L'D' | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'D');
    }

    SECTION("char8_t")
    {
        auto const glyph = u8'E' | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'E');
    }

    SECTION("char16_t")
    {
        auto const glyph = u'F' | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'F');
    }

    SECTION("char32_t")
    {
        auto const glyph = U'G' | fg(ColorIndex::Red);
        REQUIRE(glyph.symbol == U'G');
    }
}

TEST_CASE("Operator|(Character, Trait) works with all character literal types",
          "[Glyph]")
{
    SECTION("char")
    {
        auto const glyph = 'A' | Trait::Bold;
        REQUIRE(glyph.symbol == U'A');
    }

    SECTION("signed char")
    {
        auto const glyph = static_cast<signed char>('B') | Trait::Bold;
        REQUIRE(glyph.symbol == U'B');
    }

    SECTION("unsigned char")
    {
        auto const glyph = static_cast<unsigned char>('C') | Trait::Bold;
        REQUIRE(glyph.symbol == U'C');
    }

    SECTION("wchar_t")
    {
        auto const glyph = L'D' | Trait::Bold;
        REQUIRE(glyph.symbol == U'D');
    }

    SECTION("char8_t")
    {
        auto const glyph = u8'E' | Trait::Bold;
        REQUIRE(glyph.symbol == U'E');
    }

    SECTION("char16_t")
    {
        auto const glyph = u'F' | Trait::Bold;
        REQUIRE(glyph.symbol == U'F');
    }

    SECTION("char32_t")
    {
        auto const glyph = U'G' | Trait::Bold;
        REQUIRE(glyph.symbol == U'G');
    }
}

template <typename Container>
auto test_glyphstring_pipe_ops_lval() -> void
{
    auto glyphs = Container(5, {U'a', {.traits = Trait::Bold}});

    SECTION("ColorBG")
    {
        glyphs | bg(ColorIndex::Green);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.background = ColorIndex::Green,
                                    .traits     = Trait::Bold}});
        }
    }

    SECTION("ColorFG")
    {
        glyphs | fg(ColorIndex::Green);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.foreground = ColorIndex::Green,
                                    .traits     = Trait::Bold}});
        }
    }

    SECTION("Trait")
    {
        glyphs | Trait::Italic;

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph ==
                    Glyph{U'a', {.traits = Trait::Bold | Trait::Italic}});
        }
    }

    SECTION("RemoveTrait")
    {
        glyphs | Trait::Bold | remove_trait(Trait::Bold);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a'});
        }
    }

    SECTION("ColorBG, ColorFG, Trait")
    {
        glyphs | bg(ColorIndex::Green) | fg(ColorIndex::Red) | Trait::Dim;

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.background = ColorIndex::Green,
                                    .foreground = ColorIndex::Red,
                                    .traits     = Trait::Bold | Trait::Dim}});
        }
    }
}

template <typename Container>
auto test_glyphstring_pipe_ops_rval() -> void
{
    SECTION("ColorBG")
    {
        auto const glyphs = Container(5, {U'a', {.traits = Trait::Bold}}) |
                            bg(ColorIndex::Green);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.background = ColorIndex::Green,
                                    .traits     = Trait::Bold}});
        }
    }

    SECTION("ColorFG")
    {
        auto const glyphs = Container(5, {U'a', {.traits = Trait::Bold}}) |
                            fg(ColorIndex::Green);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.foreground = ColorIndex::Green,
                                    .traits     = Trait::Bold}});
        }
    }

    SECTION("Trait")
    {
        auto const glyphs =
            Container(5, {U'a', {.traits = Trait::Bold}}) | Trait::Italic;

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph ==
                    Glyph{U'a', {.traits = Trait::Bold | Trait::Italic}});
        }
    }

    SECTION("RemoveTrait")
    {
        auto const glyphs = Container(5, {U'a', {.traits = Trait::Bold}}) |
                            Trait::Bold | remove_trait(Trait::Bold);

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a'});
        }
    }

    SECTION("ColorBG, ColorFG, Trait")
    {
        auto const glyphs = Container(5, {U'a', {.traits = Trait::Bold}}) |
                            bg(ColorIndex::Green) | fg(ColorIndex::Red) |
                            Trait::Dim;

        for (auto const& glyph : glyphs) {
            REQUIRE(glyph == Glyph{U'a',
                                   {.background = ColorIndex::Green,
                                    .foreground = ColorIndex::Red,
                                    .traits     = Trait::Bold | Trait::Dim}});
        }
    }
}

TEST_CASE("Operator|(GlyphString, ...) where GlyphString is a std::vector",
          "[GlyphString]")
{
    test_glyphstring_pipe_ops_lval<std::vector<Glyph>>();
    test_glyphstring_pipe_ops_rval<std::vector<Glyph>>();
}

TEST_CASE("Operator|(GlyphString, ...) where GlyphString is a std::list",
          "[GlyphString]")
{
    test_glyphstring_pipe_ops_lval<std::list<Glyph>>();
    test_glyphstring_pipe_ops_rval<std::list<Glyph>>();
}

TEST_CASE("Operator|(GlyphString, ...) where GlyphString is a std::deque",
          "[GlyphString]")
{
    test_glyphstring_pipe_ops_lval<std::deque<Glyph>>();
    test_glyphstring_pipe_ops_rval<std::deque<Glyph>>();
}

template <typename StringType>
auto test_string_pipe_ops(StringType abc) -> void
{
    SECTION("ColorBG")
    {
        auto const gs = abc | bg(ColorIndex::Green);
        REQUIRE(gs.size() == 3);
        REQUIRE(gs[0] == Glyph{U'a', {.background = ColorIndex::Green}});
        REQUIRE(gs[1] == Glyph{U'b', {.background = ColorIndex::Green}});
        REQUIRE(gs[2] == Glyph{U'c', {.background = ColorIndex::Green}});
    }

    SECTION("ColorFG")
    {
        auto const gs = abc | fg(ColorIndex::Green);
        REQUIRE(gs.size() == 3);
        REQUIRE(gs[0] == Glyph{U'a', {.foreground = ColorIndex::Green}});
        REQUIRE(gs[1] == Glyph{U'b', {.foreground = ColorIndex::Green}});
        REQUIRE(gs[2] == Glyph{U'c', {.foreground = ColorIndex::Green}});
    }

    SECTION("Trait")
    {
        auto const gs = abc | Trait::Bold;
        REQUIRE(gs.size() == 3);
        REQUIRE(gs[0] == Glyph{U'a', {.traits = Trait::Bold}});
        REQUIRE(gs[1] == Glyph{U'b', {.traits = Trait::Bold}});
        REQUIRE(gs[2] == Glyph{U'c', {.traits = Trait::Bold}});
    }

    SECTION("RemoveTrait")
    {
        auto const gs = abc | Trait::Bold | remove_trait(Trait::Bold);
        REQUIRE(gs.size() == 3);
        REQUIRE(gs[0] == Glyph{U'a'});
        REQUIRE(gs[1] == Glyph{U'b'});
        REQUIRE(gs[2] == Glyph{U'c'});
    }

    SECTION("ColorBG, ColorFG, Trait")
    {
        auto const gs =
            abc | bg(ColorIndex::Green) | fg(ColorIndex::Red) | Trait::Dim;
        REQUIRE(gs.size() == 3);
        auto const brush = Brush{.background = ColorIndex::Green,
                                 .foreground = ColorIndex::Red,
                                 .traits     = Trait::Dim};
        REQUIRE(gs[0] == Glyph{U'a', brush});
        REQUIRE(gs[1] == Glyph{U'b', brush});
        REQUIRE(gs[2] == Glyph{U'c', brush});
    }
}

TEST_CASE("Operator|(StringType ...)", "[GlyphString]")
{
    SECTION("char32_t const*") { test_string_pipe_ops(U"abc"); }

    SECTION("std::u32string") { test_string_pipe_ops(std::u32string{U"abc"}); }

    // SECTION("char16_t const*") { test_string_pipe_ops(u"abc"); }

    // SECTION("std::u16string") { test_string_pipe_ops(std::u16string{u"abc"});
    // }

    // SECTION("char8_t const*") { test_string_pipe_ops(u8"abc"); }

    // SECTION("std::u8string") { test_string_pipe_ops(std::u8string{u8"abc"});
    // }

    // SECTION("wchar_t const*") { test_string_pipe_ops(L"abc"); }

    // SECTION("char const*") { test_string_pipe_ops("abc"); }

    // SECTION("std::string") { test_string_pipe_ops(std::string{"abc"}); }
}
