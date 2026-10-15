#pragma once
#include <algorithm>
#include <ox/application.hpp>
#include <ox/bordered.hpp>
#include <ox/routing.hpp>
namespace ox {
enum class PopupPlacement { Center, Below, Right };
/** Root overlay with an opaque content rectangle; anchor is borrowed and follows moves.
 */
template <WidgetDerived Child>
class Popup : public Widget {
   public:
    using Widget::handle;
    Child child;
    LifetimeView<Widget> anchor;
    PopupPlacement placement = PopupPlacement::Center;
    Point offset = {};
    std::optional<Area> requested_size;
    bool close_on_outside = true;
    bool close_on_escape = true;
    bool close_on_tab = true;
    sl::Signal<void()> on_open;
    sl::Signal<void()> on_close;
    explicit Popup(Child child) : Widget{FocusPolicy::Click}, child{std::move(child)}
    {
        visible = false;
        overlay = true;
        modal = true;
        auto lifetime = track(*this);
        shortcuts = [lifetime](KeyEvent const& event) {
            auto* self = lifetime.try_get();
            if (self && self->visible && self->close_on_tab &&
                event.action != KeyAction::Release &&
                (event.matches(KeyCode::Tab, {}, event.action) ||
                 event.matches(KeyCode::Tab, {.shift = true}, event.action))) {
                return EventResponse{.repaint = self->hide()};
            }
            return EventResponse{};
        };
    }
    auto is_open() const -> bool { return visible; }
    auto show() -> bool
    {
        if (visible) { return false; }
        visible = true;
        bring_to_front();
        auto lifetime = track(*this);
        refresh();
        if (auto* self = lifetime.try_get()) { self->on_open(); }
        return true;
    }
    auto hide() -> bool
    {
        if (!visible) { return false; }
        visible = false;
        request_layout();
        request_repaint();
        on_close();
        return true;
    }
    void refresh() override
    {
        request_layout();
        arrange(*this, {{}, screen_size()});
    }
    void paint_background(Canvas canvas) override
    {
        // Layouts and borders can leave gaps. Clear the whole content rectangle
        // before painting descendants so those gaps cannot expose lower layers.
        fill(canvas.subcanvas(child.at(), child.size()),
             U' ' | appearance({}, Surface::Raised));
    }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (close_on_escape && e.matches(KeyCode::Escape, {}, KeyAction::Press)) {
            return {.handled = true, .repaint = hide()};
        }
        return {};
    }
    auto handle(MousePress const& e) -> EventResponse override
    {
        if (close_on_outside && !contains(child.bounds(), e.at)) {
            auto handled = modal;
            return {.handled = handled, .repaint = hide()};
        }
        return {.handled = modal};
    }

   protected:
    void on_arrange(Rect) override
    {
        auto natural = requested_size ? *requested_size
                                      : child.measure(size(), glyph_width_policy());
        validate(natural);
        auto extent = Area{std::min(natural.width, size().width),
                           std::min(natural.height, size().height)};
        auto x = (std::int64_t)(size().width - extent.width) / 2;
        auto y = (std::int64_t)(size().height - extent.height) / 2;
        if (placement != PopupPlacement::Center && anchor.valid()) {
            auto rect = anchor.get().global_bounds();
            x = (std::int64_t)rect.at.x +
                (placement == PopupPlacement::Right ? rect.size.width : 0);
            y = (std::int64_t)rect.at.y +
                (placement == PopupPlacement::Below ? rect.size.height : 0);
            if (placement == PopupPlacement::Below &&
                y + extent.height > size().height) {
                y = (std::int64_t)rect.at.y - extent.height;
            }
            if (placement == PopupPlacement::Right && x + extent.width > size().width) {
                x = (std::int64_t)rect.at.x - extent.width;
            }
        }
        auto at = Point{(int)std::clamp(x + offset.x, std::int64_t{0},
                                        (std::int64_t)size().width - extent.width),
                        (int)std::clamp(y + offset.y, std::int64_t{0},
                                        (std::int64_t)size().height - extent.height)};
        arrange(child, {at, extent});
    }
};
enum class DialogResult { Accepted, Rejected };
template <WidgetDerived Child>
class Dialog : public Popup<Bordered<Child>> {
   public:
    using Popup<Bordered<Child>>::handle;
    sl::Signal<void(DialogResult)> on_result;
    explicit Dialog(Child child, std::string title = {})
        : Popup<Bordered<Child>>{std::move(child) |
                                 Border{.label = {.text = std::move(title)}}}
    {
        this->close_on_outside = false;
        this->close_on_tab = false;
    }
    void accept() { finish(DialogResult::Accepted); }
    void reject() { finish(DialogResult::Rejected); }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (this->close_on_escape && e.matches(KeyCode::Escape, {}, KeyAction::Press)) {
            reject();
            return {.handled = true, .repaint = true};
        }
        return {};
    }

   private:
    void finish(DialogResult result)
    {
        if (!this->is_open()) { return; }
        auto lifetime = track(*this);
        this->hide();
        if (auto* self = lifetime.try_get()) { self->on_result(result); }
    }
};
}  // namespace ox
