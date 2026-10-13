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

    auto const unicode = esc::utf8_to_utf32(text);
    auto end = std::size_t{0};
    auto columns = std::size_t{0};
    for (auto cluster : esc::graphemes(unicode)) {
        auto const width = esc::grapheme_width(cluster, c.width_policy());
        if (!width || *width == 0) {
            throw std::invalid_argument{"Label requires printable text"};
        }
        if (*width > (std::size_t)c.size().width - columns) { break; }
        columns += *width;
        end += cluster.size();
    }
    auto const x = find_align_offset(align, c.size().width, (int)columns);
    put(c, {x, c.size().height / 2},
        esc::glyphs(std::u32string_view{unicode}.substr(0, end), style));
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