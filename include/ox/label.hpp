#pragma once

#include <cstdint>
#include <format>
#include <locale>
#include <string>
#include <string_view>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/text_view.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Draw a clipped, aligned label without filling around it. Vertical labels stack
 * graphemes; alignment is along the line and the cross axis is centered. */
void put_label(Canvas c,
               std::string_view text,
               Align align,
               Style style,
               bool vertical = false);
void paint_label(Canvas c, std::string_view text, Align align, Style style);

}  // namespace ox

namespace ox {

/**
 * A single horizontal line of text.
 */
class Label : public Widget {
   public:
    using Widget::handle;

    struct Options {
        std::string text = "";
        Align align = Align::Center;
        Style style = {};
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::fixed(1);
    } static const init;

   public:
    std::string text;
    Align align;
    Style style;

    Label(Options x = init);

    Label(std::string text_);

    Label(char const* text_);

   public:
    auto set_text(std::string value) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas c) -> Cursor override;

   private:
    mutable TextLayout measure_layout_;
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
    using Widget::handle;

    struct Options {
        NumberType value = 0;
        Align align = Align::Center;
        Style style = {};
        std::locale locale = std::locale{};
        std::string format = "{}";
        std::string prefix = {};
        std::string suffix = {};
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::fixed(1);
    } inline static const init = {};

   public:
    NumberType value;
    Align align;
    Style style;
    std::locale locale;
    std::string format;
    std::string prefix;
    std::string suffix;

    BasicNumberLabel(Options x = init)
        : Widget{FocusPolicy::None, x.width_policy, x.height_policy},
          value{std::move(x.value)},
          align{std::move(x.align)},
          style{std::move(x.style)},
          locale{std::move(x.locale)},
          format{std::move(x.format)},
          prefix{std::move(x.prefix)},
          suffix{std::move(x.suffix)}
    {}

   public:
    auto set_value(NumberType next) -> bool
    {
        if (value == next) { return false; }
        value = next;
        request_layout();
        request_repaint();
        return true;
    }
    auto measure(Area available, esc::WidthPolicy policy = {}) const -> Area override
    {
        return measure_text(prefix + std::vformat(locale, format, std::make_format_args(value)) + suffix,
                            available, TextWrap::None, policy);
    }
    auto paint(Canvas c) -> Cursor override
    {
        auto const text = prefix + std::vformat(locale, format, std::make_format_args(value)) + suffix;
        paint_label(c, text, align, appearance(style));

        return {};
    }
};

using IntegerLabel = BasicNumberLabel<int>;
using FloatLabel = BasicNumberLabel<float>;

}  // namespace ox