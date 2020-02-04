#ifndef CPPURSES_SYSTEM_SHORTCUTS_HPP
#define CPPURSES_SYSTEM_SHORTCUTS_HPP
#include <unordered_map>

#include <signals/signal.hpp>

#include <cppurses/system/events/key.hpp>

namespace cppurses {

/// Provides functions for defining global keyboard shortcuts.
class Shortcuts {
   public:
    /// Add an entry for the \p key, returning a Signal to connect an action to.
    /** Key has combined key presses defined for multi-key shortcuts. The
     *  returned Signal reference will be called each time the keyboard shortcut
     *  is encountered. */
    static auto add_shortcut(Key::Code key) -> sig::Signal<void()>&
    {
        if (shortcuts_.count(key) == 0)
            shortcuts_[key] = sig::Signal<void()>{};
        return shortcuts_.at(key);
    }

    /// Stop \p key and its associated Signal from being called.
    /** No-op if key is not an existing shortcut. */
    static void remove_shortcut(Key::Code key) { shortcuts_.erase(key); }

    /// Remove all shortcuts from the system.
    static void clear() { shortcuts_.clear(); }

    /// Call on the associated Signal if \p key exists as a shortcut.
    static auto send_key(Key::Code key) -> bool
    {
        if (enabled_ and shortcuts_.count(key) == 1) {
            shortcuts_[key]();
            return true;
        }
        return false;
    }

    /// Stop all shortcuts from working.
    static void disable_all() { enabled_ = false; }

    /// Allow all shortcuts to be processed, on by default.
    static void enable_all() { enabled_ = true; }

   private:
    static std::unordered_map<Key::Code, sig::Signal<void()>> shortcuts_;
    static bool enabled_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SHORTCUTS_HPP
