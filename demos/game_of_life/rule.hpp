#ifndef TERMOX_DEMOS_GAME_OF_LIFE_RULE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_RULE_HPP
#include <bitset>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace gol {

/// Game of Life RuleString Representation.
/** Each index into a bitset represents the number of neighboring cells. */
struct Rule {
    using Neighbors = std::bitset<9>;
    Neighbors birth;  // Numbers of neighbor cells for an empty cell to be born.
    Neighbors survival;  // Numbers of neighbor cells where a cell will survive.
};

constexpr auto value_of(char n) -> std::uint16_t { return n - '0'; }

/// Each char is interpreted as a number of neighbors.
constexpr auto make_neighbors(std::string_view numbers) -> Rule::Neighbors
{
    auto bits = 0uLL;
    for (auto ch : numbers)
        bits |= 1uLL << value_of(ch);
    return {bits};
}

constexpr auto parse_error_str() -> char const*
{
    return "parse_rule_string() invalid argument.";
}

constexpr auto parse_bs_rulestring(std::string_view rs) -> Rule
{
    auto const delimit_pos = rs.find('/');
    if (delimit_pos == std::string_view::npos)
        throw std::invalid_argument{parse_error_str()};
    if (delimit_pos + 1uL >= rs.size())
        throw std::invalid_argument{parse_error_str()};
    if (rs[delimit_pos + 1uL] != 'S' && rs[delimit_pos + 1uL] != 's')
        throw std::invalid_argument{parse_error_str()};

    auto const b = make_neighbors(rs.substr(1uL, delimit_pos - 1uL));
    auto const s = make_neighbors(rs.substr(delimit_pos + 2uL));
    return {b, s};
}

constexpr auto parse_sb_rulestring(std::string_view rs) -> Rule
{
    auto const delimit_pos = rs.find('/');
    if (delimit_pos == std::string_view::npos)
        throw std::invalid_argument{parse_error_str()};

    auto const s = make_neighbors(rs.substr(0uL, delimit_pos));
    auto const b = make_neighbors(rs.substr(delimit_pos + 1uL));
    return {s, b};
}

/// Add \p i to \p ss if \p rule contains \p i.
inline void add_if_set(std::ostringstream& ss,
                       Rule::Neighbors const& rule,
                       int i)
{
    if (rule[i])
        ss << i;
}

/// Creates a Rule object from a Game of Life Rulestring
/** conwaylife.com/wiki/Rulestring
 *  B/S Format: B\d*\/S\d*
 *  S/B Format: \d*\/\d*
 *  Throws std::invalid_argument if string is not in proper format.
 */
constexpr auto parse_rule_string(std::string_view rs) -> Rule
{
    if (rs.empty())
        throw std::invalid_argument{parse_error_str()};
    if (rs[0] == 'B' || rs[0] == 'b')
        return parse_bs_rulestring(rs);
    else
        return parse_sb_rulestring(rs);
}

/// Turns Rule into a B/S rulestring
inline auto to_rule_string(Rule r) -> std::string
{
    auto ss = std::ostringstream{};
    ss << 'B';
    for (auto i = 0; i < 9; ++i)
        add_if_set(ss, r.birth, i);
    ss << "/S";
    for (auto i = 0; i < 9; ++i)
        add_if_set(ss, r.survival, i);
    return ss.str();
}

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_RULE_HPP
