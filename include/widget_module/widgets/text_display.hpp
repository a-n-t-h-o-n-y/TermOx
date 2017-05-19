#ifndef WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP
#define WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP

#include "painter_module/glyph_string.hpp"
#include "widget_module/widget.hpp"
#include "widget_module/coordinate.hpp"
#include <cstddef>
#include <vector>

namespace twf {
class Paint_event;
class Resize_event;

class Text_display : public Widget {
   public:
    Text_display() = default;
    explicit Text_display(Glyph_string content);

    // String Modification
    void insert(const Glyph_string& string, std::size_t index);
    void append(const Glyph_string& string);
    void erase(std::size_t index, std::size_t length = Glyph_string::npos);
    void pop_back();
    void clear();

    // Movement
    void scroll_up(std::size_t n = 1);
    void scroll_down(std::size_t n = 1);

    // Query Functions
    std::size_t row_length(std::size_t y) const;
    std::size_t string_index(Coordinate position) const ;
    std::size_t string_index(std::size_t x, std::size_t y) const;
    Coordinate display_position(std::size_t index) const;

    // Options
    void enable_word_wrap(bool enable = true) { word_wrap_ = enable; }
    void disable_word_wrap(bool disable = true) { word_wrap_ = !disable; }

   protected:
    bool paint_event(const Paint_event& event) override;
    bool resize_event(const Resize_event& event) override;

   private:
    struct line_info {
        std::size_t start_index;
        std::size_t length;
    };

    std::vector<line_info> display_state_;

    std::size_t top_line_{0};
    bool word_wrap_ = false;
    Glyph_string contents_;

    void update_display(std::size_t from_line = 0);
    std::size_t line_at(std::size_t index) const;
    std::size_t index_at(std::size_t line) const;
    std::size_t top_line() const;
    std::size_t last_line() const;
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP
