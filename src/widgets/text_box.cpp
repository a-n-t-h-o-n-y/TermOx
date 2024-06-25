#include <termox/widgets/text_box.hpp>

#include <termox/core.hpp>
#include <termox/widgets/widget.hpp>

namespace {

[[nodiscard]] auto calculate_line_start_indices(std::string const& content,
                                                bool word_wrap,
                                                std::size_t width)
    -> std::vector<std::size_t>
{
    (void)word_wrap;  // TODO

    if (width == 0) {
        return {0};
    }

    auto indices = std::vector<std::size_t>{};
    auto start = std::size_t{0};

    for (auto i = std::size_t{0}; i < content.size(); ++i) {
        if (content[i] == '\n') {
            indices.push_back(start);
            start = i + 1;
        }
        else if (i - start == width) {
            indices.push_back(start);
            start = i;
        }
    }

    if (start < content.size()) {
        indices.push_back(start);
    }

    return indices;
}

}  // namespace

namespace ox::widgets {

auto paint(TextBox const& tb, Canvas c) -> void
{
    auto const line_start_indices =
        calculate_line_start_indices(tb.text, tb.word_wrap, tb.width);

    // first lines
    for (auto i = std::size_t{0};
         (i + 1 < line_start_indices.size()) && (i < (std::size_t)c.size.height); ++i) {
        auto const start = line_start_indices[i];
        auto const end = line_start_indices[i + 1];
        auto line = std::string_view{tb.text}.substr(start, end - start);
        if (line.back() == '\n') {
            line.remove_suffix(1);
        }
        ox::Painter{c}[{0, (int)i}] << line;
    }
    // last line
    if (!line_start_indices.empty() &&
        (line_start_indices.size() <= (std::size_t)c.size.height)) {
        auto const start = line_start_indices.back();
        auto line = std::string_view{tb.text}.substr(start);
        if (line.back() == '\n') {
            line.remove_suffix(1);
        }
        ox::Painter{c}[{0, (int)line_start_indices.size() - 1}] << line;
    }
}

auto resize(TextBox& tb, Area new_size) -> void
{
    // In case user modified text or cursor_index.
    tb.cursor_index = std::min(tb.cursor_index, tb.text.size());

    tb.width = (std::size_t)new_size.width;
}

auto key_press(TextBox& tb, Key k) -> void
{
    // In case user modified text or cursor_index.
    tb.cursor_index = std::min(tb.cursor_index, tb.text.size());

    switch (k) {
        case Key::Backspace:
            if (!tb.text.empty() && tb.cursor_index <= tb.text.size()) {
                --tb.cursor_index;
                tb.text.erase(tb.cursor_index, 1);
            }
            break;
        case Key::Enter: tb.text.insert(tb.cursor_index++, 1, '\n'); break;
        case Key::Delete:
            if (tb.cursor_index < tb.text.size()) {
                tb.text.erase(tb.cursor_index, 1);
            }
            break;
        case Key::ArrowLeft:
            if (tb.cursor_index > 0) {
                --tb.cursor_index;
            }
            break;
        case Key::ArrowRight:
            if (tb.cursor_index < tb.text.size()) {
                ++tb.cursor_index;
            }
            break;
        case Key::Home: tb.cursor_index = 0; break;
        case Key::End: tb.cursor_index = tb.text.size(); break;
        default:
            if (auto const c = key_to_char(k); c != '\0') {
                tb.text.insert(tb.cursor_index, 1, c);
                ++tb.cursor_index;
            }
            break;
    }
}

auto cursor(TextBox const& /*tb*/) -> ox::Terminal::Cursor
{
    // TODO make a real implementation.
    return Point{.x = 5, .y = 2};
}

}  // namespace ox::widgets