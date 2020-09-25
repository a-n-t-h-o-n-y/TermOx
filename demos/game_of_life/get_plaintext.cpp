#include "get_plaintext.hpp"

#include <fstream>
#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

// TODO Implement
auto get_plaintext(std::string const & /* filename */) -> Pattern
{
    return {{}, parse_rule_string("B3/S23")};
}

}  // namespace gol
