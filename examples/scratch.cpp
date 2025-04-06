#include <fstream>

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

int main()
{
    auto head =
        Row{
            TextBox{{
                .wrap = TextBox::Wrap::Word,
                .align = Align::Left,
                .brush = {.background = XColor::BrightBlack,
                          .foreground = XColor::White,
                          .traits = Trait::Bold},
                .focus_policy = FocusPolicy::Strong,
            }},
            ScrollBar{{}},
        } |
        Border::round("TextBox");

    link(std::get<0>(head.child.children), std::get<1>(head.child.children));

    return Application{head}.run();
}

// int main()
// {
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