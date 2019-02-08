#ifndef CPPURSES_SYSTEM_SHORTCUTS_HPP
#define CPPURSES_SYSTEM_SHORTCUTS_HPP
#include <unordered_map>

#include <signals/signal.hpp>

#include <cppurses/system/key.hpp>

namespace cppurses {

/// Provides functions for defining global keyboard shortcuts.
class Shortcuts {
   public:
    /// Add an entry for the \p key, returning a Signal to connect an action to.
    /** Key has combined key presses defined for multi-key shortcuts. The
     *  returned Signal reference will be called each time the keyboard shortcut
     *  is encountered. */
    static sig::Signal<void()>& add_shortcut(Key key);

    /// Stop \p key and its associated Signal from being called.
    /** No-op if key is not an existing shortcut. */
    static void remove_shortcut(Key key) { shortcuts_.erase(key); }

    /// Remove all shortcuts from the system.
    static void clear() { shortcuts_.clear(); }

    /// Call on the associated Signal if \p key exists as a shortcut.
    static bool send_key(Key key);

   private:
    static std::unordered_map<Key, sig::Signal<void()>> shortcuts_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SHORTCUTS_HPP
