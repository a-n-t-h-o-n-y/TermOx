#include "system/focus.hpp"
#include "system/events/focus_event.hpp"
#include "system/system.hpp"
#include "widget/widget.hpp"
#include "widget/focus_policy.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace {

using namespace cppurses;

Widget* next_tab_focus() {
    if (System::head() == nullptr) {
        return nullptr;
    }
    std::vector<Widget*> widgets;

    // Populate widgets vector
    widgets.push_back(System::head());
    int i{0};
    while (i < widgets.size()) {
        Widget* current = widgets[i];
        auto children = current->children();
        // std::copy instead
        std::for_each(std::begin(children), std::end(children),
                      [&widgets](Widget* p) { widgets.push_back(p); });
        ++i;
    }

    // Rearrange widgets vector
    const Widget* const current_focus_widget = Focus::focus_widget();
    if (current_focus_widget != nullptr) {
        auto current_iter = std::find(std::begin(widgets), std::end(widgets),
                                      current_focus_widget);
        if (current_iter != std::end(widgets)) {
            std::move(std::begin(widgets), current_iter + 1,
                      std::back_inserter(widgets));
        }
    }

    // Find the next focus widget
    for (Widget* child : widgets) {
        if (child != nullptr) {
            if (child->focus_policy == Focus_policy::Tab ||
                child->focus_policy == Focus_policy::Strong) {
                return child;
            }
        }
    }
    return nullptr;
}

}  // namespace

namespace cppurses {

Widget* Focus::focus_widget_ = nullptr;

Widget* Focus::focus_widget() {
    return focus_widget_;
}

void Focus::mouse_press(Widget* clicked) {
    if (clicked->focus_policy == Focus_policy::Click ||
        clicked->focus_policy == Focus_policy::Strong) {
        Focus::set_focus_to(clicked);
    }
}

bool Focus::tab_press() {
    if (focus_widget_->focus_policy == Focus_policy::Tab ||
        focus_widget_->focus_policy == Focus_policy::Strong) {
        Widget* next = next_tab_focus();
        Focus::set_focus_to(next);
        return true;
    }
    return false;
}

void Focus::set_focus_to(Widget* new_focus) {
    if (new_focus == nullptr || new_focus->focus_policy == Focus_policy::None) {
        return;
    }
    if (focus_widget_ != nullptr) {
        std::unique_ptr<Focus_out_event> event{
            new Focus_out_event(focus_widget_)};
        System::post_event(std::move(event));
    }
    focus_widget_ = new_focus;
    if (focus_widget_ != nullptr) {
        std::unique_ptr<Focus_in_event> event{
            new Focus_in_event(focus_widget_)};
        System::post_event(std::move(event));
    }
}

void Focus::clear_focus() {
    if (focus_widget_ != nullptr) {
        std::unique_ptr<Focus_out_event> event{
            new Focus_out_event(focus_widget_)};
        System::post_event(std::move(event));
    }
    focus_widget_ = nullptr;
}

}  // namespace cppurses
