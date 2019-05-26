#ifndef CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#include <cstddef>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
struct Point;

/// Used to define the alignment of text in a Text_display Widget.
enum class Alignment { Left, Center, Right };

/// Non-interactive box to display a given Glyph_string.
/** Provides operations to change the text, wrap words on spaces, change the
 *  alignment of the text and scroll through the text, among others. */
class Text_display : public Widget {
   public:
    /// Construct a Text_display with initial Glyph_string \p contents.
    /** By default, wraps text, and has left alignment. */
    explicit Text_display(Glyph_string contents = "");

    /// Replace the current contents with \p text.
    /** Reset the display to show the first line at the top of the screen and
     *  the cursor at the first Glyph, or where the first Glyph would be. */
    void set_contents(Glyph_string text);

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
    void set_alignment(Alignment type);

    /// Return the currently used Alignment.
    Alignment alignment() const { return alignment_; }

    /// Scroll the display up by \p n lines.
    /** Tops out at the first line displayed at the top of the display. */
    virtual void scroll_up(std::size_t n = 1);

    /// Scroll the display down by \p n lines.
    /** Bottoms out at the last line displaying at the top of the display. */
    virtual void scroll_down(std::size_t n = 1);

    /// Enable word wrapping on spaces if \p enable is true, else disable it.
    void enable_word_wrap(bool enable = true);

    /// Disable word wrapping if \disable is true, otherwise enable it.
    void disable_word_wrap(bool disable = true);

    /// Toggle word wrap between enabled/disabled depending on current state.
    void toggle_word_wrap();

    /// Brush to be applied to all new incoming Glyphs, but not existing Glyphs.
    /** Widget::brush is applied after this Brush. */
    Brush insert_brush{this->brush};

    /// Return the length of the line at row \p y.
    /** Index 0 is the top of the Widget. */
    std::size_t row_length(std::size_t y) const;

    /// Return the number of lines currently displayed.
    std::size_t display_height() const;

    /// Return the index into the contents from a physical Point on the Widget.
    /** If \p position is past any text on the corresponding line, then return
     *  index of the last Glyph on that line. If Point is past displayed lines,
     *  return the index of the last Glyph in contents. */
    std::size_t index_at(Point position) const;

    /// Return the position of the Glyph at \p index.
    /** If \p index is not currently displayed on screen, return the closest
     *  Glyph position to \p index that is displayed on screen. */
    Point display_position(std::size_t index) const;

    /// Return the entire contents of the Text_display.
    /** Provided as a non-const reference so contents can be modified without
     *  limitation from the Text_display interface. Be sure to call
     *  Text_display::update() after modifying the contents directly. */
    Glyph_string& contents() { return contents_; }

    /// Return the entire contents of the Text_display.
    const Glyph_string& contents() const { return contents_; }

    /// Return whether word wrapping is enabled.
    bool word_wrap_enabled() const { return word_wrap_enabled_; }

    /// Emitted when text is scrolled up. Sends number of lines scrolled by.
    sig::Signal<void(std::size_t n)> scrolled_up;

    /// Emitted when text is scrolled down. Sends number of lines scrolled by.
    sig::Signal<void(std::size_t n)> scrolled_down;

    /// Emitted when contents are modified. Sends a reference to the contents.
    sig::Signal<void(const Glyph_string&)> contents_modified;

   protected:
    /// Add call to Text_display::update_display() before posting Paint_event.
    void update() override;

    /// Paint the portion of contents that is currently visible on screen.
    bool paint_event() override;

    /// Return the line number that contains \p index.
    std::size_t line_at(std::size_t index) const;

    /// Return the line number that is being displayed at the top of the Widget.
    std::size_t top_line() const { return top_line_; }

    /// Return line number that is being displayed at the bottom of the Widget.
    std::size_t bottom_line() const {
        return this->top_line() + this->display_height() - 1;
    }

    /// Return the index into display_state_ of the last line.
    std::size_t last_line() const { return display_state_.size() - 1; }

    /// Return the total number of lines in display_state_.
    std::size_t line_count() const { return display_state_.size(); }

    /// Return the index of the first Glyph at line number \p line.
    std::size_t first_index_at(std::size_t line) const;

    /// Return the index of the last Glyph at line number \p line.
    std::size_t last_index_at(std::size_t line) const;

    /// Return the number of Glyphs contained at line index \p line
    std::size_t line_length(std::size_t line) const;

    /// Return the index of the last Glyph in contents.
    /** Can give an incorrect result if contents is empty. */
    std::size_t end_index() const {
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

    std::vector<Line_info> display_state_{Line_info{0, 0}};
    Glyph_string contents_;

    /// Index into display_state_.
    std::size_t top_line_{0};

    bool word_wrap_enabled_{true};
    Alignment alignment_{Alignment::Left};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
