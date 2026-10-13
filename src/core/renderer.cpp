#include <ox/core/renderer.hpp>

#include <algorithm>
#include <stdexcept>

#include <esc/sequence.hpp>
#include <esc/text.hpp>

namespace ox {

Renderer::Renderer(esc::TerminalProfile profile) : profile_{profile}
{
    if (!profile_.control) {
        throw std::invalid_argument{"Renderer requires cursor positioning support"};
    }
}

void Renderer::invalidate() noexcept
{
    valid_ = false;
    position_.reset();
    appearance_.reset();
    last_text_.clear();
}

void Renderer::move_to(Point point, bool force)
{
    if (!force && position_ == point) { return; }
    esc::append_encoded(output_, profile_, esc::Cursor{point.x, point.y});
    position_ = point;
    last_text_.clear();
}

auto Renderer::render(Frame const& frame,
                      Cursor cursor,
                      Color foreground,
                      Color background) -> std::string_view
{
    output_.clear();
    try {
        auto const resized = size_ != frame.size();
        auto const repaint = !valid_ || resized || foreground_ != foreground ||
                             background_ != background;
        if (!valid_ || resized) {
            position_.reset();
            last_text_.clear();
            cells_.resize(static_cast<std::size_t>(frame.size().width) *
                          static_cast<std::size_t>(frame.size().height));
        }
        revisions_.resize((std::size_t)frame.size().height);
        auto const same_frame = identity_ == frame.identity_;
        for (auto y = 0; y < frame.size().height; ++y) {
            auto const revision = frame.revisions_[(std::size_t)y];
            if (!repaint && same_frame && revisions_[(std::size_t)y] == revision) {
                continue;
            }
            auto const row = frame.row(y);
            auto const offset = static_cast<std::size_t>(y) * row.size();
            // Clearing and repainting can restore the same row. Check that common
            // case outside the encoding loop, then cache its new revision.
            auto const previous =
                std::span<Cell const>{cells_}.subspan(offset, row.size());
            if (!repaint && std::ranges::equal(row, previous)) {
                revisions_[(std::size_t)y] = revision;
                continue;
            }
            for (auto x = std::size_t{0}; x < row.size();) {
                auto const& cell = row[x];
                if (repaint || cell != cells_[offset + x]) {
                    auto appearance = cell.style;
                    if (appearance.foreground == Color{default_color}) {
                        appearance.foreground = foreground;
                    }
                    if (appearance.background == Color{default_color}) {
                        appearance.background = background;
                    }
                    auto const at = Point{static_cast<int>(x), y};
                    auto separate = false;
                    if (position_ == at && !last_text_.empty()) {
                        auto const length = last_text_.size();
                        last_text_ += cell.text;
                        // Two individually valid clusters can join (e.g. regional
                        // indicators). Cursor addressing preserves their boundary.
                        separate = esc::next_grapheme(std::u32string_view{last_text_})
                                       .size() != length;
                    }
                    move_to(at, separate);
                    if (!appearance_ || *appearance_ != appearance) {
                        next_style_.clear();
                        esc::append_encoded(next_style_, profile_, appearance);
                        if (!appearance_ || style_ != next_style_) {
                            output_ += next_style_;
                            style_.swap(next_style_);
                        }
                        appearance_ = appearance;
                    }
                    esc::append_utf8(output_, cell.text);
                    last_text_ = cell.text;
                    auto const end = x + cell.columns;
                    // At the right margin the terminal may have a pending wrap.
                    // Never infer the next position from that state.
                    position_ = end < row.size()
                                    ? Cursor{Point{static_cast<int>(end), y}}
                                    : Cursor{};
                    for (auto i = x; i < end; ++i) {
                        if (cells_[offset + i] != row[i]) {
                            cells_[offset + i] = row[i];
                        }
                    }
                }
                x += cell.columns;
            }
            revisions_[(std::size_t)y] = revision;
        }
        if (cursor && contains(Rect{{0, 0}, frame.size()}, *cursor)) {
            move_to(frame.lead_at(*cursor));
        }
        identity_ = frame.identity_;
        size_ = frame.size();
        foreground_ = foreground;
        background_ = background;
        valid_ = true;
        return output_;
    }
    catch (...) {
        invalidate();
        throw;
    }
}

}  // namespace ox
