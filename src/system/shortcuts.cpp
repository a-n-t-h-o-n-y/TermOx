#include "system/shortcuts.hpp"

#include <map>

namespace cppurses {

std::map<Key, sig::Signal<void()>> Shortcuts::shortcuts_{};

sig::Signal<void()>& Shortcuts::add_shortcut(Key key) {
    return shortcuts_[key] = sig::Signal<void()>{};
}

bool Shortcuts::send_key(Key key) {
    if (shortcuts_.count(key) == 1) {
        shortcuts_[key]();
        return true;
    }
    return false;
}

}  // namespace cppurses
