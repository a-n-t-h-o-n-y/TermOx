#include <cppurses/widget/widgets/widget_stack.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

// namespace {
// using namespace cppurses;

// void set_page_visible(Widget* page, bool visible) {
//     page->set_visible(visible);
//     for (const std::unique_ptr<Widget>& child : page->children.get()) {
//         set_page_visible(child.get(), visible);
//     }
// }

// }  // namespace

namespace cppurses {

void Widget_stack::set_active_page(std::size_t index) {
    if (this->size() == 0 || index > this->size()) {
        return;
    }
    // Disable the active page
    if (active_page_ != nullptr) {
        active_page_->disable();
    }
    // Enable the new active page
    active_page_ = this->children.get()[index].get();
    if (active_page_->parent() != nullptr &&
        active_page_->parent()->enabled()) {
        active_page_->enable();
    }

    if (sets_focus_) {
        Focus::set_focus_to(active_page_);
    }
    this->page_changed(index);
    // this->update();
}

void Widget_stack::sets_focus_on_change(bool sets_focus) {
    sets_focus_ = sets_focus;
}

void Widget_stack::add_page(std::unique_ptr<Widget> widget) {
    this->children.add(std::move(widget));
    this->children.get().back()->disable();
}

void Widget_stack::insert_page(std::size_t index,
                               std::unique_ptr<Widget> widget) {
    this->children.insert(std::move(widget), index);
    this->children.get()[index]->disable();
}

void Widget_stack::remove_page(std::size_t index) {
    if (index >= this->size()) {
        return;
    }
    Widget* at_index{this->children.get()[index].get()};
    if (at_index == this->active_page()) {
        active_page_ = nullptr;
    }
    at_index->close();
}

void Widget_stack::clear() {
    active_page_ = nullptr;
    for (const std::unique_ptr<Widget>& w : this->children.get()) {
        if (w != nullptr) {
            w->close();
        }
    }
}

std::size_t Widget_stack::size() const {
    return this->children.get().size();
}

Widget* Widget_stack::active_page() const {
    return active_page_;
}

std::size_t Widget_stack::active_page_index() const {
    if (active_page_ == nullptr) {
        return 0;  // TODO need better value here.
    }
    const auto& child_vec = this->children.get();

    auto at = std::find_if(
        std::begin(child_vec), std::end(child_vec),
        [this](const auto& w_ptr) { return w_ptr.get() == active_page_; });
    return std::distance(std::begin(child_vec), at);
}

void Widget_stack::enable(bool enable, bool post_child_polished_event) {
    this->enable_and_post_events(enable, post_child_polished_event);
    if (active_page_ != nullptr) {
        active_page_->enable(enable, post_child_polished_event);
    }
}

// bool Widget_stack::paint_event() {
//     for (const std::unique_ptr<Widget>& page : this->children.get()) {
//         if (active_page_ == page.get() && this->visible()) {
//             set_page_visible(page.get(), true);
//         } else {
//             set_page_visible(page.get(), false);
//         }
//     }
//     return Horizontal_layout::paint_event();
// }

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
