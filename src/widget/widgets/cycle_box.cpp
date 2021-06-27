#include <termox/widget/widgets/cycle_box.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>

namespace {

/// \p edge is one-past-the-last element, will go to zero instead of \p edge
/** Input Constraints: \p edge > 0  and \p i < \p edge */
auto wrapped_increment(std::size_t i, std::size_t edge) -> std::size_t
{
    return ++i == edge ? 0 : i;
}

/// \p edge is one-past-the-last element, will go to edge - 1 instead of -1.
/** Input Constraints: \p edge > 0  and \p i < \p edge */
auto wrapped_decrement(std::size_t i, std::size_t edge) -> std::size_t
{
    return i == 0 ? --edge : --i;
}

}  // namespace

namespace ox {

Cycle_box::Cycle_box(Align alignment,
                     int extra_left,
                     int extra_right,
                     Growth growth_strategy)
    : HLabel{U"", alignment, extra_left, extra_right, growth_strategy}
{
    *this | pipe::strong_focus();
}

Cycle_box::Cycle_box(Parameters p)
    : Cycle_box{p.alignment, p.extra_left, p.extra_right, p.growth_strategy}
{}

auto Cycle_box::add_option(Glyph_string label) -> sl::Signal<void()>&
{
    options_.push_back({std::move(label)});
    if (this->size() == 1)
        this->set_current_option(0);
    return options_.back().enabled;
}

void Cycle_box::remove_option(Glyph_string const& label)
{
    auto const& label_str = label.str();
    if (label_str == this->current_option_label().str())
        this->set_current_option(0);
    auto const iter = this->find(label_str);
    if (iter != std::end(options_))
        options_.erase(iter);
}

void Cycle_box::next()
{
    if (this->size() < 2)
        return;
    this->set_current_option(wrapped_increment(index_, this->size()));
    this->emit_signals();
}

void Cycle_box::previous()
{
    if (this->size() < 2)
        return;
    this->set_current_option(wrapped_decrement(index_, this->size()));
    this->emit_signals();
}

void Cycle_box::set_current_option(std::string const& label)
{
    auto const iter = this->find(label);
    if (iter != std::end(options_))
        this->set_current_option(std::distance(std::begin(options_), iter));
}

void Cycle_box::set_current_option(std::size_t index)
{
    index_ = index;
    *this | pipe::text(options_[index_].name);
}

auto Cycle_box::size() const -> std::size_t { return options_.size(); }

auto Cycle_box::mouse_press_event(Mouse const& m) -> bool
{
    switch (m.button) {
        case Mouse::Button::Left: this->next(); break;
        case Mouse::Button::Right: this->previous(); break;
        default: break;
    }
    return HLabel::mouse_press_event(m);
}

auto Cycle_box::mouse_wheel_event(Mouse const& m) -> bool
{
    switch (m.button) {
        case Mouse::Button::ScrollDown: this->next(); break;
        case Mouse::Button::ScrollUp: this->previous(); break;
        default: break;
    }
    return HLabel::mouse_wheel_event(m);
}

auto Cycle_box::key_press_event(Key k) -> bool
{
    switch (k) {
        case Key::Arrow_up:
        case Key::k: this->previous(); break;
        case Key::Arrow_down:
        case Key::j: this->next(); break;
        default: break;
    }
    return HLabel::key_press_event(k);
}

auto Cycle_box::current_option_label() const -> Glyph_string
{
    if (options_.empty())
        return U"";
    return options_[index_].name;
}

auto Cycle_box::find(std::string const& label) -> decltype(options_)::iterator
{
    auto const begin = std::begin(options_);
    auto const end   = std::end(options_);
    return std::find_if(
        begin, end, [&label](Option const& opt) { return opt.name == label; });
}

void Cycle_box::emit_signals()
{
    this->option_changed(this->current_option_label().str());
    options_[index_].enabled();  // This is a signal.
}

auto cycle_box(Cycle_box::Parameters) -> std::unique_ptr<Cycle_box>
{
    return std::make_unique<Cycle_box>();
}

Labeled_cycle_box::Labeled_cycle_box(Glyph_string title)
    : HTuple<HLabel, Tile, Cycle_box>{
          {std::move(title).append(U' '), Align::Left, 0, 0, Growth::Dynamic},
          {U'├'},
          {}}
{
    using namespace ox::pipe;
    *this | fixed_height(1) | direct_focus() | forward_focus(cycle_box);
}

Labeled_cycle_box::Labeled_cycle_box(Parameters p)
    : Labeled_cycle_box{std::move(p.label)}
{}

auto labeled_cycle_box(Glyph_string label) -> std::unique_ptr<Labeled_cycle_box>
{
    return std::make_unique<Labeled_cycle_box>(std::move(label));
}

auto labeled_cycle_box(Labeled_cycle_box::Parameters p)
    -> std::unique_ptr<Labeled_cycle_box>
{
    return std::make_unique<Labeled_cycle_box>(std::move(p));
}

}  // namespace ox

namespace ox::slot {

auto add_option(Cycle_box& cb) -> sl::Slot<void(Glyph_string)>
{
    return link_lifetimes(
        [&cb](Glyph_string label) { cb.add_option(std::move(label)); }, cb);
}

auto add_option(Cycle_box& cb, Glyph_string const& label) -> sl::Slot<void()>
{
    return link_lifetimes([&cb, label] { cb.add_option(label); }, cb);
}

auto remove_option(Cycle_box& cb) -> sl::Slot<void(std::string const&)>
{
    return link_lifetimes(
        [&cb](std::string const& label) { cb.remove_option(label); }, cb);
}

auto remove_option(Cycle_box& cb, std::string const& label) -> sl::Slot<void()>
{
    return link_lifetimes([&cb, label] { cb.remove_option(label); }, cb);
}

auto next(Cycle_box& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb]() { cb.next(); }, cb);
}

auto previous(Cycle_box& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb]() { cb.previous(); }, cb);
}

}  // namespace ox::slot
