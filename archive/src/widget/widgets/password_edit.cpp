#include <termox/widget/widgets/password_edit.hpp>

#include <algorithm>

#include <termox/painter/glyph_string.hpp>

namespace ox {
Password_edit::Password_edit(Glyph veil, bool show_contents)
    : veil_{veil}, show_contents_{show_contents}
{}

Password_edit::Password_edit(Parameters p)
    : Password_edit{p.veil, p.show_contents}
{}

void Password_edit::set_veil(Glyph x) { veil_ = x; }

auto Password_edit::veil() const -> Glyph { return veil_; }

void Password_edit::show_contents(bool enable) { show_contents_ = enable; }

auto Password_edit::shows_contents() const -> bool { return show_contents_; }

auto Password_edit::paint_event(Painter& p) -> bool
{
    if (show_contents_)
        return Line_edit::paint_event(p);
    else {
        auto const length =
            std::min(this->Line_edit::text().size(), this->area().width);
        p.put(Glyph_string{veil_, length}, {0, 0});
        this->cursor.set_position({Textline_base::core_.cursor_position(), 0});
    }
    return true;
}
}  // namespace ox
