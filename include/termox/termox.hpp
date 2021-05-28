#ifndef TERMOX_TERMOX_HPP
#define TERMOX_TERMOX_HPP

#include <signals_light/signal.hpp>

#include <termox/common/mb_to_u32.hpp>
#include <termox/common/u32_to_mb.hpp>

#include <termox/painter/palette/amstrad_cpc.hpp>
#include <termox/painter/palette/apple_ii.hpp>
#include <termox/painter/palette/ashes.hpp>
#include <termox/painter/palette/basic.hpp>
#include <termox/painter/palette/basic8.hpp>
#include <termox/painter/palette/cga.hpp>
#include <termox/painter/palette/commodore_64.hpp>
#include <termox/painter/palette/commodore_vic20.hpp>
#include <termox/painter/palette/dawn_bringer16.hpp>
#include <termox/painter/palette/dawn_bringer32.hpp>
#include <termox/painter/palette/en4.hpp>
#include <termox/painter/palette/gameboy.hpp>
#include <termox/painter/palette/gameboy_pocket.hpp>
#include <termox/painter/palette/macintosh_ii.hpp>
#include <termox/painter/palette/msx.hpp>
#include <termox/painter/palette/nes.hpp>
#include <termox/painter/palette/savanna.hpp>
#include <termox/painter/palette/secam.hpp>
#include <termox/painter/palette/stormy6.hpp>
#include <termox/painter/palette/teletext.hpp>
#include <termox/painter/palette/thomson_m05.hpp>
#include <termox/painter/palette/windows.hpp>
#include <termox/painter/palette/windows_console.hpp>
#include <termox/painter/palette/zx_spectrum.hpp>

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/dynamic_colors.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_matrix.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>

#include <termox/system/animation_engine.hpp>
#include <termox/system/event.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/system/shortcuts.hpp>
#include <termox/system/system.hpp>

#include <termox/terminal/terminal.hpp>

#include <termox/widget/layouts/fixed.hpp>
#include <termox/widget/layouts/float.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/opposite.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/selecting.hpp>
#include <termox/widget/layouts/set.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>

#include <termox/widget/widgets/accordion.hpp>
#include <termox/widget/widgets/banner.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/button_list.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/color_select.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>
#include <termox/widget/widgets/graph.hpp>
#include <termox/widget/widgets/hideable.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/log.hpp>
#include <termox/widget/widgets/matrix_view.hpp>
#include <termox/widget/widgets/menu.hpp>
#include <termox/widget/widgets/menu_stack.hpp>
#include <termox/widget/widgets/notify_light.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/number_view.hpp>
#include <termox/widget/widgets/read_file.hpp>
#include <termox/widget/widgets/scrollbar.hpp>
#include <termox/widget/widgets/selectable.hpp>
#include <termox/widget/widgets/slider.hpp>
#include <termox/widget/widgets/spinner.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/textbox.hpp>
#include <termox/widget/widgets/tile.hpp>
#include <termox/widget/widgets/titlebar.hpp>
#include <termox/widget/widgets/toggle_button.hpp>
#include <termox/widget/widgets/write_file.hpp>

#include <termox/widget/align.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/bordered.hpp>
#include <termox/widget/cursor.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/layout.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widget_slots.hpp>

#endif  // TERMOX_TERMOX_HPP
