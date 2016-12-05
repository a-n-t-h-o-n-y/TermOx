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
#include <painter_module/geometry.hpp>
#include <painter_module/paint_engine.hpp>
#include <painter_module/detail/ncurses_paint_engine.hpp>
#include <painter_module/brush.hpp>

// #include <aml/signals/signal.hpp> // remove
#include <aml/signals/slot.hpp>

// #include <limits> // remove
#include <memory>
#include <functional>

namespace mcurses {

class Widget : public Object {
public:
	// Types
	enum class Focus_policy { TabFocus, ClickFocus, StrongFocus, NoFocus };

	Widget();
	virtual ~Widget() = default;
	
	virtual void update();

	bool has_coordinates(unsigned glob_x, unsigned glob_y) override;

	// void set_geometry(unsigned x, unsigned y, unsigned width, unsigned height); //??^^
	void set_x(unsigned x){ x_ = x; }
	void set_y(unsigned y){ y_ = y; }
	// void set_width(unsigned width) { width_ = width; } // remove
	// void set_height(unsigned height) { height_ = height; } // remove

	// void set_width_hint(unsigned width_hint) { width_hint_ = width_hint; Layout_param_changed(); } // remove
	// void set_height_hint(unsigned height_hint) { height_hint_ = height_hint; Layout_param_changed(); } // remove
	// void set_max_width(unsigned max_width) { max_width_ = max_width; Layout_param_changed(); } // remove
	// void set_max_height(unsigned max_height) { max_height_ = max_height; Layout_param_changed(); } // remove
	// void set_min_width(unsigned min_width) { min_width_ = min_width; Layout_param_changed(); } // remove
	// void set_min_width_hint(unsigned min_width_hint) { min_width_hint_ = min_width_hint; Layout_param_changed(); } // remove
	// void set_min_height(unsigned min_height) { min_height_ = min_height; Layout_param_changed(); } // remove
	// void set_min_height_hint(unsigned min_height_hint) { min_height_hint_ = min_height_hint; Layout_param_changed(); } // remove

	// void set_fixed_width(unsigned width);
	// void set_fixed_height(unsigned height);

	// void set_size_policy(const Size_policy& policy) { size_policy_ = policy; Layout_param_changed(); } // remove

	void set_cursor(bool show) { show_cursor_ = show; }
	void set_cursor_x(unsigned x) { cursor_x_ = x; }
	void set_cursor_y(unsigned y) { cursor_y_ = y; }
	unsigned cursor_x() const { return cursor_x_; }
	unsigned cursor_y() const { return cursor_y_; }
	void set_paint_engine(std::unique_ptr<Paint_engine> engine) { paint_engine_ = std::move(engine); }
	
	unsigned x() const { return x_; }
	unsigned y() const { return y_; }
	unsigned global_x() const { return find_global_x(); }
	unsigned global_y() const { return find_global_y(); }
	// unsigned width() const { return width_; } // remove
	// unsigned height() const { return height_; } // remove
	unsigned max_x() const { return x_ + this->geometry().height(); } // from within parent? do you need this? who uses it?
	unsigned max_y() const { return y_ + this->geometry().width(); } // can you simplify these?
	unsigned global_max_x() const { return this->global_x() + this->geometry().width(); } // can you simplify these?
	unsigned global_max_y() const { return this->global_y() + this->geometry().height(); } // can you simplify these?

	// unsigned width_hint() const { return width_hint_; } // remove
	// unsigned height_hint() const { return height_hint_; } // remove
	// unsigned max_width() const { return max_width_; } // remove
	// unsigned max_height() const { return max_height_; } // remove
	// unsigned min_width() const { return min_width_; } // remove
	// unsigned min_width_hint() const { return min_width_hint_; } // remove
	// unsigned min_height() const { return min_height_; } // remove
	// unsigned min_height_hint() const { return min_height_hint_; } // remove
	
	// Size_policy size_policy() const { return size_policy_; } // remove

	bool cursor() const {return show_cursor_; }
	void set_focus(bool focus);
	void clear_focus() { this->set_focus(false); }
	bool has_focus() const { return focus_; }
	Focus_policy focus_policy() const { return focus_policy_; }
	void set_focus_policy(Focus_policy policy) { focus_policy_ = policy; }
	bool has_mouse_tracking() const { return mouse_tracking_; }
	void set_mouse_tracking(bool track) { mouse_tracking_ = track; }
	void set_brush(Brush brush) { default_brush_ = brush; this->update(); }
	void set_brush_recursively(Brush brush);
	Brush& brush() { return default_brush_; }
	bool visible() const { return visible_; }
	void set_visible(bool visible);
	// void set_wrap_at_space(bool condition) { wrap_at_space_ = condition; } // delete this
	// bool wrap_at_space() const { return wrap_at_space_; } // delete this
	bool has_border() const { return border_.is_enabled(); }
	void enable_border();
	void disable_border();
	void set_geometry(const Geometry& g);
	Geometry& geometry() { return geometry_; }
	const Geometry& geometry() const { return geometry_; }

	Paint_engine& paint_engine() const;

	void erase_widget_screen();
	
	// Signals
	// signal<void()> Layout_param_changed; // remove

	// Slots
	slot<void()> close;
	slot<void()> hide;
	slot<void()> show;
	slot<void()> repaint;
	slot<void()> give_focus;
	slot<void()> update_me;


protected:
	virtual bool event(Event& event) override;
	virtual void move_event(Move_event& event);
	virtual void resize_event(Resize_event& event);
	virtual void paint_event(Paint_event& event);
	virtual void mouse_press_event(Mouse_event& event);
	virtual void mouse_release_event(Mouse_event& event);
	virtual void mouse_double_click_event(Mouse_event& event);
	virtual void wheel_event(Mouse_event& event);
	virtual void mouse_move_event(Mouse_event& event);
	virtual void key_press_event(Key_event& event);
	virtual void key_release_event(Key_event& event);
	virtual void close_event(Close_event& event);
	virtual void hide_event(Hide_event& event);
	virtual void show_event(Show_event& event);
	virtual void enable_event(Enable_event& event) override;
	virtual void focus_event(Focus_event& event);

	unsigned find_global_x() const;
	unsigned find_global_y() const;

	// Properties for Layouts
	// unsigned width_hint_ = System::max_width(); // remove
	// unsigned height_hint_ = System::max_height(); // remove
	// unsigned max_width_ = std::numeric_limits<unsigned>::max(); // remove
	// unsigned max_height_ = std::numeric_limits<unsigned>::max(); // remove
	// unsigned min_width_ = 0; // remove
	// unsigned min_width_hint_ = 0; // remove
	// unsigned min_height_ = 0; // remove
	// unsigned min_height_hint_ = 0; // remove
	// Size_policy size_policy_; // remove

	// Physical Properties
	unsigned x_ = 0;
	unsigned y_ = 0;
	// unsigned width_ = width_hint_; // remove
	// unsigned height_ = height_hint_; // remove

	bool show_cursor_ = true;
	unsigned cursor_x_ = 0;	// create cursor object to hold x, y and bool show
	unsigned cursor_y_ = 0;
	bool focus_ = false;
	bool mouse_tracking_ = false;
	bool visible_ = true;
	// bool wrap_at_space_ = false;	// delete this, handled in derived widget

	Focus_policy focus_policy_ = Focus_policy::NoFocus;
	Brush default_brush_ = Brush(background(Color::Black), foreground(Color::White));
	Border border_ = Border{this};

	std::unique_ptr<Paint_engine> paint_engine_ = nullptr;
	Geometry geometry_ = Geometry{this};

private:
	void paint_disabled_widget();
	void initialize();
};

} // namespace mcurses

#endif // WIDGET_HPP
