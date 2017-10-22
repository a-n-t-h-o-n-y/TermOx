#ifndef SYSTEM_SHORTCUTS_HPP
#define SYSTEM_SHORTCUTS_HPP
#include <cppurses/system/key.hpp>

#include <signals/signal.hpp>

#include <map>

namespace cppurses {

class Shortcuts {
   public:
    static sig::Signal<void()>& add_shortcut(Key key);
    // void remove_shortcut(Key key);
    // void clear();
    static bool send_key(Key key);

   private:
    static std::map<Key, sig::Signal<void()>> shortcuts_;
};

}  // namespace cppurses

#endif  // SYSTEM_SHORTCUTS_HPP
