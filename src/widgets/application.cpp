#include <termox/widgets/application.hpp>

#include <termox/widgets/widget.hpp>

namespace {
using namespace ox::widgets;

auto do_tab_focus_change(Widget& head, Widget const* current_focus) -> void
{
    Widget* next =
        find_if(head, [found = false, current_focus](Widget const& w) mutable {
            if (found) {
                return w.properties.focus_policy == FocusPolicy::Strong ||
                       w.properties.focus_policy == FocusPolicy::Tab;
            }
            else {
                if (&w == current_focus) {
                    found = true;
                }
                return false;
            }
        });

    if (next == nullptr) {
        next = find_if(head, [](Widget const& w) {
            return w.properties.focus_policy == FocusPolicy::Strong ||
                   w.properties.focus_policy == FocusPolicy::Tab;
        });
    }

    if (next != nullptr && next != current_focus) {
        Focus::set(*next);
    }
}

auto do_shift_tab_focus_change(Widget& head,
                               Widget const* current_focus) -> void
{
    Widget* next = nullptr;

    for_each(head, [&next, previous = (Widget*)nullptr,
                    current_focus](Widget& w) mutable {
        if (&w == current_focus) {
            next = previous;
        }
        if (w.properties.focus_policy == FocusPolicy::Strong ||
            w.properties.focus_policy == FocusPolicy::Tab) {
            previous = &w;
        }
    });

    if (next == nullptr) {
        for_each(head, [&next](Widget& w) {
            if (w.properties.focus_policy == FocusPolicy::Strong ||
                w.properties.focus_policy == FocusPolicy::Tab) {
                next = &w;
            }
        });
    }

    if (next != nullptr && next != current_focus) {
        Focus::set(*next);
    }
}

}  // namespace

namespace ox::widgets {

auto Application::run() -> int
{
    ox::Terminal::event_queue.append(esc::Resize{term_.size()});
    return process_events(term_, *this);
}

auto Application::handle_key_press(ox::Key k) -> ox::EventResponse
{
    // TODO a shortcuts manager? processed here?
    Widget* const focused = Focus::get();
    if (focused != nullptr) {
        if (focused->properties.focus_policy == FocusPolicy::Strong ||
            focused->properties.focus_policy == FocusPolicy::Tab) {
            if (k == ox::Key::Tab) {
                do_tab_focus_change(head_, focused);
            }
            else if (k == ox::Key::BackTab) {
                do_shift_tab_focus_change(head_, focused);
            }
            else {
                key_press(*focused, k);
            }
        }
        else {
            key_press(*focused, k);
        }
    }
    paint(head_, ox::Canvas{.at = {0, 0}, .size = term_.changes.size()});
    return {};
}

}  // namespace ox::widgets