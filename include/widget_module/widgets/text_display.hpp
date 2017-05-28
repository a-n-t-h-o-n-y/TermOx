#ifndef WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP
#define WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP

#include "painter_module/attribute.hpp"
#include "painter_module/brush.hpp"
#include "painter_module/glyph.hpp"
#include "painter_module/glyph_string.hpp"
#include "widget_module/coordinates.hpp"
#include "widget_module/widget.hpp"
#include <aml/signals/signals.hpp>
#include <cstddef>
#include <vector>
namespace cppurses {
class Paint_event;
class Resize_event;

class Text_display : public Widget {
   public:
    explicit Text_display(Glyph_string content = "");

    // String Modification
    sig::Slot<void(Glyph_string)> set_text;
    sig::Slot<void(Glyph_string, std::size_t)> insert;
    sig::Slot<void(Glyph_string)> append;
    sig::Slot<void(std::size_t)> erase;
    sig::Slot<void()> pop_back;
    sig::Slot<void()> clear;

    // Scrolling
    sig::Slot<void()> scroll_up;
    sig::Slot<void()> scroll_down;
    sig::Slot<void(std::size_t)> scroll_up_n;
    sig::Slot<void(std::size_t)> scroll_down_n;

    // Options
    sig::Slot<void()> enable_word_wrap;
    sig::Slot<void()> disable_word_wrap;
    sig::Slot<void()> toggle_word_wrap;
    sig::Slot<void(bool)> set_word_wrap;

    sig::Slot<void(Attribute)> set_attribute;
    sig::Slot<void(Attribute)> remove_attribute;

    // Query Functions
    std::size_t row_length(std::size_t y) const;
    std::size_t display_height() const;
    std::size_t index_at(Coordinates position) const;
    std::size_t index_at(std::size_t x, std::size_t y) const;
    Coordinates display_position(std::size_t index) const;
    Glyph_string contents() const { return contents_; }
    Glyph glyph_at(std::size_t index) const { return contents_.at(index); }
    std::size_t contents_size() const { return contents_.size(); }
    bool contents_empty() const { return contents_.empty(); }
    bool does_word_wrap() const { return word_wrap_; }

    // Signals
    sig::Signal<void(std::size_t n)> scrolled_up;
    sig::Signal<void(std::size_t n)> scrolled_down;
    sig::Signal<void()> scrolled;
    sig::Signal<void(const Glyph_string&)> text_changed;

   protected:
    bool paint_event(const Paint_event& event) override;
    bool resize_event(const Resize_event& event) override;
    void scroll_up_(std::size_t n);
    void scroll_down_(std::size_t n);
    void set_text_(Glyph_string string);
    void insert_(Glyph_string string, std::size_t index);
    void append_(Glyph_string string);
    void erase_(std::size_t index, std::size_t length = Glyph_string::npos);
    void pop_back_();
    void clear_();
    void enable_word_wrap_(bool enable = true);
    void disable_word_wrap_(bool disable = true);

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

    std::vector<line_info> display_state_{line_info{0, 0}};

    std::size_t top_line_{0};
    bool word_wrap_ = true;
    Glyph_string contents_;
    Brush incoming_brush_{this->brush()};

    void update_display(std::size_t from_line = 0);
    void initialize();
};

}  // namespace cppurses
#endif  // WIDGET_MODULE_WIDGETS_TEXT_DISPLAY_HPP
