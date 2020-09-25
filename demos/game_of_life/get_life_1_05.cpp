#include "get_life_1_05.hpp"

#include <string>

#include "coordinate.hpp"
#include "pattern.hpp"

namespace gol {

// TODO Implement this
auto get_life_1_05(std::string const & /*filename*/) -> Pattern
{
    return {{}, parse_rule_string("B3/S23")};
}

}  // namespace gol
