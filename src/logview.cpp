#include <limits>
#include <ox/logview.hpp>
#include <ox/selection.hpp>
namespace ox {
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
    auto bytes = std::size_t{0};
    while (lines_.size() > max_lines_) {
        bytes += lines_.front().size() + 1;
        lines_.pop_front();
    }
    if (!bytes) return;
    auto offset = editor.scroll_offset();
    auto caret = editor.editor().caret();
    auto anchor = editor.editor().anchor();
    auto lifetime = track(*this);
    editor.edit([=](TextEdit& e) {
        e.set_caret(0);
        e.set_caret(bytes, true);
        e.erase_backward();
        e.set_caret(anchor > bytes ? anchor - bytes : 0);
        e.set_caret(caret > bytes ? caret - bytes : 0, true);
    });
    // Wrapped line counts can change on resize; clamp the retained viewport.
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
    auto caret = editor.editor().caret();
    auto anchor = editor.editor().anchor();
    for (auto& line : records) {
        lines_.push_back(std::move(line));
    }
    auto removed = std::string{};
    while (lines_.size() > max_lines_) {
        removed += lines_.front();
        removed += '\n';
        lines_.pop_front();
    }
    auto removed_layout = TextLayout{};
    removed_layout.update(removed, editor.size().width, editor.wrap,
                          glyph_width_policy());
    auto rows = removed.empty() ? 0 : (int)removed_layout.lines().size() - 1;
    at.y = std::max(0, at.y - rows);
    auto lifetime = track(*this);
    updating_ = true;
    editor.edit([&](TextEdit& e) {
        e.set_caret(e.text().size());
        e.insert(batch);
        if (!removed.empty()) {
            e.set_caret(0);
            e.set_caret(removed.size(), true);
            e.erase_backward();
        }
        e.set_caret(anchor > removed.size() ? anchor - removed.size() : 0);
        e.set_caret(caret > removed.size() ? caret - removed.size() : 0, true);
    });
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
