#include <cppurses/system/shortcuts.hpp>

#include <unordered_map>

#include <cppurses/system/events/key.hpp>

namespace cppurses {

std::unordered_map<Key::Code, sig::Signal<void()>> Shortcuts::shortcuts_;

bool Shortcuts::enabled_ = true;

}  // namespace cppurses
