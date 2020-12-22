#ifndef TERMOX_HPP
#define TERMOX_HPP

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_matrix.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>
#include <termox/painter/utility/wchar_to_bytes.hpp>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>

#include <termox/system/animation_engine.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>

#include <termox/terminal/input.hpp>
#include <termox/terminal/output.hpp>
#include <termox/terminal/terminal.hpp>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>

#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/color_select.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>
#include <termox/widget/widgets/graph.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/log.hpp>
#include <termox/widget/widgets/matrix_display.hpp>
#include <termox/widget/widgets/menu.hpp>
#include <termox/widget/widgets/menu_stack.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/open_file.hpp>
#include <termox/widget/widgets/save_file.hpp>
#include <termox/widget/widgets/status_bar.hpp>
#include <termox/widget/widgets/text_display.hpp>
#include <termox/widget/widgets/textbox.hpp>
#include <termox/widget/widgets/titlebar.hpp>
#include <termox/widget/widgets/vertical_slider.hpp>

#include <termox/widget/area.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layout.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widget_slots.hpp>

#endif  // TERMOX_HPP
