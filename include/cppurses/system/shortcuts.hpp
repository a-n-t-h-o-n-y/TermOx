#ifndef CPPURSES_SYSTEM_SHORTCUTS_HPP
#define CPPURSES_SYSTEM_SHORTCUTS_HPP
#include <unordered_map>

#include <signals/signal.hpp>

#include <cppurses/system/key.hpp>

namespace cppurses {

class Shortcuts {
   public:
    static sig::Signal<void()>& add_shortcut(Key key);
    static void remove_shortcut(Key key);
    static void clear();
    static bool send_key(Key key);

   private:
    static std::unordered_map<Key, sig::Signal<void()>> shortcuts_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SHORTCUTS_HPP
