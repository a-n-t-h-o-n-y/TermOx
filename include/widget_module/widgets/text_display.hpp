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
    void set_text(const Glyph_string& string);
    void insert(const Glyph_string& string, std::size_t index);
    void append(const Glyph_string& string);
    void erase(std::size_t index, std::size_t length = Glyph_string::npos);
    void pop_back();
    void clear();

    // Movement
    virtual void scroll_up(std::size_t n = 1);
    virtual void scroll_down(std::size_t n = 1);

    // Query Functions
    std::size_t row_length(std::size_t y) const;
    std::size_t display_height() const;
    std::size_t index_at(Coordinate position) const;
    std::size_t index_at(std::size_t x, std::size_t y) const;
    Coordinate display_position(std::size_t index) const;

    Glyph_string contents() const { return contents_; }
    Glyph glyph_at(std::size_t index) const { return contents_.at(index); }
    std::size_t contents_size() const { return contents_.size(); }
    bool contents_empty() const { return contents_.empty(); }
    bool does_word_wrap() const { return word_wrap_; }

    // Options
    void enable_word_wrap(bool enable = true);
    void disable_word_wrap(bool disable = true);

   protected:
    bool paint_event(const Paint_event& event) override;
    bool resize_event(const Resize_event& event) override;

    std::size_t line_at(std::size_t index) const;
    std::size_t top_line() const;
    std::size_t bottom_line() const;
    std::size_t last_line() const;

    std::size_t first_index_at(std::size_t line) const;
    std::size_t last_index_at(std::size_t line) const;

    std::size_t line_length(std::size_t line) const;
    std::size_t end_index() const;

   private:
    struct line_info {
        std::size_t start_index;
        std::size_t length;
    };

    std::vector<line_info> display_state_;

    std::size_t top_line_{0};
    bool word_wrap_ = true;
    Glyph_string contents_;

    void update_display(std::size_t from_line = 0);
};

}  // namespace twf
#endif  // WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP
