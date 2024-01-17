#include "get_rle.hpp"

#include <cctype>
#include <fstream>
#include <istream>
#include <string>
#include <utility>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace {
using namespace gol;

auto extract_int(std::string& run_length) -> int
{
    auto count = 1;
    if (!run_length.empty()) {
        count = std::stoi(run_length);
        run_length.clear();
    }
    return count;
}

void next_row(Coordinate& insert_position, std::string& run_length, int begin_x)
{
    auto count        = extract_int(run_length);
    insert_position.x = begin_x;
    for (; count != 0; --count)
        insert_position.y += 1;
}

void seek_past_comments(std::istream& is)
{
    while (is.peek() == '#') {
        auto tmp = std::string{};
        std::getline(is, tmp);
    }
}

/// Return the top left coordinate of the pattern.
auto get_offset(std::istream& is) -> Coordinate
{
    is.seekg(4, std::ios_base::cur);  // "x = "
    auto width = 0;
    is >> width;
    is.seekg(6, std::ios_base::cur);  // ", y = "
    auto height = 0;
    is >> height;
    return {-1 * (width / 2), -1 * (height / 2)};
}

/// Extract the rule to use from the file.
auto get_rule_string(std::istream& is) -> std::string
{
    if (is.peek() != ',')
        return "B3/S23";

    auto result = std::string{};
    is.seekg(9, std::ios_base::cur);
    while (is && is.peek() != '\n') {
        result += is.peek();
        is.seekg(1, std::ios_base::cur);
    }
    return result;
}

template <typename Container_t>
void insert_alive(Container_t& pattern, Coordinate& insert_position, int count)
{
    for (; count != 0; --count) {
        pattern.push_back(insert_position);
        ++insert_position.x;
    }
}

void insert_dead(Coordinate& insert_position, int count)
{
    insert_position.x += count;
}

}  // namespace

namespace gol {

auto get_RLE(std::string const& filename) -> Pattern
{
    auto fs = std::ifstream{filename};
    return parse_rle(fs);
}

auto parse_rle(std::istream& is) -> Pattern
{
    // Call Order Matters
    seek_past_comments(is);
    auto const offset = get_offset(is);
    auto const rule   = parse_rule_string(get_rule_string(is));

    auto run_length      = std::string{};
    auto insert_position = offset;
    auto cells           = Pattern::Cells{};

    while (is) {
        auto c = '\0';
        is >> c;
        if (c == 'o')  // alive cell
            insert_alive(cells, insert_position, extract_int(run_length));
        else if (c == 'b')  // dead cell
            insert_dead(insert_position, extract_int(run_length));
        else if (std::isdigit(c))
            run_length.push_back(c);
        else if (std::isspace(c))
            continue;
        else if (c == '$')
            next_row(insert_position, run_length, offset.x);
        else if (c == '!')
            break;
    }
    return {cells, rule};
}

}  // namespace gol
