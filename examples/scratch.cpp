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

template <typename T>
auto board_holder(T board)
{
    class Holder : public Widget {
       public:
        T board;

       public:
        Holder(T board_)
            : Widget{FocusPolicy::None, SizePolicy::flex()}, board{std::move(board_)}
        {}

       public:
        auto get_children() -> zzz::Generator<Widget&> override { co_yield board; }
        auto get_children() const -> zzz::Generator<Widget const&> { co_yield board; }

        void resize(Area) override
        {
            auto const row_length = (int)board.children.size();
            auto const x = this->size.width / 2 - row_length;
            auto const y = this->size.height / 2 - row_length / 2;
            board.at = {x, y};
            board.size = {
                .width = row_length * 2,
                .height = row_length,
            };
            board.resize({
                .width = row_length * 2,
                .height = row_length,
            });
        }
    };

    return Holder{std::move(board)};
}

// auto counter_widget()
// {
//     return Row{
//         std::tuple{
//             Button{{.text = "Increment"}},
//             Label{{.text = "0"}},
//         },
//         [](auto& btn, auto& label) {
//             btn.on_press.connect(tracked(
//                 [count=0](Button& btn, Label& label) { label.text =
//                 std::to_string(++count); }));
//             )
//                 [count = 0]() mutable { label.text = std::to_string(++count); });
//         },
//     };
// }

// int main()
// {
//     // auto head = TextBox{} | FocusPolicy::Strong | Border::round("TextBox");
//     // auto head = Grid<std::vector<Row<std::vector<Label>>>>{};
//     // head.children.push_back(Row{std::vector{
//     //     Label{{.text = "one"}},
//     //     Label{{.text = "two"}},
//     //     Label{{.text = "three"}},
//     // }});

//     // auto head2 = Grid{
//     //     Row{std::tuple{
//     //         Label{{.text = "one"}},
//     //         Label{{.text = "two"}},
//     //         Label{{.text = "three"}},
//     //     }},
//     //     Row{std::tuple{
//     //         Label{{.text = "four"}},
//     //         Label{{.text = "five"}},
//     //         Label{{.text = "six"}},
//     //     }},
//     //     Row{std::tuple{
//     //         Label{{.text = "seven"}},
//     //         Label{{.text = "eight"}},
//     //         Label{{.text = "nine"}},
//     //     }},
//     // };

//     // auto head = Column{std::tuple{
//     //                 Row{std::tuple{
//     //                     Label{{.text = "one"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "two"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "three"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "three.five"}},
//     //                 }},
//     //                 Divider::light(),
//     //                 Row{std::tuple{
//     //                     Label{{.text = "four"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "five"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "six"}},
//     //                 }},
//     //                 Divider::light(),
//     //                 Row{std::tuple{
//     //                     Label{{.text = "seven"}},
//     //                     Divider::light(),
//     //                     Label{{.text = "nine"}},
//     //                 }},
//     //             }} |
//     //             Border::round();

//     // auto head = board_holder(chessboard());

//     // auto head = DataTable{};
//     // head.add_column("one", Align::Center, XColor::Blue);
//     // head.add_row({"1"});
//     // head.add_column("two", Align::Center);
//     // head.add_column("three", Align::Center);
//     // head.add_row({"2", "3", "4"});
//     // head.add_column("four", Align::Center);
//     // head.add_row({"5", "6", "7", "8"});
//     // head.add_row({"9", "10", "11", "12"});
//     // head.add_row({"13", "14", "15", "16"});
//     // head.add_row({"17", "18", "19", "20"});
//     // head.add_row({"21", "22", "23", "24"});
//     // head.add_row({"25", "26", "27", "28"});
//     // head.add_row({"29", "30", "31", "32"});
//     // head.add_row({"33", "34", "35", "36"});
//     // head.add_row({"37", "38", "39", "40"});

//     auto head = Column{std::vector{
//                     counter_widget(),
//                     counter_widget(),
//                     counter_widget(),
//                     counter_widget(),
//                 }} |
//                 Border::round();

//     return Application{head}.run();
// }

// int main()
// {
// auto btn = Button{{
//     .text = "Increment",
//     .brush = {.background = XColor::BrightBlue, .foreground = XColor::Black},
// }};
// auto label = Label{{.text = "0"}};

// btn.on_press.connect(tracked(  // tracks label even after std::move
//     [count = 0](Label& label) mutable { label.text = std::to_string(++count); },
//     label));

// auto head = Row{std::tuple{
//                 std::move(btn),
//                 std::move(label),
//             }} |
//             Border::round("Counter");

// -----------------------------

// auto head = Row{std::array<Button, 5>{}};
// for (auto& btn : head.children) {
//     btn.label.text = "hello";
// }

// -----------------------------

// auto head = Column{
//     std::tuple{
//         TextBox{} | Border::round("Messages"),
//         Row{
//             std::tuple{
//                 Button{{
//                     .text = "Add Message",
//                     .brush = {.background = RGB{0x1ECBE1}},
//                     .on_press_brush = Brush{.background = RGB{0x18A0B1}},
//                 }},
//                 LineEdit{{.ghost_text = "Enter Message"}},
//             },
//         } | SizePolicy::fixed(1),
//     },
// };

// struct Buttons : Row<std::array<Button, 5>> {
//     sl::Signal<void(int)> on_increment;

//     Buttons()
//     {
//         auto x = 1;
//         for (auto& btn : children) {
//             btn.label.text = std::to_string(x);
//             btn.on_press.connect(
//                 tracked([x](auto& self) { self.on_increment(x); }, *this));
//             x += x;
//         }
//     }
// };

// auto head = Row{std::tuple{
//                     Buttons{},
//                     Column{std::array<Label, 2>{
//                         Label{{"Total"}} | SizePolicy::fixed(1),
//                         Label{{"0"}},
//                     }} | SizePolicy::fixed(20),
//                 },
//                 [](auto& btns, auto& totals) {
//                     btns.on_increment.connect(tracked(
//                         [](Label& l, int amount) {
//                             l.text = std::to_string(std::stoi(l.text) + amount);
//                         },
//                         totals.children[1]));
//                 }} |
//             Border::round("Accumulator");

// color alignment border traits for readme
// interactivity for documentation

// auto head = Label{{
//                 .text = "C++20 Terminal UI Framework",
//                 .align = Align::Center,
//                 .brush = {.foreground = XColor::Blue, .traits = Trait::Bold},
//             }} |
//             Border::round("TermOx II");

// auto head = Column{std::tuple{
//     Row{std::tuple{
//         CheckBox{},
//         Divider::ascii(),
//     }},
// }};

// Grid that emits the value selected
// Label 'Total'
// Label of the actual value accumulated

// return Application{head}.run();
// }

// int main()
// {
// auto head =
//     Row{
//         std::tuple{
//             TextBox{{
//                 .wrap = TextBox::Wrap::Word,
//                 .align = Align::Left,
//                 .brush = {.background = XColor::BrightBlack,
//                           .foreground = XColor::White,
//                           .traits = Trait::Bold},
//                 .focus_policy = FocusPolicy::Strong,
//             }},
//             ScrollBar{{}},
//         },
//         [](auto& tb, auto& sb) { link(tb, sb); },
//     } |
//     Border::round("TextBox");
//     auto head = Column{
//         std::tuple{
//             Row{std::tuple{CheckBox{}, Label{{.text = "One"}}}},
//             Row{std::tuple{CheckBox{}, Label{{.text = "Two"}}}},
//         },
//     };
//     return Application{head}.run();
// }

int main()
{
    auto head = DataTable{};
    head.add_column("one", Align::Center, XColor::Blue);
    head.add_row({"1"});
    head.add_column("two", Align::Center);
    head.add_column("three", Align::Center);
    head.add_row({"2", "3", "4"});
    head.add_column("four", Align::Center);
    head.add_row({"5", "6", "7", "8"});
    head.add_row({"9", "10", "11", "12"});
    head.add_row({"13", "14", "15", "16"});
    head.add_row({"17", "18", "19", "20"});
    head.add_row({"21", "22", "23", "24"});
    head.add_row({"25", "26", "27", "28"});
    head.add_row({"29", "30", "31", "32"});
    head.add_row({"33", "34", "35", "36"});
    head.add_row({"37", "38", "39", "40"});

    return Application{head}.run();
}