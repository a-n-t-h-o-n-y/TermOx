#include <termox/widget/widgets/detail/textline_base.hpp>

#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>

namespace ox::detail {

Textline_base::Textline_base(ox::Glyph_string initial,
                             ox::Align align,
                             Action focus_in_action)
    : core_{std::move(initial), 0, align}, focus_in_action_{focus_in_action}
{
    using namespace ox::pipe;
    *this | fixed_height(1) | show_cursor() | strong_focus();
}

Textline_base::Textline_base(Parameters p)
    : Textline_base{std::move(p.initial), p.align, p.focus_in_action}
{}

void Textline_base::set_text(ox::Glyph_string x)
{
    core_.set_text(std::move(x));
    this->update();
}

auto Textline_base::text() const noexcept -> ox::Glyph_string const&
{
    return core_.text();
}

void Textline_base::set_alignment(ox::Align x)
{
    core_.set_alignment(x);
    this->update();
}

auto Textline_base::alignment() const noexcept -> ox::Align
{
    return core_.alignment();
}

void Textline_base::set_focus_in_action(Action x) { focus_in_action_ = x; }

void Textline_base::clear_on_focus() { focus_in_action_ = Action::Clear; }

void Textline_base::nothing_on_focus() { focus_in_action_ = Action::None; }

auto Textline_base::focus_in_action() const noexcept -> Action
{
    return focus_in_action_;
}

void Textline_base::set_cursor_to_index(int i)
{
    core_.set_cursor_to_index(i);
    this->update();
}

auto Textline_base::paint_event(ox::Painter& p) -> bool
{
    auto const str = core_.display_substr();
    switch (core_.alignment()) {
        case ox::Align::Left:
            p.put(str, {0, 0});
            this->cursor.set_position({core_.cursor_position(), 0});
            break;
        case ox::Align::Right: {
            p.put(str, {this->width() - str.size(), 0});
            this->cursor.set_position(
                {this->width() - str.size() + core_.cursor_position(), 0});
            break;
        }
        default: break;
    }
    return Widget::paint_event(p);
}

auto Textline_base::key_press_event(ox::Key k) -> bool
{
    switch (k) {
        using ox::Key;
        case Key::Arrow_left: core_.decrement_cursor(); break;
        case Key::Arrow_right: core_.increment_cursor(); break;
        case Key::Delete: core_.erase_at_cursor(); break;
        case Key::Backspace: core_.erase_before_cursor(); break;
        case Key::Home: core_.cursor_home(); break;
        case Key::End: core_.cursor_end(); break;
        default: break;
    }
    this->update();
    return Widget::key_press_event(k);
}

auto Textline_base::mouse_press_event(ox::Mouse const& m) -> bool
{
    core_.move_cursor_to_display(m.at.x);
    this->update();
    return Widget::mouse_press_event(m);
}

auto Textline_base::resize_event(ox::Area new_size, ox::Area old_size) -> bool
{
    core_.set_display_length(new_size.width);
    this->update();
    return Widget::resize_event(new_size, old_size);
}

auto Textline_base::focus_in_event() -> bool
{
    if (focus_in_action_ == Action::Clear) {
        stored_text_ = core_.text();
        core_.clear();
        this->update();
    }
    return Widget::focus_in_event();
}

auto Textline_base::focus_out_event() -> bool
{
    if (focus_in_action_ == Action::Clear) {
        if (this->core_.text().empty())
            this->set_text(stored_text_);
    }
    return Widget::focus_out_event();
}

}  // namespace ox::detail
