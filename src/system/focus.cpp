#include <cppurses/system/focus.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <cppurses/system/event.hpp>
#include <cppurses/system/events/focus_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace {

using namespace cppurses;

Widget* next_tab_focus() {
    if (System::head() == nullptr) {
        return nullptr;
    }
    auto widg_tree = System::head()->children.get_descendants();
    widg_tree.insert(std::begin(widg_tree), System::head());
    auto* const focus_widg = Focus::focus_widget();
    if (focus_widg != nullptr) {
        const auto iter =
            std::find(std::begin(widg_tree), std::end(widg_tree), focus_widg);
        if (iter != std::end(widg_tree)) {
            std::rotate(std::begin(widg_tree), iter, std::end(widg_tree));
        }
    }
    auto is_focusable = [](const auto* widg) {
        return widg->enabled() && (widg->focus_policy == Focus_policy::Tab ||
                                   widg->focus_policy == Focus_policy::Strong);
    };
    const auto next_focus = std::find_if(std::next(std::begin(widg_tree)),
                                         std::end(widg_tree), is_focusable);
    return next_focus != std::end(widg_tree) ? *next_focus : focus_widg;
}

}  // namespace

namespace cppurses {

Widget* Focus::focus_widget_ = nullptr;

void Focus::mouse_press(Widget* clicked) {
    if (clicked == focus_widget_) {
        return;
    }
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
    if (new_focus == focus_widget_) {
        return;
    }
    if (new_focus == nullptr || new_focus->focus_policy == Focus_policy::None) {
        Focus::clear_focus();
        return;
    }
    if (focus_widget_ != nullptr) {
        // Focus_out_event has private constructor, can't use make_unique.
        std::unique_ptr<Focus_out_event> event{
            new Focus_out_event(focus_widget_)};
        System::post_event(std::move(event));
    }
    focus_widget_ = new_focus;
    std::unique_ptr<Focus_in_event> event{new Focus_in_event(focus_widget_)};
    System::post_event(std::move(event));
}

void Focus::clear_focus() {
    if (focus_widget_ != nullptr) {
        std::unique_ptr<Focus_out_event> event{
            new Focus_out_event(focus_widget_)};
        System::post_event(std::move(event));
        focus_widget_ = nullptr;
    }
}

}  // namespace cppurses
