#include <cppurses/widget/widgets/widget_stack.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Widget_stack::set_active_page(std::size_t index) {
    if (pages_.empty()) {
        return;
    }
    if (pages_[index] == nullptr) {
        return;
    }
    // Remove current child & place back into pages_ container.
    // TODO post show and hide events instead of adding and removing
    std::unique_ptr<Widget> removed{this->children.remove(active_page_)};
    if (has_focus(*removed)) {
        Focus::clear_focus();
    }
    auto at = std::find(std::begin(pages_), std::end(pages_), nullptr);
    if (at != std::end(pages_)) {
        *at = std::move(removed);
    }

    // Set new active page from index.
    active_page_ = pages_[index].get();
    this->children.add(std::move(pages_[index]));
    pages_[index] = nullptr;
    if (sets_focus_) {
        Focus::set_focus_to(active_page_);
    }
    this->page_changed(index);
}

void Widget_stack::sets_focus_on_change(bool sets_focus) {
    sets_focus_ = sets_focus;
}

void Widget_stack::add_page(std::unique_ptr<Widget> widget) {
    pages_.push_back(std::move(widget));
    if (pages_.size() == 1) {
        // this->set_active_page(0);
    }
}

void Widget_stack::insert_page(std::size_t index,
                               std::unique_ptr<Widget> widget) {
    pages_.insert(std::begin(pages_) + index, std::move(widget));
    if (pages_.size() == 1) {
        // this->set_active_page(0);
    }
}

std::unique_ptr<Widget> Widget_stack::remove_page(std::size_t index) {
    std::unique_ptr<Widget> removed{pages_[index].release()};
    if (removed == nullptr) {  // If nullptr, then this index is the active page
        removed = this->children.remove(active_page_);
        active_page_ = nullptr;
    }
    pages_.erase(std::begin(pages_) + index);
    return removed;
}

void Widget_stack::clear() {
    auto active_at = std::find(std::begin(pages_), std::end(pages_), nullptr);
    if (active_at != std::end(pages_)) {
        this->remove_page(active_at - std::begin(pages_));
    }
    active_page_ = nullptr;
    pages_.clear();
    this->update();
}

std::size_t Widget_stack::size() const {
    return pages_.size();
}

Widget* Widget_stack::active_page() const {
    return active_page_;
}

std::size_t Widget_stack::active_page_index() const {
    if (pages_.empty()) {
        return 0;
    }
    auto at = std::find(std::begin(pages_), std::end(pages_), nullptr);
    return std::distance(std::begin(pages_), at);
}

namespace slot {

sig::Slot<void(std::size_t)> set_active_page(Widget_stack& stack) {
    sig::Slot<void(std::size_t)> slot{
        [&stack](auto index) { stack.set_active_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void()> set_active_page(Widget_stack& stack, std::size_t index) {
    sig::Slot<void()> slot{[&stack, index] { stack.set_active_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> remove_page(Widget_stack& stack) {
    sig::Slot<void(std::size_t)> slot{
        [&stack](auto index) { stack.remove_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void()> remove_page(Widget_stack& stack, std::size_t index) {
    sig::Slot<void()> slot{[&stack, index] { stack.remove_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> insert_page(
    Widget_stack& stack) {
    sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> slot{
        [&stack](auto index, auto widget) {
            stack.insert_page(index, std::move(widget));
        }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::unique_ptr<Widget>)> insert_page(Widget_stack& stack,
                                                     std::size_t index) {
    sig::Slot<void(std::unique_ptr<Widget>)> slot{[&stack, index](auto widget) {
        stack.insert_page(index, std::move(widget));
    }};
    slot.track(stack.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
