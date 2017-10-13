#include "widget/widgets/widget_stack.hpp"
#include "system/focus.hpp"
#include "widget/widget.hpp"

#include <signals/slot.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace cppurses {

void Widget_stack::set_active_page(std::size_t index) {
    if (pages_[index].get() == nullptr) {
        return;
    }
    // Remove current child & place back into pages_ container.
    std::unique_ptr<Widget> removed{this->remove_child(active_page_)};
    if (has_focus(*removed)) {
        Focus::clear_focus();
    }
    auto at = std::find(std::begin(pages_), std::end(pages_), nullptr);
    if (at != std::end(pages_)) {
        *at = std::move(removed);
    }

    // Set new active page from index.
    active_page_ = pages_[index].get();
    this->add_child(std::move(pages_[index]));
    pages_[index] = nullptr;
    Focus::set_focus_to(active_page_);
    this->page_changed(index);
}

void Widget_stack::add_page(std::unique_ptr<Widget> widget) {
    pages_.push_back(std::move(widget));
    // if (this->size() == 1) {
    //     this->set_active_page(0);
    // }
}

void Widget_stack::insert_page(std::size_t index,
                               std::unique_ptr<Widget> widget) {
    pages_.insert(std::begin(pages_) + index, std::move(widget));
    // if (this->size() == 1) {
    //     this->set_active_page(0);
    // }
}

std::unique_ptr<Widget> Widget_stack::remove_page(std::size_t index) {
    std::unique_ptr<Widget> removed{std::move(pages_[index])};
    if (removed == nullptr) {  // Remove the active page.
        removed = this->remove_child(active_page_);
    }
    pages_.erase(std::begin(pages_) + index);
    return removed;
}

std::size_t Widget_stack::size() const {
    return pages_.size();
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
