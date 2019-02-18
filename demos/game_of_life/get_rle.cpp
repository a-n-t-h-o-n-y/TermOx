#include "get_rle.hpp"

#include <cctype>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "coordinate.hpp"

namespace {
using namespace gol;

int extract_int(std::string& run_length) {
    auto count{1};
    if (!run_length.empty()) {
        count = std::stoi(run_length);
        run_length.clear();
    }
    return count;
}

void next_row(Coordinate& insert_position,
              std::string& run_length,
              int begin_x) {
    auto count = extract_int(run_length);
    insert_position.x = begin_x;
    while (count != 0) {
        insert_position.y += 1;
        --count;
    }
}

void seek_past_comments(std::ifstream& file) {
    while (file.peek() == '#') {
        std::string tmp;
        std::getline(file, tmp);
    }
}

/// Return the top left coordinate of the pattern.
Coordinate get_offset(std::ifstream& file) {
    file.seekg(4, std::ios_base::cur);  // "x = "
    int width{0};
    file >> width;
    file.seekg(6, std::ios_base::cur);  // ", y = "
    int height{0};
    file >> height;
    return {-1 * (width / 2), -1 * (height / 2)};
}

/// Extract the rule to use from the file.
std::string get_rule(std::ifstream& file) {
    if (file.peek() != ',') {
        return "3/23";
    }
    file.seekg(9, std::ios_base::cur);
    if (file.peek() == 'B' || file.peek() == 'b') {
        file.seekg(1, std::ios_base::cur);
    }
    int birth{9};
    if (std::isdigit(file.peek())) {
        file >> birth;
    }
    file.seekg(1, std::ios_base::cur);
    if (file.peek() == 'S' || file.peek() == 's') {
        file.seekg(1, std::ios_base::cur);
    }
    int survival{9};
    if (std::isdigit(file.peek())) {
        file >> survival;
    }
    std::string rule;
    rule.append(std::to_string(birth));
    rule.push_back('/');
    rule.append(std::to_string(survival));
    return rule;
}

template <typename Container_t>
void insert_alive(Container_t& pattern,
                  Coordinate& insert_position,
                  int count) {
    while (count != 0) {
        pattern.push_back(insert_position);
        ++insert_position.x;
        --count;
    }
}

void insert_dead(Coordinate& insert_position, int count) {
    insert_position.x += count;
}
}  // namespace

namespace gol {

std::pair<std::vector<Coordinate>, std::string> get_RLE(
    const std::string& filename) {
    std::ifstream file{filename};
    std::vector<Coordinate> pattern;

    seek_past_comments(file);
    const Coordinate offset = get_offset(file);
    Coordinate insert_position{offset};
    std::string run_length{""};

    const std::string rule{get_rule(file)};

    while (file) {
        char c{'\0'};
        file >> c;
        if (c == 'o') {  // alive cell
            insert_alive(pattern, insert_position, extract_int(run_length));
        } else if (c == 'b') {  // dead cell
            insert_dead(insert_position, extract_int(run_length));
        } else if (std::isdigit(c)) {
            run_length.push_back(c);
        } else if (std::isspace(c)) {
            continue;
        } else if (c == '$') {
            next_row(insert_position, run_length, offset.x);
        } else if (c == '!') {
            break;
        }
    }
    return {pattern, rule};
}
}  // namespace gol
