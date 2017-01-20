#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "border.hpp"
#include "size_policy.hpp"

#include <system_module/object.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <system_module/events/move_event.hpp>
#include <system_module/events/mouse_event.hpp>
#include <system_module/events/key_event.hpp>
#include <system_module/events/close_event.hpp>
#include <system_module/events/hide_event.hpp>
#include <system_module/events/show_event.hpp>
#include <system_module/events/enable_event.hpp>
#include <system_module/events/focus_event.hpp>
#include <system_module/system.hpp>
#include <painter_module/brush.hpp>
#include <painter_module/detail/ncurses_paint_engine.hpp>
#include <painter_module/geometry.hpp>
#include <painter_module/paint_engine.hpp>

#include <aml/signals/slot.hpp>

#include <cstddef>
#include <functional>
#include <memory>

namespace twf {

class Widget : public Object {
   public:
    // Types
    enum class Focus_policy { TabFocus, ClickFocus, StrongFocus, NoFocus };

    Widget();
    ~Widget() override = default;

    virtual void update();

    bool has_coordinates(std::size_t global_x, std::size_t global_y) override;

    void set_x(std::size_t x) { x_ = x; }
    void set_y(std::size_t y) { y_ = y; }
    void set_cursor(bool show) { show_cursor_ = show; }
    void set_cursor_x(std::size_t x) { cursor_x_ = x; }
    void set_cursor_y(std::size_t y) { cursor_y_ = y; }
    std::size_t cursor_x() const { return cursor_x_; }
    std::size_t cursor_y() const { return cursor_y_; }
    void set_paint_engine(std::unique_ptr<Paint_engine> engine) {
        paint_engine_ = std::move(engine);
    }

    // Global Coordinateold_y_ws
    std::size_t x() const;
    std::size_t y() const;

    bool cursor() const { return show_cursor_; }
    void set_focus(bool focus);
    void clear_focus() { this->set_focus(false); }
    bool has_focus() const { return focus_; }
    Focus_policy focus_policy() const { return focus_policy_; }
    void set_focus_policy(Focus_policy policy) { focus_policy_ = policy; }
    bool has_mouse_tracking() const { return mouse_tracking_; }
    void set_mouse_tracking(bool track) { mouse_tracking_ = track; }
    void set_brush(Brush brush) {
        default_brush_ = std::move(brush);
        this->update();
    }
    void set_brush_recursively(const Brush& brush);
    Brush& brush() { return default_brush_; }
    bool visible() const { return visible_; }
    void set_visible(bool visible);
    Border& border() { return border_; }
    const Border& border() const { return border_; }
    bool has_border() const { return border_.enabled(); }
    void enable_border();
    void disable_border();
    void set_geometry(const Geometry& g);
    Geometry& geometry() { return geometry_; }
    const Geometry& geometry() const { return geometry_; }
    Size_policy& size_policy() { return size_policy_; }
    const Size_policy& size_policy() const { return size_policy_; }

    Paint_engine& paint_engine() const;

    void erase_widget_screen();

    // Signals

    // Slots
    sig::Slot<void()> close;
    sig::Slot<void()> hide;
    sig::Slot<void()> show;
    sig::Slot<void()> repaint;
    sig::Slot<void()> give_focus;
    sig::Slot<void()> update_me;

   protected:
    bool event(const Event& event) override;
    virtual bool move_event(const Move_event& event);
    virtual bool resize_event(const Resize_event& event);
    virtual bool paint_event(const Paint_event& event);
    virtual bool mouse_press_event(const Mouse_event& event);
    virtual bool mouse_release_event(const Mouse_event& event);
    virtual bool mouse_double_click_event(const Mouse_event& event);
    virtual bool wheel_event(const Mouse_event& event);
    virtual bool mouse_move_event(const Mouse_event& event);
    virtual bool key_press_event(const Key_event& event);
    virtual bool key_release_event(const Key_event& event);
    virtual bool close_event(const Close_event& event);
    virtual bool hide_event(const Hide_event& event);
    virtual bool show_event(const Show_event& event);
    bool enable_event(const Enable_event& event) override;
    virtual bool focus_event(const Focus_event& event);
    bool child_event(const Child_event& event) override;

    // Top left corner coordinates relative to parent
    std::size_t x_ = 0;
    std::size_t y_ = 0;

    bool show_cursor_ = true;
    std::size_t cursor_x_ = 0;
    // create cursor object to hold x, y and bool show ^^
    std::size_t cursor_y_ = 0;
    bool focus_ = false;
    bool mouse_tracking_ = false;
    bool visible_ = true;

    Focus_policy focus_policy_ = Focus_policy::NoFocus;
    Brush default_brush_ =
        Brush(background(Color::Black), foreground(Color::White));
    Border border_;
    Size_policy size_policy_;

    std::unique_ptr<Paint_engine> paint_engine_ = nullptr;
    Geometry geometry_ = Geometry{this};

   private:
    void paint_disabled_widget();
    void initialize();
};

}  // namespace twf

#endif  // WIDGET_HPP
