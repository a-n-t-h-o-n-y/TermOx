#include <filesystem>
#include <fstream>
#include <iostream>

#include <ox/ox.hpp>
using namespace ox;

// [[nodiscard]]
// auto chessboard()
// {
//     auto c = Column<std::array<Row<std::array<Label, 8>>, 8>>{};
//     auto i = 0;
//     for (auto& row : c.children) {
//         for (auto& label : row.children) {
//             label.style.background = i % 2 ? colors::white : colors::green;
//             ++i;
//         }
//         ++i;
//     }
//     return c;
// }

// int main()
// {
//     auto head = chessboard();

//     return Application{head}.run();
// }

// -------------------------------------------------------------------------------------

// int main()
// {
//     auto head =
//         Row{
//             TextBox{{
//                 .wrap = TextBox::Wrap::Word,
//                 .align = Align::Left,
//                 .style = {.background = colors::bright_black,
//                           .foreground = colors::white,
//                           .traits = Trait::Bold},
//                 .focus_policy = FocusPolicy::Strong,
//             }},
//             ScrollBar{{}},
//         } |
//         Border{.box = shape::Box::round(), .label = "TextBox"};

//     link(get_child<0>(head), get_child<1>(head));

//     return Application{head}.run();
// }

// -------------------------------------------------------------------------------------

// int main()
// {
//     auto head = Row{
//         DataTable{},
//         ScrollBar{},
//     };
//     auto& [dt, sb] = head.children;

//     dt.add_column("one", Align::Center, colors::blue);
//     dt.add_row({"1"});
//     dt.add_column("two", Align::Center);
//     dt.add_column("three", Align::Center);
//     dt.add_row({"2", "3", "4"});
//     dt.add_column("four", Align::Center);
//     dt.add_row({"5", "6", "7", "8"});
//     dt.add_row({"9", "10", "11", "12"});
//     dt.add_row({"13", "14", "15", "16"});
//     dt.add_row({"17", "18", "19", "20"});
//     dt.add_row({"21", "22", "23", "24"});
//     dt.add_row({"33", "34", "35", "36"});
//     dt.add_row({"37", "38", "39", "40"});

//     for (auto i = 0; i < 1000; ++i) {
//         dt.add_row({std::to_string(i), std::to_string(i + 1), std::to_string(i + 2),
//                     std::to_string(i + 3)});
//     }

//     link(dt, sb);

//     return Application{head, Terminal{{
//                                  .foreground = RGB{0x030303},
//                                  .background = RGB{0xD4C9BE},
//                              }}}
//         .run();
// }

// -------------------------------------------------------------------------------------

// auto hover_btn(std::string const& label) -> Button
// {
//     auto fade = Fade{
//         .paint_fn =
//             [](Canvas c, float percent) {
//                 auto const blend =
//                     gradient_blend(RGB{0x0a3f46}, RGB{0x1ecbe1}, percent);
//                 fill(c, bg(blend));
//             },
//         .fade_in = std::chrono::milliseconds{1},
//         .fade_out = std::chrono::milliseconds{300},
//     };

//     return Button{{
//         .label = {.text = label},
//         .decoration = fade,
//     }};
// }

// int main()
// {
//     auto head = Row{
//         Column<std::vector<Button>>{} | SizePolicy::fixed(18),
//         Divider{{
//             .lines = Lines::bold(),
//             .style = {.foreground = colors::bright_black},
//         }},
//         TextBox{{
//             .focus_policy = FocusPolicy::Strong,
//         }},
//     };

//     for (auto i = 0; i < 21; ++i) {
//         get_child<0>(head).children.push_back(hover_btn("Hover " +
//         std::to_string(i)));
//     }

//     return Application{head, Terminal{{
//                                  .mouse_mode = MouseMode::Move,
//                                  .foreground = RGB{0x0a3f46},
//                                  .background = RGB{0xB4724B},
//                              }}}
//         .run();
// }

// int main()
// {
//     auto head = Suspended{
//         TextBox{{
//             .style = {.background = colors::bright_black},
//             .focus_policy = FocusPolicy::Strong,
//         }},
//         {.width = 40, .height = 5},
//     };
//     return Application{head}.run();
// }

// struct SWidget : Widget {
//     SWidget(FocusPolicy fp = FocusPolicy::Strong, SizePolicy sp = SizePolicy::flex())
//         : Widget{fp, sp}
//     {}

//     // Signals
//     sl::Signal<void(MousePress)> on_mouse_press;
//     sl::Signal<void(MouseRelease)> on_mouse_release;
//     sl::Signal<void(MouseWheel)> on_mouse_wheel;
//     sl::Signal<void(MouseMove)> on_mouse_move;
//     sl::Signal<void()> on_mouse_enter;
//     sl::Signal<void()> on_mouse_leave;
//     sl::Signal<void()> on_focus_in;
//     sl::Signal<void()> on_focus_out;
//     sl::Signal<void(KeyEvent)> on_key_press;
//     sl::Signal<void(KeyEvent)> on_key_release;
//     sl::Signal<void(Area)> on_resize;
//     sl::Signal<void()> on_timer;
//     sl::Signal<void(Canvas)> on_paint;

//    protected:
//     auto mouse_press(MousePress m) -> EventResponse override
//     {
//         this->on_mouse_press(m);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto mouse_release(MouseRelease m) -> EventResponse override
//     {
//         this->on_mouse_release(m);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto mouse_wheel(MouseWheel m) -> EventResponse override
//     {
//         this->on_mouse_wheel(m);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto mouse_move(MouseMove m) -> EventResponse override
//     {
//         this->on_mouse_move(m);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto mouse_enter() -> EventResponse override
//     {
//         this->on_mouse_enter();
//         return {.handled = true, .repaint = true};
//     }
//
//     auto mouse_leave() -> EventResponse override
//     {
//         this->on_mouse_leave();
//         return {.handled = true, .repaint = true};
//     }
//
//     auto focus_in() -> EventResponse override
//     {
//         this->on_focus_in();
//         return {.handled = true, .repaint = true};
//     }
//
//     auto focus_out() -> EventResponse override
//     {
//         this->on_focus_out();
//         return {.handled = true, .repaint = true};
//     }
//
//     auto key_press(KeyEvent k) -> EventResponse override
//     {
//         this->on_key_press(k);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto key_release(KeyEvent k) -> EventResponse override
//     {
//         this->on_key_release(k);
//         return {.handled = true, .repaint = true};
//     }
//
//     auto timer() -> EventResponse override
//     {
//         this->on_timer();
//         return {.handled = true, .repaint = true};
//     }
//
//     void resize(Area old_size) override { this->on_resize(old_size); }
//     void paint(Canvas c) override { this->on_paint(c); }
// };

int main()
{
    auto head = Row{
        RadioGroup{{
            .items = {"one", "two", "three"},
            .focus_policy = FocusPolicy::Strong,
        }},
        TextBox{{.focus_policy = FocusPolicy::Strong}},
    };
    auto& [rg, tb] = head.children;
    Connection{
        .signal = rg.on_select,
        .slot = [](std::string const& selection, auto& tb) { tb.set_text(selection); },
    }(tb);

    return Application{head}.run();
}