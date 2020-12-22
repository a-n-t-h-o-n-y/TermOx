#ifndef TERMOX_DEMOS_GAME_OF_LIFE_RULE_STORE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_RULE_STORE_HPP
#include <map>
#include <string_view>

#include "rule.hpp"

namespace gol {

class Rule_store {
   public:
    using Name = std::wstring_view;

   private:
    std::map<Name, Rule> rules_;

   public:
    void add_rule(Name name, Rule r) { rules_[name] = r; }

    /// Get a Rule by name, throws std::out_of_range if no Rule found.
    auto get(Name name) const -> Rule { return rules_.at(name); }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_RULE_STORE_HPP
