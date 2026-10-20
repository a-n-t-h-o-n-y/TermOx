/** @file
 * Styled, aligned horizontal and vertical UTF-8 labels.
 */
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
 * graphemes; alignment is along the line and the cross axis is centered.
 * Draw only the fitting prefix, without splitting graphemes. A vertical grapheme
 * wider than the view ends that prefix, even if later text would fit.
 * Text must be valid UTF-8 with printable graphemes of known, positive width;
 * tabs, line breaks, and control characters are rejected.
 */
void put_label(Canvas c,
               std::string_view text,
               Align align,
               Style style,
               bool vertical = false);
/** Fill the clipped view with spaces in the supplied style, then draw with put_label().
 */
void paint_label(Canvas c, std::string_view text, Align align, Style style,
                 bool vertical = false);
/** Natural extent of printable, single-line text, using reusable Unicode analysis.
 * Horizontal extent is terminal columns by one row. Vertical extent is the widest
 * grapheme by the grapheme count; empty vertical text measures 0 by 0.
 * Text has the same validity requirements as put_label().
 */
auto measure_label(TextLayout& cache, std::string_view text, bool vertical = false,
                   esc::WidthPolicy policy = {}) -> Area;

}  // namespace ox

namespace ox {

/**
 * One horizontal line or vertical stack of complete graphemes.
 */
class Label : public Widget {
   public:
    using Widget::handle;

    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Owned printable label text; use set_text() to invalidate measurement and repaint. */
        std::string text = "";
        /** Stack graphemes vertically when true; use set_vertical() to update axis policies. */
        bool vertical = false;
        /** Text alignment along its display direction. */
        Align align = Align::Center;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Semantic theme role used when drawing colors are unspecified. */
        Surface role = Surface::Canvas;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = vertical ? SizePolicy{.minimum = 1, .flexibility = 0}
                                           : SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = vertical ? SizePolicy::flex() : SizePolicy::fixed(1);
    } static const init; ///< Default options used when no explicit configuration is supplied.

   public:
    /** Owned printable label text; use set_text() to invalidate measurement and repaint. */
    std::string text;
    /** Stack graphemes vertically when true; use set_vertical() to update axis policies. */
    bool vertical;
    /** Text alignment along its display direction. */
    Align align;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Semantic theme role used when drawing colors are unspecified. */
    Surface role;

    /** Create printable label text and its axis policies; validate text during
     * measurement/painting.
     */
    Label(Options x = init);

    /** Own UTF-8 text with the default horizontal label options. */
    Label(std::string text_);

    /** Copy null-terminated UTF-8 text with default horizontal options. */
    Label(char const* text_);

   public:
    /** Replace text and request layout/repaint only when it changes; return whether it
     * changed.
     */
    auto set_text(std::string value) -> bool;
    /** Change text direction, update policies that still match the previous direction's
     * defaults, and request layout/repaint if changed. Customized policies are preserved.
     * Editing vertical directly followed by refresh() performs the same policy update.
     */
    auto set_vertical(bool value) -> bool;
    void refresh() override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas c) -> Cursor override;

   private:
    mutable TextLayout measure_layout_;
    bool policy_vertical_;
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

    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Numeric value formatted for display; set_value() requests layout and repaint
         * on change.
         */
        NumberType value = 0;
        /** Text alignment along its display direction. */
        Align align = Align::Center;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Semantic theme role used when drawing colors are unspecified. */
        Surface role = Surface::Canvas;
        /** Locale used for numeric formatting and parsing where supported. */
        std::locale locale = std::locale{};
        /** std::format-compatible numeric format string; invalid formats can throw when
         * formatting is evaluated.
         */
        std::string format = "{}";
        /** Display text preceding the formatted number. */
        std::string prefix = {};
        /** Display text following the formatted number. */
        std::string suffix = {};
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::fixed(1);
    } inline static const init = {}; ///< Default options used when no explicit configuration is supplied.

   public:
    /** Numeric value formatted for display; set_value() requests layout and repaint on
     * change.
     */
    NumberType value;
    /** Text alignment along its display direction. */
    Align align;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Semantic theme role used when drawing colors are unspecified. */
    Surface role;
    /** Locale used for numeric formatting and parsing where supported. */
    std::locale locale;
    /** std::format-compatible numeric format string; invalid formats can throw when
     * formatting is evaluated.
     */
    std::string format;
    /** Display text preceding the formatted number. */
    std::string prefix;
    /** Display text following the formatted number. */
    std::string suffix;

    /** Own the numeric value and formatting configuration; formatting is evaluated by
     * measure() and paint().
     */
    BasicNumberLabel(Options x = init)
        : Widget{FocusPolicy::None, x.width_policy, x.height_policy},
          value{std::move(x.value)},
          align{std::move(x.align)},
          style{std::move(x.style)},
          role{x.role},
          locale{std::move(x.locale)},
          format{std::move(x.format)},
          prefix{std::move(x.prefix)},
          suffix{std::move(x.suffix)}
    {}

   public:
    /** Store a changed value and request layout/repaint; formatting errors surface at
     * measurement or painting.
     */
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
        paint_label(c, text, align, appearance(style, role));

        return {};
    }
};

/** Numeric label storing and formatting an int. */
using IntegerLabel = BasicNumberLabel<int>;
/** Numeric label storing and formatting a float. */
using FloatLabel = BasicNumberLabel<float>;

}  // namespace ox
