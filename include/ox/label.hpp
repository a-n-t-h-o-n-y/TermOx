#pragma once

#include <cstdint>
#include <format>
#include <locale>
#include <string>
#include <string_view>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox::detail {

void paint_label(Canvas c, std::string_view text, Align align, Brush brush);

}  // namespace ox::detail

namespace ox {

/**
 * A single horizontal line of text.
 */
class Label : public Widget {
    struct Options {
        std::string text = "";
        Align align = Align::Center;
        Brush brush = {};
    } static const init;

   public:
    std::string text;
    Align align;
    Brush brush;

    Label(Options x = init);

   public:
    void paint(Canvas c) override;
};

// -------------------------------------------------------------------------------------

/**
 * Create a locale that inserts a digit separator character.
 * @param spacing The number of digits between separators.
 * @param init_locale The locale to base the new locale on.
 * @throws std::invalid_argument if spacing is negative.
 */
[[nodiscard]] auto digit_separator_locale(std::int8_t spacing = 3,
                                          std::locale init_locale = std::locale{})
    -> std::locale;

/**
 * A single horizontal line of text that displays an integer or floating point value.
 * @details The number is formatted using std::format and std::locale.
 * https://en.cppreference.com/w/cpp/utility/format/spec
 * Digit separators can be set by using the provided digit_separator_locale() function.
 */
template <typename NumberType>
class BasicNumberLabel : public Widget {
   public:
    struct Options {
        NumberType value = 0;
        Align align = Align::Center;
        Brush brush = {};
        std::locale locale = std::locale{};
        std::string format = "{}";
    } inline static const init = {};

   public:
    NumberType value;
    Align align;
    Brush brush;
    std::locale locale;
    std::string format;

    BasicNumberLabel(Options x = init)
        : Widget{FocusPolicy::None, SizePolicy::flex()},
          value{std::move(x.value)},
          align{std::move(x.align)},
          brush{std::move(x.brush)},
          locale{std::move(x.locale)},
          format{std::move(x.format)}
    {}

   public:
    void paint(Canvas c) override
    {
        auto const text = std::vformat(locale, format, std::make_format_args(value));
        detail::paint_label(c, text, align, brush);
    }
};

using IntegerLabel = BasicNumberLabel<int>;
using FloatLabel = BasicNumberLabel<float>;

}  // namespace ox