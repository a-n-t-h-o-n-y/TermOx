#pragma once
#include <ox/layout.hpp>
#include <ox/routing.hpp>
#include <ox/selection.hpp>
namespace ox {
struct FormField {
    std::string label;
    std::string help = {};
    std::function<std::optional<std::string>(Widget const&)> validate = {};
};
/** Own ordinary tuple/range fields; validators return a message or nullopt. */
template <LayoutContainer Container>
class Form : public Widget {
   public:
    using Widget::handle;
    Container children;
    Style style = {};
    Style error_style = {.foreground = colors::red};
    int gap = 1;
    std::optional<int> label_width;
    sl::Signal<void()> on_submit;
    sl::Signal<void(std::vector<std::optional<std::string>> const&)> on_invalid;
    Form(Container children, std::vector<FormField> fields)
        : Widget{}, children{std::move(children)}
    {
        set_fields(std::move(fields));
        auto lifetime = track(*this);
        shortcuts = [lifetime](KeyEvent const& event) {
            auto* self = lifetime.try_get();
            if (!self ||
                !event.matches(KeyCode::Enter, {.ctrl = true}, KeyAction::Press)) {
                return EventResponse{};
            }
            auto tree = widget_tree(*self);
            auto focused = std::ranges::any_of(tree, [](auto const& n) {
                return n.widget.valid() && n.widget.get().is_focused();
            });
            return focused ? self->handle(event) : EventResponse{};
        };
    }
    auto fields() const -> std::span<FormField const> { return fields_; }
    auto errors() const -> std::span<std::optional<std::string> const>
    { return errors_; }
    void set_fields(std::vector<FormField> fields)
    {
        std::size_t count = 0;
        detail::for_each_child(children, [&](Widget const&) { ++count; });
        if (fields.size() != count) throw std::invalid_argument{"Form field count"};
        for (auto const& f : fields) {
            validate_row_text(f.label);
            validate_row_text(f.help);
        }
        fields_ = std::move(fields);
        errors_.assign(count, {});
        layouts_.resize(count);
        refresh();
    }
    auto submit() -> bool
    {
        auto lifetime = track(*this);
        auto validators = fields_;
        auto widgets = std::vector<LifetimeView<Widget>>{};
        detail::for_each_child(children,
                               [&](Widget& w) { widgets.push_back(track(w)); });
        auto errors = std::vector<std::optional<std::string>>(validators.size());
        for (std::size_t i = 0; i < validators.size(); ++i) {
            if (!lifetime.valid() || !widgets[i].valid()) return false;
            if (validators[i].validate) {
                errors[i] = validators[i].validate(widgets[i].get());
                if (errors[i]) validate_row_text(*errors[i]);
            }
        }
        auto* self = lifetime.try_get();
        if (!self) return false;
        self->errors_ = errors;
        auto invalid = std::find_if(errors.begin(), errors.end(),
                                    [](auto const& e) { return e.has_value(); });
        auto valid = invalid == errors.end();
        self->refresh();
        self = lifetime.try_get();
        if (!self) return false;
        if (valid)
            self->on_submit();
        else {
            auto target = widgets[(std::size_t)(invalid - errors.begin())];
            if (target.valid()) {
                auto tree = widget_tree(target.get());
                auto order = focus_order(tree);
                if (!order.empty()) {
                    self->request_focus(tree[order.front()].widget.get());
                }
            }
            if ((self = lifetime.try_get())) { self->on_invalid(errors); }
        }
        return valid;
    }
    auto get_children() -> zzz::Generator<Widget&> override
    { return detail::child_references(children); }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { return detail::child_references(children); }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (is_enabled() &&
            e.matches(KeyCode::Enter, {.ctrl = true}, KeyAction::Press)) {
            submit();
            return {.handled = true, .repaint = true};
        }
        return {};
    }
    void refresh() override
    {
        Widget::refresh();
        on_arrange(bounds());
    }
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    {
        validate(a);
        check();
        auto labels = label_extent(a, p);
        std::int64_t width = labels, height = 0;
        std::size_t i = 0;
        detail::for_each_child(children, [&](Widget const& w) {
            auto s = w.measure({std::max(0, a.width - labels), a.height}, p);
            width = std::max(width, (std::int64_t)labels + s.width);
            height += std::max(1, s.height) + (message(i).empty() ? 0 : 1);
            if (i++) height += gap;
        });
        if (width > std::numeric_limits<int>::max() ||
            height > std::numeric_limits<int>::max())
            throw std::length_error{"Form extent overflow"};
        return {(int)width, (int)height};
    }
    auto paint(Canvas c) -> Cursor override
    {
        fill(c, appearance(style));
        auto labels = label_extent(c.size(), c.width_policy());
        for (std::size_t i = 0; i < rects_.size(); ++i) {
            auto r = rects_[i];
            paint_row_text(
                c.subcanvas({0, r.at.y}, {std::max(0, labels - (labels > 0 ? 1 : 0)),
                                          std::min(1, r.size.height)}),
                fields_[i].label, Align::Left, appearance(style), layouts_[i]);
            auto msg = message(i);
            if (!msg.empty())
                paint_row_text(c.subcanvas({labels, r.at.y + r.size.height},
                                           {std::max(0, size().width - labels), 1}),
                               msg, Align::Left,
                               appearance(errors_[i] ? error_style : style),
                               layouts_[i]);
        }
        return {};
    }

   protected:
    void on_arrange(Rect) override
    {
        check();
        auto labels = label_extent(size(), glyph_width_policy());
        auto policies = std::vector<SizePolicy>{};
        auto extra = 0;
        std::size_t i = 0;
        detail::for_each_child(children, [&](Widget const& w) {
            auto p = w.height_policy;
            auto natural =
                w.measure({std::max(0, size().width - labels), size().height},
                          glyph_width_policy())
                    .height;
            p.minimum = std::max(p.minimum, std::min(p.maximum, std::max(1, natural)));
            policies.push_back(p);
            if (!message(i++).empty()) ++extra;
        });
        auto rects = layout_column(
            policies,
            {std::max(0, size().width - labels), std::max(0, size().height - extra)},
            gap);
        auto shift = 0;
        i = 0;
        for (auto& r : rects) {
            r.at.x = labels;
            r.at.y += shift;
            if (!message(i++).empty()) ++shift;
        }
        rects_ = rects;
        auto widgets = std::vector<LifetimeView<Widget>>{};
        detail::for_each_child(children,
                               [&](Widget& w) { widgets.push_back(track(w)); });
        auto lifetime = track(*this);
        for (std::size_t j = 0; j < widgets.size(); ++j) {
            if (!lifetime.valid()) return;
            if (widgets[j].valid()) arrange(widgets[j].get(), rects[j]);
        }
    }

   private:
    std::vector<FormField> fields_;
    std::vector<std::optional<std::string>> errors_;
    std::vector<Rect> rects_;
    std::vector<TextLayout> layouts_;
    void check() const
    {
        if (gap < 0 || (label_width && *label_width < 0))
            throw std::invalid_argument{"Form layout options"};
        std::size_t n = 0;
        detail::for_each_child(children, [&](Widget const&) { ++n; });
        if (n != fields_.size())
            throw std::invalid_argument{"Form field count changed"};
    }
    auto message(std::size_t i) const -> std::string_view
    {
        return errors_[i] ? std::string_view{*errors_[i]}
                          : std::string_view{fields_[i].help};
    }
    auto label_extent(Area a, esc::WidthPolicy p) const -> int
    {
        if (label_width) return std::min(a.width, *label_width);
        int width = 0;
        for (auto const& f : fields_)
            width = std::max(width, measure_text(f.label, a, TextWrap::None, p).width);
        return (int)std::min<std::int64_t>(a.width,
                                           (std::int64_t)width + (width > 0 ? 1 : 0));
    }
};
}  // namespace ox
