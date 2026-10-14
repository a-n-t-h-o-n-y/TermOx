#include <ox/focus.hpp>

#include <algorithm>
#include <ranges>
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
auto tab_candidate(WidgetNode const& node) -> bool
{
    if (!node.input || node.visible.size.width == 0 || node.visible.size.height == 0 ||
        !node.widget.valid()) {
        return false;
    }
    auto const policy = node.widget.get().focus_policy;
    return policy == FocusPolicy::Tab || policy == FocusPolicy::Strong;
}
}  // namespace

Focus::~Focus()
{
    if (auto* widget = in_focus_.try_get()) { widget->focused_ = false; }
}

auto Focus::clear() -> EventResponse
{
    ++revision_;
    recover_ = false;
    auto old = in_focus_;
    in_focus_ = {};
    auto response = EventResponse{};
    if (old.valid()) {
        old.get().focused_ = false;
        response = combine({.repaint = true}, old.get().focus_out());
    }
    return response;
}

auto Focus::set(Widget& widget) -> EventResponse
{
    auto target = track(widget);
    auto tree = widget_tree(root_);
    if (!eligible(tree, target)) {
        throw std::invalid_argument{"Focus target is not eligible in this application"};
    }
    if (in_focus_ == target) { return {}; }
    auto const revision = revision_ + 1;
    auto response = clear();
    // Outgoing callbacks may remove, disable, or focus another widget.
    if (response.quit || revision_ != revision || in_focus_.valid()) {
        return response;
    }
    widget_tree(root_, tree);
    if (!eligible(tree, target)) {
        recover_ = true;
        return combine(response, reconcile(tree));
    }
    in_focus_ = target;
    recover_ = true;
    target.get().focused_ = true;
    response = combine(response, combine({.repaint = true}, target.get().focus_in()));
    if (revision_ != revision) { recover_ = in_focus_.valid(); }
    return response;
}

auto Focus::reconcile(std::span<WidgetNode const> tree) -> EventResponse
{
    if (!recover_ || eligible(tree, in_focus_)) { return {}; }
    auto const first = std::ranges::find_if(tree, tab_candidate);
    if (first != tree.end()) { return set(first->widget.get()); }
    auto const revision = revision_ + 1;
    auto response = clear();
    if (revision_ == revision) { recover_ = true; }
    return response;
}

auto Focus::advance(std::span<WidgetNode const> tree, bool backward) -> EventResponse
{
    auto candidates = tree | std::views::filter(tab_candidate);
    auto const first = candidates.begin();
    auto const end = candidates.end();
    if (first == end) { return clear(); }
    auto target = std::ranges::find(candidates, in_focus_, &WidgetNode::widget);
    if (backward) {
        if (target == first) { target = end; }
        --target;
    }
    else {
        if (target == end) { target = first; }
        else if (++target == end) {
            target = first;
        }
    }
    // Consume the borrowed candidate before set invokes callbacks.
    return set(target->widget.get());
}
}  // namespace ox
