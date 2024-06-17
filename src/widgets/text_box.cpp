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

    auto indices = std::vector<std::size_t>{};
    auto start   = std::size_t{0};

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
    for (auto i = std::size_t{0}; i + 1 < tb.line_start_indices.size(); ++i) {
        if (i >= (std::size_t)c.size.height) {
            break;
        }
        auto const start = tb.line_start_indices[i];
        auto const end   = tb.line_start_indices[i + 1];
        auto line        = std::string_view{tb.text}.substr(start, end - start);
        if (line.back() == '\n') {
            line.remove_suffix(1);
        }
        ox::Painter{c}[{0, (int)i}] << line;
    }
    if (!tb.line_start_indices.empty()) {
        auto const start = tb.line_start_indices.back();
        auto line        = std::string_view{tb.text}.substr(start);
        if (line.back() == '\n') {
            line.remove_suffix(1);
        }
        ox::Painter{c}[{0, (int)tb.line_start_indices.size() - 1}] << line;
    }
}

auto resize(TextBox& tb, Area new_size) -> void
{
    tb.line_start_indices = calculate_line_start_indices(
        tb.text, tb.word_wrap, (std::size_t)new_size.width);
    tb.width = (std::size_t)new_size.width;
}

auto key_press(TextBox& tb, Key k) -> void
{
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
    tb.line_start_indices =
        calculate_line_start_indices(tb.text, tb.word_wrap, tb.width);
}

}  // namespace ox::widgets