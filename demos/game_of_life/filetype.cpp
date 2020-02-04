#include "filetype.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <string>

namespace {
/// Remove all control characters from the string.
void remove_controls(std::string& s)
{
    const auto end = std::remove_if(std::begin(s), std::end(s),
                                    [](char c) { return std::iscntrl(c); });
    s.erase(end, std::end(s));
}
}  // namespace

namespace gol {

FileType get_filetype(const std::string& filename)
{
    auto result     = FileType::Unknown;
    auto input_file = std::ifstream{filename};
    if (input_file.fail()) {
        return result;
    }
    std::string first_line;
    std::getline(input_file, first_line);
    remove_controls(first_line);
    if (first_line == "#Life 1.05") {
        result = FileType::Life_1_05;
    }
    else if (first_line == "#Life 1.06") {
        result = FileType::Life_1_06;
    }
    else if (first_line.size() > 5 and first_line.substr(0, 6) == "!Name:") {
        result = FileType::Plaintext;
    }
    else {
        while (!input_file.fail() and !first_line.empty() and
               first_line[0] == '#') {
            std::getline(input_file, first_line);
        }
        if (first_line.size() > 3 and first_line.substr(0, 3) == "x =") {
            result = FileType::RLE;
        }
    }
    return result;
}

std::string get_extension(const std::string& filename)
{
    auto result   = std::string{""};
    const auto at = std::find(std::rbegin(filename), std::rend(filename), '.');
    if (at != std::rend(filename)) {
        result = std::string{at.base(), std::end(filename)};
    }
    return result;
}

}  // namespace gol
