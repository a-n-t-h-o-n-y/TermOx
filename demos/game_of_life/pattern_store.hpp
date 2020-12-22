#ifndef TERMOX_DEMOS_GAME_OF_LIFE_PATTERN_STORE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_PATTERN_STORE_HPP
#include <map>
#include <string_view>

#include "pattern.hpp"

namespace gol {

// TODO this can be generic Store, that stores some type by names, then you
// give it Pattern or Rule as a template

class Pattern_store {
   public:
    using Name = std::wstring_view;

   private:
    std::map<Name, Pattern> patterns_;

   public:
    void add_pattern(Name name, Pattern pattern)
    {
        patterns_[name] = std::move(pattern);
    }

    auto get(Name name) const -> Pattern { return patterns_.at(name); }
};

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_PATTERN_STORE_HPP
