#include <fstream>
#include <ox/ox.hpp>
using namespace ox;

[[nodiscard]] auto chessboard()
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
        auto get_children() -> Generator<Widget&> override { co_yield board; }
        auto get_children() const -> Generator<Widget const&> { co_yield board; }

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

int main()
{
    // auto head = TextBox{} | FocusPolicy::Strong | Border::round("TextBox");
    // auto head = Grid<std::vector<Row<std::vector<Label>>>>{};
    // head.children.push_back(Row{std::vector{
    //     Label{{.text = "one"}},
    //     Label{{.text = "two"}},
    //     Label{{.text = "three"}},
    // }});

    // auto head2 = Grid{
    //     Row{std::tuple{
    //         Label{{.text = "one"}},
    //         Label{{.text = "two"}},
    //         Label{{.text = "three"}},
    //     }},
    //     Row{std::tuple{
    //         Label{{.text = "four"}},
    //         Label{{.text = "five"}},
    //         Label{{.text = "six"}},
    //     }},
    //     Row{std::tuple{
    //         Label{{.text = "seven"}},
    //         Label{{.text = "eight"}},
    //         Label{{.text = "nine"}},
    //     }},
    // };

    // auto head = Column{std::tuple{
    //                 Row{std::tuple{
    //                     Label{{.text = "one"}},
    //                     Divider::light(),
    //                     Label{{.text = "two"}},
    //                     Divider::light(),
    //                     Label{{.text = "three"}},
    //                     Divider::light(),
    //                     Label{{.text = "three.five"}},
    //                 }},
    //                 Divider::light(),
    //                 Row{std::tuple{
    //                     Label{{.text = "four"}},
    //                     Divider::light(),
    //                     Label{{.text = "five"}},
    //                     Divider::light(),
    //                     Label{{.text = "six"}},
    //                 }},
    //                 Divider::light(),
    //                 Row{std::tuple{
    //                     Label{{.text = "seven"}},
    //                     Divider::light(),
    //                     Label{{.text = "nine"}},
    //                 }},
    //             }} |
    //             Border::round();

    // auto head = board_holder(chessboard());

    auto head = DataTable{};
    head.add_column("one", Label::Align::Center, XColor::Blue);
    head.add_row({"1"});
    head.add_column("two", Label::Align::Center);
    head.add_column("three", Label::Align::Center);
    head.add_row({"2", "3", "4"});
    head.add_column("four", Label::Align::Center);
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