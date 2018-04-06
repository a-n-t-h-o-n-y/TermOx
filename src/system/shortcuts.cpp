#include <cppurses/system/shortcuts.hpp>

#include <map>

#include <cppurses/system/key.hpp>

namespace cppurses {

std::map<Key, sig::Signal<void()>> Shortcuts::shortcuts_{};

sig::Signal<void()>& Shortcuts::add_shortcut(Key key) {
    return shortcuts_[key] = sig::Signal<void()>{};
}

void Shortcuts::remove_shortcut(Key key) {
    shortcuts_.erase(key);
}

void Shortcuts::clear() {
    shortcuts_.clear();
}

bool Shortcuts::send_key(Key key) {
    if (shortcuts_.count(key) == 1) {
        shortcuts_[key]();
        return true;
    }
    return false;
}

}  // namespace cppurses
