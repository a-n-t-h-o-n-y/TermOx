#include <cppurses/widget/widgets/cycle_box.hpp>

#include <cstddef>
#include <string>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/detail/tracks_lifetime.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace {

/// \p edge is one-past-the-last element, will go to zero instead of \p edge
/** Input Constraints: \p edge > 0  and \p i < \p edge */
auto wrapped_increment(std::size_t i, std::size_t edge) -> std::size_t
{
    return ++i == edge ? 0 : i;
}

/// \p edge is one-past-the-last element, will go to edge - 1 instead of -1.
/** Input Constraints: \p edge > 0  and \p i < \p edge */
auto wrapped_decrement(std::size_t i, std::size_t edge) -> std::size_t
{
    return i == 0 ? --edge : --i;
}

}  // namespace

namespace cppurses {

void Cycle_box::next()
{
    if (this->size() < 2)
        return;
    this->set_current_option(wrapped_increment(index_, this->size()));
    this->emit_signals();
}

void Cycle_box::previous()
{
    if (this->size() < 2)
        return;
    this->set_current_option(wrapped_decrement(index_, this->size()));
    this->emit_signals();
}

namespace slot {
using namespace detail;

auto add_option(Cycle_box& cb) -> sig::Slot<void(Glyph_string)>
{
    return tracks_lifetime(
        cb, [&cb](Glyph_string label) { cb.add_option(std::move(label)); });
}

auto add_option(Cycle_box& cb, Glyph_string const& label) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb, label] { cb.add_option(label); });
}

auto remove_option(Cycle_box& cb) -> sig::Slot<void(std::string const&)>
{
    return tracks_lifetime(
        cb, [&cb](std::string const& label) { cb.remove_option(label); });
}

auto remove_option(Cycle_box& cb, std::string const& label) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb, label] { cb.remove_option(label); });
}

auto next(Cycle_box& cb) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb]() { cb.next(); });
}

auto previous(Cycle_box& cb) -> sig::Slot<void()>
{
    return tracks_lifetime(cb, [&cb]() { cb.previous(); });
}
}  // namespace slot
}  // namespace cppurses
