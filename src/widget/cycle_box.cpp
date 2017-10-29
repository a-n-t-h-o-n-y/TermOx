#include <cppurses/widget/widgets/cycle_box.hpp>

#include <signals/signals.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

Cycle_box::Cycle_box() {
    // this->height_policy.type(Size_policy::Fixed);
    // this->height_policy.hint(1);

    // this->set_title(std::move(title));

    // disable_walls(label.border);
    // disable_corners(label.border);
    // label.border.east_enabled = true;
    // label.border.east = "├";
    // enable_border(label);

    this->set_alignment(Alignment::Center);  // might be default
}

// void Cycle_box::set_title(Glyph_string title) {
//     label.set_text(std::move(title));
//     this->resize_label();
// }

sig::Signal<void()>& Cycle_box::add_option(Glyph_string option) {
    options_.emplace_back(std::move(option));
    if (options_.size() == 1) {
        this->set_text(options_.front().name);
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
        this->set_text(this->current_option());
        this->option_changed(this->current_option().str());
        options_.front().enabled();
        this->update();
    }
}

void Cycle_box::cycle_backward() {
    if (options_.size() > 1) {
        auto begin = std::rbegin(options_);
        std::rotate(begin, begin + 1, std::rend(options_));
        this->set_text(this->current_option());
        this->option_changed(this->current_option().str());
        options_.front().enabled();
        this->update();
    }
}

bool Cycle_box::mouse_press_event(Mouse_button button,
                                  std::size_t global_x,
                                  std::size_t global_y,
                                  std::size_t local_x,
                                  std::size_t local_y,
                                  std::uint8_t device_id) {
    if (button == Mouse_button::Left || button == Mouse_button::ScrollUp) {
        this->cycle_forward();
    } else if (button == Mouse_button::ScrollDown) {
        this->cycle_backward();
    }
    return Label::mouse_press_event(button, global_x, global_y, local_x,
                                    local_y, device_id);
}

Cycle_box::Option::Option(Glyph_string name_) : name{std::move(name_)} {}

// void Cycle_box::resize_label() {
//     label.width_policy.hint(label.contents().size() + 2);
//     label.width_policy.type(Size_policy::Fixed);
// }

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
