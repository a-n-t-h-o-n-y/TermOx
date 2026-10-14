#pragma once
#include <deque>
#include <ox/textbox.hpp>
namespace ox {
/** Bounded read-only text with selection/copy, wrapping, and explicit tail following.
 */
class LogView : public Widget {
   public:
    struct Options {
        std::size_t max_lines = 10000;
        bool follow_tail = true;
        TextBox::Options editor = {};
    };
    TextBox editor;
    sl::Signal<void(std::size_t)> on_append;
    sl::Signal<void(bool)> on_follow;
    LogView();
    explicit LogView(Options);
    auto lines() const -> std::deque<std::string> const& { return lines_; }
    auto max_lines() const -> std::size_t { return max_lines_; }
    void set_max_lines(std::size_t);
    auto follows_tail() const -> bool { return follow_tail_; }
    auto set_follow_tail(bool) -> bool;
    void append(std::string_view);
    void clear();
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
