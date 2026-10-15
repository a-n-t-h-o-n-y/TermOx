#include <limits>
#include <ox/logview.hpp>
#include <ox/selection.hpp>
namespace ox {
namespace {
auto retained_offset(Point offset,
                     std::string_view removed,
                     TextBox const& editor,
                     esc::WidthPolicy policy) -> Point
{
    if (removed.empty()) { return offset; }
    auto layout = TextLayout{};
    layout.update(removed, editor.size().width, editor.wrap, policy);
    auto rows = (int)layout.lines().size() - 1;
    offset.y = std::max(0, offset.y - rows);
    return offset;
}
}  // namespace
LogView::LogView() : LogView{Options{}} {}
LogView::LogView(Options x)
    : Widget{},
      editor{[&] {
          x.editor.text.clear();
          x.editor.read_only = true;
          return std::move(x.editor);
      }()},
      max_lines_{x.max_lines},
      follow_tail_{x.follow_tail}
{
    Connection{.signal = editor.on_scroll,
               .slot = [](Point at, Area content, Area viewport, LogView& self) {
                   self.on_scroll(at, content, viewport);
               }}(*this);
    if (!max_lines_) throw std::invalid_argument{"Log capacity must be positive"};
    editor.edit([](TextEdit& e) { e = TextEdit{"", true, 0}; });
    Connection{.signal = editor.on_scroll,
               .slot = [](Point at, Area content, Area viewport, LogView& self) {
                   if (!self.updating_ && self.follow_tail_ &&
                       at.y < std::max(0, content.height - viewport.height))
                       self.set_follow_tail(false);
               }}(*this);
}
void LogView::reveal_tail()
{
    editor.set_scroll_offset(
        {editor.scroll_offset().x, std::numeric_limits<int>::max()});
}
auto LogView::set_follow_tail(bool follow) -> bool
{
    if (std::exchange(follow_tail_, follow) == follow) return false;
    auto lifetime = track(*this);
    if (follow) {
        updating_ = true;
        reveal_tail();
        if (auto* self = lifetime.try_get()) self->updating_ = false;
    }
    if (auto* self = lifetime.try_get()) self->on_follow(follow);
    return true;
}
void LogView::trim()
{
    auto removed = std::string{};
    while (lines_.size() > max_lines_) {
        removed += lines_.front();
        removed += '\n';
        lines_.pop_front();
    }
    if (removed.empty()) return;
    auto bytes = removed.size();
    auto offset =
        retained_offset(editor.scroll_offset(), removed, editor, glyph_width_policy());
    auto lifetime = track(*this);
    editor.append_lines({}, bytes);
    if (auto* self = lifetime.try_get()) { self->editor.set_scroll_offset(offset); }
}
void LogView::set_max_lines(std::size_t n)
{
    if (!n) throw std::invalid_argument{"Log capacity must be positive"};
    max_lines_ = n;
    auto lifetime = track(*this);
    updating_ = true;
    trim();
    if (auto* self = lifetime.try_get()) {
        if (self->follow_tail_) self->reveal_tail();
        self = lifetime.try_get();
        if (self) self->updating_ = false;
    }
}
void LogView::append(std::string_view text)
{
    auto records = std::vector<std::string>{};
    auto start = std::size_t{0};
    for (std::size_t i = 0; i <= text.size(); ++i)
        if (i == text.size() || text[i] == '\n') {
            if (i == text.size() && i > 0 && start == i) break;
            auto line = std::string{text.substr(start, i - start)};
            validate_row_text(line);
            records.push_back(std::move(line));
            start = i + 1;
        }
    auto batch = std::string{};
    for (auto const& line : records) {
        batch += line;
        batch += '\n';
    }
    auto at = editor.scroll_offset();
    for (auto& line : records) {
        lines_.push_back(std::move(line));
    }
    auto removed = std::string{};
    while (lines_.size() > max_lines_) {
        removed += lines_.front();
        removed += '\n';
        lines_.pop_front();
    }
    at = retained_offset(at, removed, editor, glyph_width_policy());
    auto lifetime = track(*this);
    updating_ = true;
    auto discard = std::min(removed.size(), editor.text().size());
    editor.append_lines(std::string_view{batch}.substr(removed.size() - discard),
                        discard);
    auto* self = lifetime.try_get();
    if (!self) { return; }
    if (self->follow_tail_) { self->reveal_tail(); }
    else {
        self->editor.set_scroll_offset(at);
    }
    self = lifetime.try_get();
    if (!self) { return; }
    self->updating_ = false;
    self->on_append(records.size());
}

void LogView::clear()
{
    lines_.clear();
    auto lifetime = track(*this);
    updating_ = true;
    editor.set_text("");
    if (auto* self = lifetime.try_get()) self->updating_ = false;
}
auto LogView::measure(Area a, esc::WidthPolicy p) const -> Area
{ return editor.measure(a, p); }
void LogView::on_arrange(Rect)
{
    auto lifetime = track(*this);
    updating_ = true;
    arrange(editor, {{}, size()});
    if (auto* self = lifetime.try_get()) {
        if (self->follow_tail_) self->reveal_tail();
        self = lifetime.try_get();
        if (self) self->updating_ = false;
    }
}
}  // namespace ox
