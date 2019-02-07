#include <cppurses/widget/widgets/cycle_box.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/system/mouse_data.hpp>

namespace cppurses {

Cycle_box::Cycle_box() {
    this->set_alignment(Alignment::Center);  // might be default
    this->disable_word_wrap();
}

sig::Signal<void()>& Cycle_box::add_option(Glyph_string option) {
    options_.emplace_back(std::move(option));
    if (options_.size() == 1) {
        this->set_contents(options_.front().name);
    }
    this->update();
    return options_.back().enabled;
}

void Cycle_box::remove_option(const std::string& option) {
    auto iter = std::find_if(
        std::begin(options_), std::end(options_),
        [&option](const Option& opt) { return opt.name.str() == option; });
    if (iter != std::end(options_)) {
        options_.erase(iter);
    }
    this->update();
}

Glyph_string Cycle_box::current_option() const {
    if (options_.empty()) {
        return "";
    }
    return options_.front().name;
}

void Cycle_box::cycle_forward() {
    if (options_.size() > 1) {
        auto begin = std::begin(options_);
        std::rotate(begin, begin + 1, std::end(options_));
        this->set_contents(this->current_option());
        this->option_changed(this->current_option().str());
        options_.front().enabled();
        this->update();
    }
}

void Cycle_box::cycle_backward() {
    if (options_.size() > 1) {
        auto begin = std::rbegin(options_);
        std::rotate(begin, begin + 1, std::rend(options_));
        this->set_contents(this->current_option());
        this->option_changed(this->current_option().str());
        options_.front().enabled();
        this->update();
    }
}

bool Cycle_box::mouse_press_event(const Mouse_data& mouse) {
    if (mouse.button == Mouse_button::Left ||
        mouse.button == Mouse_button::ScrollUp) {
        this->cycle_forward();
    } else if (mouse.button == Mouse_button::ScrollDown) {
        this->cycle_backward();
    }
    return Label::mouse_press_event(mouse);
}

Cycle_box::Option::Option(Glyph_string name_) : name{std::move(name_)} {}

namespace slot {

sig::Slot<void(Glyph_string)> add_option(Cycle_box& cb) {
    sig::Slot<void(Glyph_string)> slot{
        [&cb](Glyph_string option) { cb.add_option(std::move(option)); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> add_option(Cycle_box& cb, const Glyph_string& option) {
    sig::Slot<void()> slot{[&cb, option] { cb.add_option(option); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void(const std::string&)> remove_option(Cycle_box& cb) {
    sig::Slot<void(const std::string&)> slot{
        [&cb](const std::string& option) { cb.remove_option(option); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> remove_option(Cycle_box& cb, const std::string& option) {
    sig::Slot<void()> slot{[&cb, option] { cb.remove_option(option); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> cycle_forward(Cycle_box& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.cycle_forward(); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> cycle_backward(Cycle_box& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.cycle_backward(); }};
    slot.track(cb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
