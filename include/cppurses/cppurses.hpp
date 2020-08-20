#ifndef CPPURSES_HPP
#define CPPURSES_HPP

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/painter/utility/wchar_to_bytes.hpp>

#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/focus_event.hpp>
#include <cppurses/system/events/input_event.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/events/timer_event.hpp>

#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>

#include <cppurses/terminal/input.hpp>
#include <cppurses/terminal/output.hpp>
#include <cppurses/terminal/terminal.hpp>

#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>

#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/confirm_button.hpp>
#include <cppurses/widget/widgets/cycle_box.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/graph.hpp>
#include <cppurses/widget/widgets/horizontal_scrollbar.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/line_edit.hpp>
#include <cppurses/widget/widgets/log.hpp>
#include <cppurses/widget/widgets/matrix_display.hpp>
#include <cppurses/widget/widgets/menu.hpp>
#include <cppurses/widget/widgets/menu_stack.hpp>
#include <cppurses/widget/widgets/number_edit.hpp>
#include <cppurses/widget/widgets/open_file.hpp>
#include <cppurses/widget/widgets/save_file.hpp>
#include <cppurses/widget/widgets/status_bar.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widgets/textbox.hpp>
#include <cppurses/widget/widgets/titlebar.hpp>
#include <cppurses/widget/widgets/vertical_scrollbar.hpp>
#include <cppurses/widget/widgets/vertical_slider.hpp>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layout.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widget_slots.hpp>

#endif  // CPPURSES_HPP
