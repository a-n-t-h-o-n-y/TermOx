#include <ox/checkbox.hpp>

#include <ox/put.hpp>

namespace ox {

CheckBox::Options const CheckBox::init = {};

CheckBox::CheckBox(Options x)
    : Widget{x.focus_policy, SizePolicy::fixed(4)},
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
        if (state_ == State::Checked) { this->on_check(); }
        else {
            this->on_uncheck();
        }
    }
}

auto CheckBox::get_state() const -> State { return state_; }

auto CheckBox::key_press(KeyEvent k) -> EventResponse
{
    if (!k.matches(KeyCode::Enter, {}, k.action)) { return {}; }
    this->toggle();

    return {.handled = true, .repaint = true};
}

auto CheckBox::mouse_press(MousePress m) -> EventResponse
{
    if (m.button != MouseButton::Left) { return {}; }
    this->toggle();

    return {.handled = true, .repaint = true};
}

void CheckBox::paint(Canvas c)
{
    auto glyphs = state_ == State::Checked ? display.checked : display.unchecked;
    if (in_focus_) { glyphs = glyphs | display.in_focus_style; }
    put(c, {.x = 0, .y = 0}, glyphs);
}

auto CheckBox::focus_in() -> EventResponse
{
    in_focus_ = true;
    return {.handled = true, .repaint = true};
}

auto CheckBox::focus_out() -> EventResponse
{
    in_focus_ = false;
    return {.handled = true, .repaint = true};
}

}  // namespace ox