#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/widgets.hpp>

// struct Label2 {
//     std::string text;
//     bool in_focus = false;

//     enum class Align { Left, Center, Right } align = Align::Center;
// };

// void paint(Label2 const& l, ox::Canvas c)
// {
//     // TODO these are not quite correct, should use a substr and x can go
//     // negative so handle that.
//     // TODO should vertical align be a thing? or should it always be
//     centered?
//     // probably always centered for a Label2.
//     switch (l.align) {
//         case Label2::Align::Left: ox::Painter{c}[{1, 1}] << l.text; break;
//         case Label2::Align::Center:
//             ox::Painter{c}[{
//                 .x = std::max((int)((c.size.width - l.text.size()) / 2), 0),
//                 .y = 1,
//             }] << std::string_view{l.text}.substr(0, c.size.width);
//             break;
//         case Label2::Align::Right:
//             ox::Painter{c}[{
//                 .x = (int)(c.size.width - l.text.size()),
//                 .y = 1,
//             }] << l.text;
//             break;
//     }

//     ox::Painter{c}[{0, 0}] << ox::Painter::RoundedBox{
//         .size  = c.size,
//         .brush = {.foreground = l.in_focus ? ox::XColor::BrightYellow
//                                            : ox::XColor::Default},
//     };
// }

// void focus_in(Label2& l) { l.in_focus = true; }
// void focus_out(Label2& l) { l.in_focus = false; }

// struct Button {
//     Label2 label;
//     sl::Signal<void()> clicked;
// };

// void paint(Button const& b, ox::Canvas c) { paint(b.label, c); }

// void mouse_press(Button& b, ox::Mouse m)
// {
//     if (m.button == ox::Mouse::Button::Left) {
//         b.clicked();
//     }
// }

struct Clicker {
    ox::Point at;
    ox::Glyph display = U'X' | fg(ox::XColor::Default);
};

void paint(Clicker const& c, ox::Canvas canvas) { canvas[c.at] = c.display; }

void mouse_press(Clicker& c, ox::Mouse m)
{
    if (m.button == ox::Mouse::Button::Left) {
        c.at = m.at;
    }
}

void focus_in(Clicker& c)
{
    c.display = c.display | fg(ox::XColor::BrightYellow);
}

void focus_out(Clicker& c) { c.display = c.display | fg(ox::XColor::Default); }

// struct Counter {
//     int count = 0;
//     // Timers::Handle timer = Timers::enroll(*this, std::chrono::seconds{1});
// };

// struct Textbox {
//     std::string text;
//     std::size_t cursor = 0;
//     bool in_focus      = false;
// };

// void focus_in(Textbox& t) { t.in_focus = true; }
// void focus_out(Textbox& t) { t.in_focus = false; }

// void paint(Textbox const& t, ox::Canvas c)
// {
//     for (auto i = 0; i < c.size.height; ++i) {
//         auto const start = i * c.size.width;
//         auto const end   = start + c.size.width;
//         if (start >= (int)t.text.size()) {
//             break;
//         }
//         auto const line = std::string_view{t.text}.substr(start, end -
//         start); ox::Painter{c}[{0, i}]
//             << (line | fg(t.in_focus ? ox::XColor::BrightYellow
//                                      : ox::XColor::Default));
//     }
//     // TODO need a way to set cursor if in focus
//     if (c.size.width != 0) {
//         ox::Terminal::cursor = ox::Point{c.at.x + (int)t.cursor %
//         c.size.width,
//                                          c.at.y + (int)t.cursor /
//                                          c.size.width};
//     }
// }

// void key_press(Textbox& t, ox::Key k)
// {
//     switch (k) {
//         case ox::Key::Backspace:
//             if (!t.text.empty() && t.cursor <= t.text.size()) {
//                 --t.cursor;
//                 t.text.erase(t.cursor, 1);
//             }
//             break;
//         case ox::Key::Delete:
//             if (t.cursor < t.text.size()) {
//                 t.text.erase(t.cursor, 1);
//             }
//             break;
//         case ox::Key::ArrowLeft:
//             if (t.cursor > 0) {
//                 --t.cursor;
//             }
//             break;
//         case ox::Key::ArrowRight:
//             if (t.cursor < t.text.size()) {
//                 ++t.cursor;
//             }
//             break;
//         case ox::Key::Home: t.cursor = 0; break;
//         case ox::Key::End: t.cursor = t.text.size(); break;
//         default:
//             if (auto const c = key_to_char(k); c != '\0') {
//                 t.text.insert(t.cursor, 1, c);
//                 ++t.cursor;
//             }
//             break;
//     }
// }

// struct MyThing : ox::widgets::HLayout {
//     Button& button = append(*this, Button{.label = {.text = "I'm a
//     Button"}}); Label2& label  = insert(*this, 0, Label2{.text = "I'm a
//     Label"});

//     MyThing()
//     {
//         append(*this, Label2{.text = "I'm another Label"});

//         {
//             auto v = ox::widgets::VLayout{};
//             append(v, Label2{.text = "I'm a VLayout Label"},
//                    {.focus_policy = ox::widgets::FocusPolicy::Strong,
//                     .size_policy  = {.min = 0, .flex = 0}});
//             append(v, Label2{.text = "I'm another VLayout Label"},
//                    {.focus_policy = ox::widgets::FocusPolicy::Strong,
//                     .size_policy  = {.min = 0, .flex = 0}});
//             append(v, Label2{.text = "I'm another VLayout Label too"},
//                    {.focus_policy = ox::widgets::FocusPolicy::Strong,
//                     .size_policy  = {.min = 0, .flex = 0}});
//             append(*this, std::move(v));
//         }
//         append(*this, Clicker{});
//         append(*this, Textbox{},
//                {.focus_policy = ox::widgets::FocusPolicy::Strong});

//         button.clicked.connect([&] { label.text = "Button Clicked"; });
//     }
// };

int try_main()
{
    using namespace ox::widgets;
    using namespace ox;

    return Application{
        HLayout{
            Border{
                Label{
                    .text  = "Left Aligned",
                    .align = Label::Align::Left,
                    .brush = {.background = XColor::Blue},
                },
                {
                    .brush   = {.background = XColor::Blue},
                    .corners = Painter::Box::round_corners,
                },
            },
            Widget{
                TextBox{
                    .text =
                        "This is some really long test content that will be "
                        "used to display within the\ntext box widget.\n\n<< "
                        "Two newlines right there.\nand another line.",
                },
                {.focus_policy = FocusPolicy::Strong},
            },
            Border{
                Button{
                    .label   = {.text = "A Button"},
                    .clicked = {[] {
                        std::cerr << "Button Clicked" << std::endl;
                    }},
                },
                {.corners = Painter::Box::round_corners},
            },
            Widget{Border{Clicker{}}, {.focus_policy = FocusPolicy::Strong}},
            Label{
                .text  = "Right Aligned",
                .align = Label::Align::Right,
                .brush = {.background = XColor::White,
                          .foreground = XColor::Black},
            },
        },
    }
        .run();
}

int main()
{
    try {
        try_main();
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