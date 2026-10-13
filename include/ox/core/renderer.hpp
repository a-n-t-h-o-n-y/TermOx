#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <esc/capabilities.hpp>

#include <ox/core/frame.hpp>

namespace ox {

/** Compares frames and encodes changes without terminal I/O.
 * Owns its profile, previous cells, and reusable output storage. */
class Renderer {
   public:
    explicit Renderer(esc::TerminalProfile profile);

    /** The returned bytes borrow storage until the next render or destruction.
     * Rendering assumes the previous result was written in full. After an output
     * failure or external terminal writes, invalidate before rendering again.
     * Cursor visibility and output flushing belong to the caller.
     */
    auto render(Frame const& frame,
                Cursor cursor = {},
                Color foreground = default_color,
                Color background = default_color) -> std::string_view;

    /** Forget displayed cells, cursor position, and style; retain allocated storage. */
    void invalidate() noexcept;

   private:
    esc::TerminalProfile profile_;
    std::vector<Cell> cells_;
    std::shared_ptr<char const> identity_;
    std::vector<std::uint64_t> revisions_;
    Area size_ = {0, 0};
    bool valid_ = false;
    Color foreground_ = default_color;
    Color background_ = default_color;
    Cursor position_;
    std::optional<Style> appearance_;
    std::string style_;
    std::string next_style_;
    std::string output_;
    std::u32string last_text_;

    void move_to(Point point, bool force = false);
};

}  // namespace ox
