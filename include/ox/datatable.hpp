#pragma once
#include <string>
#include <vector>

#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/layout.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 * A table of text data.
 * @details This is a simple table of text data. It is not intended to be a full
 * spreadsheet application. It is intended to be a simple way to display data in a grid.
 * Start by adding columns first, then rows of text data.
 */
class DataTable : public Widget {
   public:
    struct Options {
        Color background = TermColor::Default;
        Color foreground_cell = TermColor::Default;
        Color foreground_line = TermColor::Default;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    Color background;
    Color foreground_cell;
    Color foreground_line;

    std::size_t offset = 0;  // scrolling

    DataTable(Options x = init);

   public:
    /**
     * Add a column to the table.
     * @param heading The text to display at the top of the column.
     * @param align The alignment of the text in the column.
     * @param foreground The foreground color of the column heading.
     */
    void add_column(std::string heading,
                    Align align = Align::Left,
                    Color foreground = TermColor::Default);

    /**
     * Add a row to the table.
     * @param row The row to add. Must have the same number of columns as the table.
     * @throws std::out_of_range if the row has the wrong number of columns.
     */
    void add_row(std::vector<std::string> row);

   public:
    void paint(Canvas) override;

    void mouse_wheel(Mouse) override;

    void resize(Area) override;

    auto get_children() -> zzz::Generator<Widget&> override;

    auto get_children() const -> zzz::Generator<Widget const&> override;

   private:
    Row<std::vector<Label>> headings_;
    std::vector<std::vector<std::string>> columns_;
};

}  // namespace ox