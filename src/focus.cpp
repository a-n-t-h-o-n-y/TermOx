#include <ox/focus.hpp>

#include <algorithm>
#include <stdexcept>

namespace ox {
namespace {
auto eligible(std::span<WidgetNode const> tree, LifetimeView<Widget> target) -> bool
{
    return std::ranges::any_of(tree, [&](auto const& node) {
        return node.widget == target && node.input && node.visible.size.width > 0 &&
               node.visible.size.height > 0 && target.valid() &&
               target.get().focus_policy != FocusPolicy::None;
    });
}
}  // namespace

void Focus::report(EventResponse response)
{
    if (response.repaint) { loop_.request_repaint(); }
    if (response.quit && !loop_.exit_code()) { loop_.quit(*response.quit); }
}

auto Focus::clear() -> EventResponse
{
    ++revision_;
    recover_ = false;
    auto old = in_focus_;
    in_focus_ = {};
    auto response = EventResponse{};
    if (old.valid()) {
        old.get().focused = false;
        response = combine({.repaint = true}, old.get().focus_out());
    }
    report(response);
    return response;
}

auto Focus::set(Widget& widget) -> EventResponse
{
    auto target = track(widget);
    if (!eligible(widget_tree(root_), target)) {
        throw std::invalid_argument{"Focus target is not eligible in this application"};
    }
    if (in_focus_ == target) { return {}; }
    auto const revision = revision_ + 1;
    auto response = clear();
    // Outgoing callbacks may remove, disable, or focus another widget.
    if (response.quit || revision_ != revision || in_focus_.valid()) {
        return response;
    }
    if (!eligible(widget_tree(root_), target)) {
        recover_ = true;
        auto const tree = widget_tree(root_);
        return combine(response, reconcile(tree));
    }
    in_focus_ = target;
    recover_ = true;
    target.get().focused = true;
    response = combine(response, combine({.repaint = true}, target.get().focus_in()));
    if (revision_ != revision) { recover_ = in_focus_.valid(); }
    report(response);
    return response;
}

auto Focus::reconcile(std::span<WidgetNode const> tree) -> EventResponse
{
    if (!recover_ || eligible(tree, in_focus_)) { return {}; }
    auto const order = focus_order(tree);
    if (!order.empty()) { return set(tree[order.front()].widget.get()); }
    auto const revision = revision_ + 1;
    auto response = clear();
    if (revision_ == revision) { recover_ = true; }
    return response;
}

auto Focus::advance(std::span<WidgetNode const> tree, bool backward) -> EventResponse
{
    auto const order = focus_order(tree);
    if (order.empty()) { return clear(); }
    auto const current = std::ranges::find_if(
        order, [&](auto index) { return tree[index].widget == in_focus_; });
    auto index = backward ? order.size() - 1 : std::size_t{0};
    if (current != order.end()) {
        auto const offset = static_cast<std::size_t>(current - order.begin());
        index = backward ? (offset + order.size() - 1) % order.size()
                         : (offset + 1) % order.size();
    }
    return set(tree[order[index]].widget.get());
}
}  // namespace ox
