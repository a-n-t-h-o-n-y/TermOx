#include <ox/ox.hpp>

#include <array>
#include <string>

using namespace ox;

int main()
{
    auto root = Column{
        Row{DataTable{
                {.columns = {{.heading = "Name", .size = SizePolicy::min(18)},
                             {.heading = "Count",
                              .size = SizePolicy::fixed(10),
                              .align = Align::Right},
                             {.heading = "Description", .size = SizePolicy::min(24)}}}},
            ScrollBar{}},
        Label{
            {.text = "Arrows/Page/Home/End: select | Enter: open | Click header: sort",
             .height_policy = SizePolicy::fixed(1)}}};
    auto& body = get_child<0>(root);
    auto& table = get_child<0>(body);
    auto& bar = get_child<1>(body);
    auto& status = get_child<1>(root);
    auto rows = std::vector<std::vector<std::string>>{};
    for (int i = 0; i < 100; ++i) {
        rows.push_back(
            {"Item " + std::to_string(i), std::to_string(100 - i),
             "Unicode: 界 é 👩‍💻 — long text is clipped safely"});
    }
    table.set_rows(std::move(rows));
    link(table, bar);
    auto app = Application{root};
    auto ascending = std::array{false, false, false};
    table.on_header.connect([&](std::size_t column) {
        ascending[column] = !ascending[column];
        table.sort_rows(
            column, ascending[column],
            column == 1
                ? std::function<bool(std::string_view, std::string_view)>{[](auto a,
                                                                             auto b) {
                      return std::stoi(std::string{a}) < std::stoi(std::string{b});
                  }}
                : std::function<bool(std::string_view, std::string_view)>{});
    });
    table.on_activate.connect([&](std::size_t row) {
        status.text = "Opened " + table.rows()[row][0];
        app.request_repaint();
    });
    return app.run();
}
