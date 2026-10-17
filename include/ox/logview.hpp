/** @file
 * Bounded streaming logs with filtering and follow mode.
 */
#pragma once
#include <deque>
#include <ox/textbox.hpp>
namespace ox {
/** Bounded read-only text with selection/copy, wrapping, and explicit tail following.
 */
class LogView : public Widget {
   public:
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Positive maximum retained logical log lines; older complete lines are evicted. */
        std::size_t max_lines = 10000;
        /** Reveal new log content at the tail when true; manual scrolling can disable
         * following.
         */
        bool follow_tail = true;
        /** Initial configuration for the embedded editor; LogView forces read-only mode. */
        TextBox::Options editor = {};
    };
    /** Owned read-only text editor providing selection, copying, and scrolling. */
    TextBox editor;
    /** Emits the count of logical lines accepted by append(), after trimming and editor
     * update.
     */
    sl::Signal<void(std::size_t)> on_append;
    /** Emits the new follow-tail flag when it changes. */
    sl::Signal<void(bool)> on_follow;
    /** Create a bounded read-only log following its tail with default retention. */
    LogView();
    /** Configure retention and tail following around an owned read-only TextBox;
     * max_lines must be positive.
     */
    explicit LogView(Options);
    /** Borrow retained logical log lines until append, clear, or retention changes. */
    auto lines() const -> std::deque<std::string> const& { return lines_; }
    /** Return the positive retained-line limit. */
    auto max_lines() const -> std::size_t { return max_lines_; }
    /** Set a positive retention limit and trim oldest lines if needed; zero throws
     * std::invalid_argument.
     */
    void set_max_lines(std::size_t);
    /** Return whether appended content is automatically revealed. */
    auto follows_tail() const -> bool { return follow_tail_; }
    /** Change tail-follow policy, reveal the tail when enabling it, and emit on_follow
     * only on change.
     */
    auto set_follow_tail(bool) -> bool;
    /** Normalize and append logical lines, evict oldest complete lines to the retention
     * limit, update the editor, and emit on_append.
     */
    void append(std::string_view);
    /** Remove all retained lines and reset the displayed document and viewport. */
    void clear();
    /** Return the embedded view's analyzed content extent in terminal cells. */
    auto content_size() const -> Area { return editor.content_size(); }
    /** Return the embedded view's visible terminal-cell extent. */
    auto viewport_size() const -> Area { return editor.viewport_size(); }
    /** Return the embedded view's clamped content offset in columns and rows. */
    auto scroll_offset() const -> Point { return editor.scroll_offset(); }
    /** Forward scrolling to the embedded view, which clamps and emits on_scroll only for
     * effective state changes.
     */
    auto set_scroll_offset(Point at) -> bool { return editor.set_scroll_offset(at); }
    /** Emits (offset, content extent, viewport extent) when scroll state or geometry
     * changes; units are terminal columns and rows.
     */
    sl::Signal<void(Point, Area, Area)> on_scroll;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto get_children() -> zzz::Generator<Widget&> override { co_yield editor; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield editor; }

   protected:
    void on_arrange(Rect) override;

   private:
    std::deque<std::string> lines_;
    std::size_t max_lines_;
    bool follow_tail_;
    bool updating_ = false;
    void trim();
    void reveal_tail();
};
}  // namespace ox
