#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/widgets.hpp>

using namespace ox;

struct Clicker {
    Point at = {0, 0};
    Glyph display = U'X' | fg(XColor::Default);
};

void paint(Clicker const& c, Canvas canvas)
{
    if (c.at.x < canvas.size.width && c.at.y < canvas.size.height) {
        canvas[c.at] = c.display;
    }
}

void mouse_release(Clicker& c, Mouse m)
{
    if (m.button == Mouse::Button::Left) {
        c.at = m.at;
    }
}

void mouse_move(Clicker& c, Mouse m) { c.at = m.at; }

void mouse_wheel(Clicker& c, Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        c.display = c.display | bg(XColor::BrightBlue);
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        c.display = c.display | bg(XColor::BrightRed);
    }
}

void focus_in(Clicker& c) { c.display = c.display | fg(XColor::BrightYellow); }

void focus_out(Clicker& c) { c.display = c.display | fg(XColor::Default); }

// -------------------------------------------------------------------------------------

// struct does not help you, either way you have to take a parameter, which dictates the
// order of construction. And you don't want to have the order dictated.

[[nodiscard]] auto message_source(sl::Slot<void(std::string)> on_message) -> VLayout
{
    // return VLayout{
    //     {
    //         label("Message:"),
    //         SizePolicy::fixed(5),
    //         FocusPolicy::Strong,
    //     },
    //     Divider{},
    //     {
    //         text_box({.editable = true}),
    //         SizePolicy::flex(),
    //         FocusPolicy::Strong,
    //     },
    //     Divider{},
    //     {
    //         Clicker{},
    //         SizePolicy::flex(.5),
    //     },
    //     Divider{},
    //     {
    //         border({
    //             button({
    //                 .text = "Send",
    //                 .brush = {.background = XColor::BrightBlue,
    //                           .foreground = XColor::Black,
    //                           .traits = Trait::Underline},
    //                 .on_press_brush = {{.background = XColor::Blue,
    //                                     .foreground = XColor::Black,
    //                                     .traits = Trait::Underline | Trait::Bold}},
    //                 .on_hover_brush = {{.background = XColor::BrightBlue,
    //                                     .foreground = XColor::Black,
    //                                     .traits = Trait::Underline | Trait::Bold}},
    //                 .on_release =
    //                     [on_message = on_message, &tb = tb] {
    //                         on_message(tb.text);
    //                         tb.text.clear();
    //                     },
    //             }),
    //         }),
    //         SizePolicy::fixed(4),
    //     },
    // }

    auto head = vlayout();

    // you can't do the structs because you'd have to name them each time.
    // auto [title, tb, btn] = head.append(
    //     {
    //         label("Message:"),
    //         SizePolicy::fixed(5),
    //         FocusPolicy::Strong,
    //     },
    //     divider(),
    //     {
    //         text_box(),
    //         SizePolicy::fixed(5),
    //     },
    //     divider(),
    //     {
    //         button("Send"),
    //     });

    // btn.on_press = [on_message = on_message, &tb = tb] {
    //     on_message(tb.text);
    //     tb.text.clear();
    // };

    // auto& tb = head.append(textbox() | SizePolicy::flex() | FocusPolicy::Strong);
    // auto& tb = head.append(textbox(), {SizePolicy::flex(), FocusPolicy::Strong});

    auto& tb = head.append(text_box({.editable = true}), SizePolicy::flex(),
                           FocusPolicy::Strong);

    append_divider(head);

    head.append(Clicker{}, SizePolicy::flex(.5));

    append_divider(head);

    // TODO a better border interface is border(Box{}), the .child member gets in the
    // way
    head.append(border({
                    .child = button({
                        .text = "Send",
                        .brush = {.background = XColor::BrightBlue,
                                  .foreground = XColor::Black,
                                  .traits = Trait::Underline},
                        .on_press_brush = {{.background = XColor::Blue,
                                            .foreground = XColor::Black,
                                            .traits = Trait::Underline | Trait::Bold}},
                        .on_hover_brush = {{.background = XColor::BrightBlue,
                                            .foreground = XColor::Black,
                                            .traits = Trait::Underline | Trait::Bold}},
                        .on_release =
                            [on_message = on_message, &tb = tb] {
                                on_message(tb.text);
                                tb.text.clear();
                            },
                    }),
                }),
                SizePolicy::fixed(4));

    return head;
}

[[nodiscard]] auto message_app() -> HLayout
{
    auto head = hlayout();

    append_divider(head, {U'┃'});

    auto& rhs = head.append(text_box({
        .text = "Right Hand Side",
        .editable = false,
    }));

    insert_at(head, 0, message_source([&rhs = rhs](std::string msg) {
                  rhs.text = std::move(msg);
              }),
              SizePolicy::flex(2));

    return head;
}

// -------------------------------------------------------------------------------------

[[nodiscard]] auto message_app2() -> HLayout
{
    auto head = hlayout();

    auto& lay1 = head.append(vlayout());
    append_divider(head);
    auto& lay2 = head.append(vlayout());
    append_divider(head);
    auto& lay3 = head.append(vlayout());
    append_divider(head);
    auto& lay4 = head.append(vlayout());

    lay1.append(Clicker{});
    append_divider(lay1);
    lay1.append(Clicker{});
    append_divider(lay1);
    lay1.append(Clicker{});

    lay2.append(Clicker{});
    append_divider(lay2);
    lay2.append(Clicker{});

    lay3.append(Clicker{});
    append_divider(lay3);
    lay3.append(Clicker{});

    lay4.append(Clicker{});
    append_divider(lay4);
    lay4.append(border({Clicker{}}));
    append_divider(lay4);
    lay4.append(Clicker{});
    append_divider(lay4);
    lay4.append(Clicker{});

    return head;
}

int main()
{
    try {
        return Application{
            message_app2(),
            {MouseMode::Move},
        }
            .run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        return 1;
    }
}