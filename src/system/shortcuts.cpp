#include <termox/system/shortcuts.hpp>

#include <signals_light/signal.hpp>

#include <termox/system/key.hpp>

namespace ox {

auto Shortcuts::add_shortcut(Key k) -> sl::Signal<void()>&
{
    if (shortcuts_.count(k) == 0)
        shortcuts_[k] = sl::Signal<void()>{};
    return shortcuts_.at(k);
}

void Shortcuts::remove_shortcut(Key k) { shortcuts_.erase(k); }

void Shortcuts::clear() { shortcuts_.clear(); }

auto Shortcuts::send_key(Key k) -> bool
{
    if (enabled_ && shortcuts_.count(k) == 1) {
        shortcuts_[k]();
        return true;
    }
    return false;
}

void Shortcuts::disable_all() { enabled_ = false; }

void Shortcuts::enable_all() { enabled_ = true; }

}  // namespace ox
