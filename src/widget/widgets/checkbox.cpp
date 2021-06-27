#include <termox/widget/widgets/checkbox.hpp>

#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/trait.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox {

template <template <typename> typename Layout_t>
Checkbox<Layout_t>::Checkbox(State initial_state, Display display, bool locked)
    : Base_t{initial_state == State::Unchecked ? display.unchecked
                                               : display.checked,
             Align::Left, 0, 0, Growth::Dynamic},
      state_{initial_state},
      display_{display},
      locked_{locked}
{
    if (locked_) {
        display_.checked |= Trait::Dim;
        display_.unchecked |= Trait::Dim;
        this->update_display();
    }
}

template <template <typename> typename Layout_t>
Checkbox<Layout_t>::Checkbox(Parameters p)
    : Checkbox{p.initial_state, p.display, p.locked} {};

template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::check()
{
    if (locked_)
        return;
    if (state_ == State::Unchecked) {
        state_ = State::Checked;
        this->Base_t::set_text(display_.checked);
        checked.emit();
        toggled.emit();
    }
}

/// Set the state to be unchecked.
template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::uncheck()
{
    if (locked_)
        return;
    if (state_ == State::Checked) {
        state_ = State::Unchecked;
        this->Base_t::set_text(display_.unchecked);
        unchecked.emit();
        toggled.emit();
    }
}

template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::toggle()
{
    switch (state_) {
        case State::Checked: this->uncheck(); break;
        case State::Unchecked: this->check(); break;
    }
}

template <template <typename> typename Layout_t>
auto Checkbox<Layout_t>::get_state() const -> State
{
    return state_;
}
template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::lock()
{
    locked_ = true;
    display_.checked |= Trait::Dim;
    display_.unchecked |= Trait::Dim;
    this->update_display();
}

template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::unlock()
{
    locked_ = false;
    display_.checked.remove_traits(Trait::Dim);
    display_.unchecked.remove_traits(Trait::Dim);
    this->update_display();
}

template <template <typename> typename Layout_t>
auto Checkbox<Layout_t>::is_locked() const -> bool
{
    return locked_;
}

template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::set_display(Display d)
{
    display_ = std::move(d);
    if (locked_) {
        display_.checked |= Trait::Dim;
        display_.unchecked |= Trait::Dim;
    }
    this->update_display();
}

template <template <typename> typename Layout_t>
auto Checkbox<Layout_t>::get_display() -> Display
{
    auto result = display_;
    if (locked_) {
        result.checked.remove_traits(Trait::Dim);
        result.unchecked.remove_traits(Trait::Dim);
    }
    return result;
}

template <template <typename> typename Layout_t>
auto Checkbox<Layout_t>::mouse_press_event(Mouse const& m) -> bool
{
    if (m.button == Mouse::Button::Left)
        this->toggle();
    return Base_t::mouse_press_event(m);
}

template <template <typename> typename Layout_t>
void Checkbox<Layout_t>::update_display()
{
    this->Base_t::set_text(state_ == State::Unchecked ? display_.unchecked
                                                      : display_.checked);
}

template class Checkbox<layout::Horizontal>;
template class Checkbox<layout::Vertical>;

auto hcheckbox(HCheckbox::State initial_state,
               HCheckbox::Display display,
               bool locked) -> std::unique_ptr<HCheckbox>
{
    return std::make_unique<HCheckbox>(initial_state, std::move(display),
                                       locked);
}

auto hcheckbox(HCheckbox::Parameters p) -> std::unique_ptr<HCheckbox>
{
    return std::make_unique<HCheckbox>(std::move(p));
}

auto vcheckbox(VCheckbox::State initial_state,
               VCheckbox::Display display,
               bool locked) -> std::unique_ptr<VCheckbox>
{
    return std::make_unique<VCheckbox>(initial_state, std::move(display),
                                       locked);
}

auto vcheckbox(VCheckbox::Parameters p) -> std::unique_ptr<VCheckbox>
{
    return std::make_unique<VCheckbox>(std::move(p));
}

}  // namespace ox

namespace ox::slot {

template <template <typename> typename Layout_t>
auto toggle(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.toggle(); }, cb);
}

template auto toggle(HCheckbox&) -> sl::Slot<void()>;
template auto toggle(VCheckbox&) -> sl::Slot<void()>;

template <template <typename> typename Layout_t>
auto check(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.check(); }, cb);
}

template auto check(HCheckbox&) -> sl::Slot<void()>;
template auto check(VCheckbox&) -> sl::Slot<void()>;

template <template <typename> typename Layout_t>
auto uncheck(Checkbox<Layout_t>& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb] { cb.uncheck(); }, cb);
}

template auto uncheck(HCheckbox&) -> sl::Slot<void()>;
template auto uncheck(VCheckbox&) -> sl::Slot<void()>;

}  // namespace ox::slot
