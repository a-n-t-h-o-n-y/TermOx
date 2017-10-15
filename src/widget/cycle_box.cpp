#include "widget/widgets/cycle_box.hpp"

#include <signals/signals.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

Cycle_box::Cycle_box(Glyph_string title) {
    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(1);

    this->set_title(std::move(title));

    disable_walls(label.border);
    disable_corners(label.border);
    label.border.east_enabled = true;
    label.border.east = "├";
    enable_border(label);

    options_box.set_alignment(Alignment::Center);

    // set_background(options_box, Color::White);
    // set_foreground(options_box, Color::Black);

    options_box.clicked.connect(slot::cycle(*this));
}

void Cycle_box::set_title(Glyph_string title) {
    label.set_text(std::move(title));
    this->resize_label();
}

void Cycle_box::add_option(std::string option) {
    options_.emplace_back(std::move(option));
    if (options_.size() == 1) {
        options_box.set_text(options_.front());
    }
    this->update();
}

void Cycle_box::remove_option(const std::string& option) {
    auto iter = std::find(std::begin(options_), std::end(options_), option);
    if (iter != std::end(options_)) {
        options_.erase(iter);
    }
    this->update();
}

std::string Cycle_box::current_option() const {
    if (options_.empty()) {
        return "";
    }
    return options_.front();
}

void Cycle_box::cycle() {
    if (options_.size() > 1) {
        auto begin = std::begin(options_);
        std::rotate(begin, begin + 1, std::end(options_));
        options_box.set_text(this->current_option());
        this->option_changed(this->current_option());
        this->update();
    }
}

void Cycle_box::resize_label() {
    label.width_policy.hint(label.contents().size() + 2);
    label.width_policy.type(Size_policy::Fixed);
}

namespace slot {

sig::Slot<void(std::string)> add_option(Cycle_box& cb) {
    sig::Slot<void(std::string)> slot{
        [&cb](std::string option) { cb.add_option(std::move(option)); }};
    slot.track(cb.destroyed);
    return slot;
}

sig::Slot<void()> add_option(Cycle_box& cb, const std::string& option) {
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

sig::Slot<void()> cycle(Cycle_box& cb) {
    sig::Slot<void()> slot{[&cb]() { cb.cycle(); }};
    slot.track(cb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
