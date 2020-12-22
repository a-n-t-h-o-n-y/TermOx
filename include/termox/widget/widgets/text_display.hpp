#ifndef TERMOX_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#define TERMOX_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#include <cstddef>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/brush.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/widget.hpp>

namespace ox {
struct Point;

/// Non-interactive box to display a given Glyph_string.
/** Provides operations to change the text, wrap words on spaces, change the
 *  alignment of the text and scroll through the text, among others. */
class Text_display : public Widget {
   public:
    /// Brush to be applied to all new incoming Glyphs, but not existing Glyphs.
    /** Widget::brush is applied after this Brush. */
    Brush insert_brush = this->brush;

    /// Emitted when text is scrolled up. Sends number of lines scrolled by.
    sl::Signal<void(std::size_t n)> scrolled_up;

    /// Emitted when text is scrolled down. Sends number of lines scrolled by.
    sl::Signal<void(std::size_t n)> scrolled_down;

    /// Emitted when text is scrolled, sends the top line.
    sl::Signal<void(std::size_t n)> scrolled_to;

    /// Emitted when contents are modified. Sends a reference to the contents.
    sl::Signal<void(Glyph_string const&)> contents_modified;

    /// Emitted when total line count changes.
    sl::Signal<void(std::size_t)> line_count_changed;

   public:
    /// Construct a Text_display with initial Glyph_string \p contents.
    /** By default, wraps text, and has left alignment. */
    explicit Text_display(Glyph_string contents = "")
        : contents_{std::move(contents)}
    {}

    /// Replace the current contents with \p text.
    /** Reset the display to show the first line at the top of the screen and
     *  the cursor at the first Glyph, or where the first Glyph would be. */
    void set_contents(Glyph_string text)
    {
        contents_ = std::move(text);
        this->update();
        top_line_ = 0;
        this->cursor.set_position({0, 0});
        contents_modified(contents_);
    }

    /// Inserts \p text starting at \p index into the current contents.
    /** Applys insert_brush to each Glyph added. Index can be one past the
     *  current length of the contents, to append. No-op if index is larger than
     *  one past the current length of contents. */
    void insert(Glyph_string text, std::size_t index);

    /// Inserts \p text to the end of the current contents.
    /** Applys insert_brush to each Glyph inserted. */
    void append(Glyph_string text);

    /// Remove Glyphs from contents starting at \p index, for \p length Glyphs.
    void erase(std::size_t index, std::size_t length = Glyph_string::npos);

    /// Remove the last Glyph from the current contents. No-op if this->empty();
    void pop_back();

    /// Remove all Glyphs from the this text display.
    /** this->empty() is true after call. */
    void clear();

    /// Set the Alignment, changing how the contents are displayed.
    /** Not fully implemented at the moment, Left alignment is currently
     *  supported. */
    void set_alignment(Align type)
    {
        alignment_ = type;
        this->update();
    }

    /// Return the currently used Alignment.
    auto alignment() const -> Align { return alignment_; }

    /// Scroll the display up by \p n lines.
    /** Tops out at the first line displayed at the top of the display. */
    virtual void scroll_up(std::size_t n = 1);

    /// Scroll the display down by \p n lines.
    /** Bottoms out at the last line displaying at the top of the display. */
    virtual void scroll_down(std::size_t n = 1);

    /// Enable word wrapping on spaces if \p enable is true, else disable it.
    void enable_word_wrap(bool enable = true)
    {
        word_wrap_enabled_ = enable;
        this->update();
    }

    /// Disable word wrapping if \disable is true, otherwise enable it.
    void disable_word_wrap(bool disable = true)
    {
        word_wrap_enabled_ = !disable;
        this->update();
    }

    /// Toggle word wrap between enabled/disabled depending on current state.
    void toggle_word_wrap()
    {
        word_wrap_enabled_ = !word_wrap_enabled_;
        this->update();
    }

    /// Return the length of the line at row \p y.
    /** Index 0 is the top of the Widget. */
    auto row_length(std::size_t y) const -> std::size_t
    {
        const auto line = this->top_line() + y;
        return this->line_length(line);
    }

    /// Return the number of lines currently displayed.
    auto display_height() const -> std::size_t
    {
        auto difference = 1 + this->last_line() - this->top_line();
        if (difference > this->height())
            difference = this->height();
        return difference;
    }

    /// Return the total number of lines in display_state_.
    auto line_count() const -> std::size_t { return display_state_.size(); }

    /// Set the top line, by row index.
    void set_top_line(std::size_t n)
    {
        if (n < display_state_.size())
            top_line_ = n;
        this->update();
    }

    /// Return the index into the contents from a physical Point on the Widget.
    /** If \p position is past any text on the corresponding line, then return
     *  index of the last Glyph on that line. If Point is past displayed lines,
     *  return the index of the last Glyph in contents. */
    auto index_at(Point position) const -> std::size_t;

    /// Return the position of the Glyph at \p index.
    /** If \p index is not currently displayed on screen, return the closest
     *  Glyph position to \p index that is displayed on screen. */
    auto display_position(std::size_t index) const -> Point;

    /// Return the entire contents of the Text_display.
    /** Provided as a non-const reference so contents can be modified without
     *  limitation from the Text_display interface. Be sure to call
     *  Text_display::update() after modifying the contents directly. */
    auto contents() -> Glyph_string& { return contents_; }

    /// Return the entire contents of the Text_display.
    auto contents() const -> Glyph_string const& { return contents_; }

    /// Return whether word wrapping is enabled.
    auto word_wrap_enabled() const -> bool { return word_wrap_enabled_; }

    /// Add call to Text_display::update_display() before posting Paint_event.
    void update() override
    {
        // This call to update_display is required here, and not in paint_event.
        // Could probably be refactored so this can be in paint_event, more
        // efficient...
        this->update_display();
        Widget::update();
    }

   protected:
    /// Paint the portion of contents that is currently visible on screen.
    auto paint_event() -> bool override;

    /// Return the line number that contains \p index.
    auto line_at(std::size_t index) const -> std::size_t;

    /// Return the line number that is being displayed at the top of the Widget.
    auto top_line() const -> std::size_t { return top_line_; }

    /// Return line number that is being displayed at the bottom of the Widget.
    auto bottom_line() const -> std::size_t
    {
        return this->top_line() + this->display_height() - 1;
    }

    /// Return the index into display_state_ of the last line.
    auto last_line() const -> std::size_t { return display_state_.size() - 1; }

    /// Return the index of the first Glyph at line number \p line.
    auto first_index_at(std::size_t line) const -> std::size_t
    {
        if (line >= display_state_.size())
            line = display_state_.size() - 1;
        return display_state_.at(line).start_index;
    }

    /// Return the index of the last Glyph at line number \p line.
    auto last_index_at(std::size_t line) const -> std::size_t
    {
        const auto next_line = line + 1;
        if (next_line >= display_state_.size())
            return this->end_index();
        return display_state_.at(next_line).start_index;
    }

    /// Return the number of Glyphs contained at line index \p line
    auto line_length(std::size_t line) const -> std::size_t
    {
        if (line >= display_state_.size())
            line = display_state_.size() - 1;
        return display_state_.at(line).length;
    }

    /// Return the index of the last Glyph in contents.
    /** Can give an incorrect result if contents is empty. */
    auto end_index() const -> std::size_t
    {
        return this->contents().empty() ? 0 : this->contents().size() - 1;
    }

    /// Recalculate the text layout via display_state_.
    /** This updates display_state_, depends on the Widget's dimensions, if word
     *  wrap is enabled, and the contents.*/
    void update_display(std::size_t from_line = 0);

   private:
    /// Provides a start index into contents and total length for a text line.
    struct Line_info {
        std::size_t start_index;
        std::size_t length;
    };

    /// Index into display_state_.
    std::size_t top_line_   = 0;
    bool word_wrap_enabled_ = true;
    Align alignment_        = Align::Left;
    Glyph_string contents_;
    std::vector<Line_info> display_state_ = {Line_info{0, 0}};
};

/// Helper function to create an instance.
template <typename... Args>
auto text_display(Args&&... args) -> std::unique_ptr<Text_display>
{
    return std::make_unique<Text_display>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
