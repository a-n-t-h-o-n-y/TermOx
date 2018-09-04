#include <cppurses/system/shortcuts.hpp>

#include <unordered_map>

#include <signals/signal.hpp>

#include <cppurses/system/key.hpp>

namespace cppurses {

std::unordered_map<Key, sig::Signal<void()>> Shortcuts::shortcuts_;

sig::Signal<void()>& Shortcuts::add_shortcut(Key key) {
    if (shortcuts_.count(key) == 0) {
        shortcuts_[key] = sig::Signal<void()>{};
    }
    // return shortcuts_[key];
    return shortcuts_.at(key);
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
