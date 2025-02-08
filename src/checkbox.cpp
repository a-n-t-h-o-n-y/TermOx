#include <ox/checkbox.hpp>

namespace ox {

CheckBox::Options const CheckBox::init = {};

CheckBox::CheckBox(Options x)
    : Widget{FocusPolicy::None, SizePolicy::fixed(4)},
      display{std::move(x.display)},
      state_{std::move(x.state)}
{}

void CheckBox::toggle()
{
    if (state_ == State::Checked) {
        state_ = State::UnChecked;
        this->on_uncheck();
    }
    else {
        state_ = State::Checked;
        this->on_check();
    }
}

void CheckBox::check()
{
    if (state_ != State::Checked) {
        state_ = State::Checked;
        this->on_check();
    }
}

void CheckBox::uncheck()
{
    if (state_ != State::UnChecked) {
        state_ = State::UnChecked;
        this->on_uncheck();
    }
}

void CheckBox::set_state(State s)
{
    if (state_ != s) {
        state_ = s;
        if (state_ == State::Checked) {
            this->on_check();
        }
        else {
            this->on_uncheck();
        }
    }
}

auto CheckBox::get_state() const -> State { return state_; }

void CheckBox::key_press(Key k)
{
    if (k == Key::Enter) {
        this->toggle();
    }
}

void CheckBox::mouse_press(Mouse m)
{
    if (m.button == Mouse::Button::Left) {
        this->toggle();
    }
}

void CheckBox::paint(Canvas c)
{
    auto glyphs = state_ == State::Checked ? display.checked : display.unchecked;
    if (in_focus_) {
        glyphs = glyphs | display.in_focus_brush;
    }
    Painter{c}[{0, 0}] << glyphs;
}

void CheckBox::focus_in() { in_focus_ = true; }

void CheckBox::focus_out() { in_focus_ = false; }

}  // namespace ox