#include <filesystem>
#include <fstream>
#include <iostream>

#include <ox/ox.hpp>
using namespace ox;

[[nodiscard]]
auto chessboard()
{
    auto c = Column<std::array<Row<std::array<Label, 8>>, 8>>{};
    auto i = 0;
    for (auto& row : c.children) {
        for (auto& label : row.children) {
            label.brush.background = i % 2 ? XColor::White : XColor::Green;
            ++i;
        }
        ++i;
    }
    return c;
}

// int main()
// {
//     auto head = chessboard();

//     return Application{head}.run();
// }

// int main()
// {
//     auto head =
//         Row{
//             TextBox{{
//                 .wrap = TextBox::Wrap::Word,
//                 .align = Align::Left,
//                 .brush = {.background = XColor::BrightBlack,
//                           .foreground = XColor::White,
//                           .traits = Trait::Bold},
//                 .focus_policy = FocusPolicy::Strong,
//             }},
//             ScrollBar{{}},
//         } |
//         Border::round("TextBox");

//     link(std::get<0>(head.child.children), std::get<1>(head.child.children));

//     return Application{head}.run();
// }

// int main()
// {
//     Terminal::background = RGB{0x0a3f46};
//     Terminal::foreground = RGB{0xB4724B};

//     auto head = DataTable{};
//     head.add_column("one", Align::Center, XColor::Blue);
//     head.add_row({"1"});
//     head.add_column("two", Align::Center);
//     head.add_column("three", Align::Center);
//     head.add_row({"2", "3", "4"});
//     head.add_column("four", Align::Center);
//     head.add_row({"5", "6", "7", "8"});
//     head.add_row({"9", "10", "11", "12"});
//     head.add_row({"13", "14", "15", "16"});
//     head.add_row({"17", "18", "19", "20"});
//     head.add_row({"21", "22", "23", "24"});
//     head.add_row({"25", "26", "27", "28"});
//     head.add_row({"29", "30", "31", "32"});
//     head.add_row({"33", "34", "35", "36"});
//     head.add_row({"37", "38", "39", "40"});

//     return Application{head}.run();
// }

auto hover_btn(std::string const& label) -> Button
{
    auto fade = Fade{
        .paint_fn =
            [](Canvas c, float percent) {
                auto const blend =
                    gradient_blend(RGB{0x0a3f46}, RGB{0x1ecbe1}, percent);
                fill(c, bg(blend));
            },
        .fade_in = std::chrono::milliseconds{1},
        .fade_out = std::chrono::milliseconds{300},
    };

    return Button{{
        .label = {.text = label},
        .decoration = fade,
    }};
}

int main()
{
    Terminal::background = RGB{0x0a3f46};
    Terminal::foreground = RGB{0xB4724B};

    auto head = Row{
        Column<std::vector<Button>>{} | SizePolicy::fixed(18),
        Divider::bold({
            .brush = {.foreground = XColor::BrightBlack},
        }),
        TextBox{{
            .focus_policy = FocusPolicy::Strong,
        }},
    };

    for (auto i = 0; i < 21; ++i) {
        std::get<0>(head.children)
            .children.push_back(hover_btn("Hover " + std::to_string(i)));
    }

    return Application{head, Terminal{MouseMode::Move}}.run();
}