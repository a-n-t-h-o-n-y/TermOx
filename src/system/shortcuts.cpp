#include <cppurses/system/shortcuts.hpp>

#include <unordered_map>

#include <signals/signal.hpp>

#include <cppurses/system/events/key.hpp>

namespace cppurses {

std::unordered_map<Key::Code, sig::Signal<void()>> Shortcuts::shortcuts_;

bool Shortcuts::enabled_{true};

auto Shortcuts::add_shortcut(Key::Code key) -> sig::Signal<void()>&
{
    if (shortcuts_.count(key) == 0) {
        shortcuts_[key] = sig::Signal<void()>{};
    }
    return shortcuts_.at(key);
}

auto Shortcuts::send_key(Key::Code key) -> bool
{
    if (enabled_ && shortcuts_.count(key) == 1) {
        shortcuts_[key]();
        return true;
    }
    return false;
}

}  // namespace cppurses
