#include <ox/label.hpp>

#include <cstddef>
#include <cstdint>
#include <locale>
#include <stdexcept>
#include <string>
#include <string_view>

#include <esc/text.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/put.hpp>

namespace ox::detail {
using namespace ox;

// Used by both Label and BasicNumberLabel
void paint_label(Canvas c, std::string_view text, Align align, Style style)
{
    fill(c, style);

    auto unicode_str = esc::utf8_to_utf32(text);
    if (std::ssize(unicode_str) > c.size.width) {
        unicode_str.resize((std::size_t)c.size.width);
    }
    auto const x = find_align_offset(align, c.size.width, (int)unicode_str.size());
    auto const y = c.size.height / 2;
    for (auto i = 0; i < (int)unicode_str.size(); ++i) {
        auto ch = unicode_str[(std::size_t)i];
        c[{.x = x + i, .y = y}].symbol = ch;
    }
}

}  // namespace ox::detail

namespace ox {

Label::Options const Label::init = {};

Label::Label(Options x)
    : Widget{FocusPolicy::None, x.size_policy},
      text{std::move(x.text)},
      align{std::move(x.align)},
      style{std::move(x.style)}
{}

Label::Label(std::string text_)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{std::move(text_)},
      align{Align::Center},
      style{}
{}

Label::Label(char const* text_)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      text{text_},
      align{Align::Center},
      style{}
{}

void Label::paint(Canvas c) { detail::paint_label(c, text, align, style); }

// -------------------------------------------------------------------------------------

auto digit_separator_locale(std::int8_t spacing, std::locale init_locale) -> std::locale
{
    struct facet : std::numpunct<char> {
        int digit_spacing;

        facet(int spacing) : digit_spacing{spacing} {}

        auto do_grouping() const -> std::string override
        { return std::string(1, static_cast<char>(digit_spacing)); }
    };
    if (spacing < 0) {
        throw std::invalid_argument{
            "digit_separator_locale: spacing must be non-negative"};
    }
    return std::locale{init_locale, new facet{spacing}};
}

}  // namespace ox