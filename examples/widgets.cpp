#include <iostream>
#include <tuple>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/ox.hpp>

using namespace ox;

struct Clicker : Widget {
    sl::Signal<void(char)> on_click;

    Point clicked_at = {0, 0};
    Glyph display = U'X' | fg(XColor::Default);

    Clicker() : Widget{FocusPolicy::Strong, SizePolicy::flex()} {}

    void paint(Canvas canvas) override
    {
        if (clicked_at.x < canvas.size.width && clicked_at.y < canvas.size.height) {
            canvas[clicked_at] = display;
        }
    }

    void mouse_press(Mouse m) override
    {
        if (m.button == Mouse::Button::Left) {
            on_click('L');
        }
        else if (m.button == Mouse::Button::Right) {
            on_click('R');
        }
    }

    void mouse_release(Mouse m) override
    {
        if (m.button == Mouse::Button::Left) {
            clicked_at = m.at;
        }
    }

    void mouse_move(Mouse m) override { clicked_at = m.at; }

    void mouse_wheel(Mouse m) override
    {
        if (m.button == Mouse::Button::ScrollUp) {
            display = display | bg(XColor::BrightBlue);
        }
        else if (m.button == Mouse::Button::ScrollDown) {
            display = display | bg(XColor::BrightRed);
        }
    }

    void focus_in() override { display = display | fg(XColor::BrightYellow); }

    void focus_out() override { display = display | fg(XColor::Default); }
};

struct ALayout : Column<std::array<Clicker, 2>> {};

struct SignalTest : Column<std::tuple<Label, Button, Bordered<Clicker>, Label>> {
    SignalTest()
        : Column<std::tuple<Label, Button, Bordered<Clicker>, Label>>{std::tuple{
              Label{{
                  .text = "Label 1",
                  .align = Label::Align::Center,
              }},
              Button{{
                  "Click me!",
                  {},
                  Brush{.background = XColor::Blue, .foreground = XColor::Black},
                  Brush{.background = XColor::BrightBlue, .foreground = XColor::Black},
              }},
              Bordered<Clicker>{{.border = Border::round()}},
              Label{{
                  .text = "Label 2",
                  .align = Label::Align::Right,
              }},
          }}
    {
        auto& [label1, button, clicker, label2] = this->children;

        button.on_press.connect(tracked(
            [count = 0](Label& label1, Label& label2) mutable {
                auto text = "Clicked! " + std::to_string(count++);
                label1.text = text;
                label2.text = std::move(text);
            },
            label1, label2));
    }
};

auto signal_test()
{
    return Column{std::tuple{
        Label{{
            .text = "Hello, world! 1",
            .align = Label::Align::Center,
            .brush = {.background = XColor::BrightBlue,
                      .foreground = XColor::Black,
                      .traits = Trait::Bold},
        }},
        Divider::double_h(),
        Row{
            std::tuple{
                CheckBox{{}} | FocusPolicy::Strong,
                Label{{
                    .text = "State: UnChecked",
                    .align = Label::Align::Left,
                }},
            },
            [](auto& cb, auto& label) {
                cb.on_check.connect(
                    tracked([](Label& l) { l.text = "State: Checked"; }, label));
                cb.on_uncheck.connect(
                    tracked([](Label& l) { l.text = "State: UnChecked"; }, label));
            },
        } | SizePolicy::fixed(1),
        Divider::light_h(),
        LineEdit{{.ghost_text = "Ghost Text"}},
        Divider::light_h(),
        Label{{
            .text = "Hello, world! 2",
            .align = Label::Align::Right,
            .brush = {.background = XColor::BrightRed, .foreground = XColor::Black},
        }},
        Row{std::vector<Label>{},
            [](auto& v) {
                v.push_back(Label{{.text = "One"}});
                v.push_back(Label{{.text = "Two",
                                   .brush = {.background = XColor::BrightGreen,
                                             .foreground = XColor::Black}}});
                v.push_back(Label{{.text = "Three"}});
            }} |
            SizePolicy::fixed(1),
        Row{std::tuple{
                TextBox{{
                    .text = U"Hello, world!\nThis is a new line!\n\nThis is too!"_gs |
                            fg(XColor::Black),
                    .wrap = TextBox::Wrap::Word,
                    .align = TextBox::Align::Center,
                    .background = XColor::BrightGreen,
                }} | FocusPolicy::Strong,
                ScrollBar{{.brush = {.background = XColor::Blue,
                                     .foreground = XColor::Green}}},
            },
            [](auto& tb, auto& sb) { link(tb, sb); }} |
            Border::round({
                .brush = {.background = XColor::BrightGreen,
                          .foreground = XColor::Black},
                .label = {{
                    .text = "Hello",
                    .align = Label::Align::Center,
                    .brush = {.background = XColor::BrightGreen,
                              .foreground = XColor::Black,
                              .traits = Trait::Bold},
                }},
            }),
        Row{std::tuple{
            Label{{
                .text = "Hello, world! 3",
                .align = Label::Align::Right,
                .brush = {.background = XColor::BrightRed, .foreground = XColor::Black},
            }} | Border::round(),
            ScrollBar{{.scrollable_length = 100, .position = 0}},
        }},
    }};
}

// auto scrollbar_test()
// {
//     auto head = HVector<>{};

//     auto& tb = head.append(TextBox{});
//     tb.focus_policy = FocusPolicy::Strong;

//     head.append(Divider::light_v()).line.brush.foreground = XColor::BrightBlack;

//     auto& sb = head.append(ScrollBar{{.scrollable_length = 100}});

//     link(tb, sb);

//     return head;
// }

int main()
{
    // auto head = SignalTest{};
    auto head = signal_test();
    // auto head = scrollbar_test();
    auto head2 = std::move(head);
    auto head3 = std::move(head2);
    return Application{head3, {MouseMode::Basic}}.run();

    // auto clicker = ALayout{};
    // auto clicker = Clicker{};
    // connect(clicker.on_click,
    // tracked([](char c) { std::cout << "Clicked: " << c << std::endl; }));
    // return Application{clicker, {MouseMode::Move}}.run();
}