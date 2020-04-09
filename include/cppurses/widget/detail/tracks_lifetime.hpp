#ifndef CPPURSES_WIDGET_DETAIL_TRACKS_LIFETIME_HPP
#define CPPURSES_WIDGET_DETAIL_TRACKS_LIFETIME_HPP
#include <utility>

#include <cppurses/widget/detail/memfn_signature.hpp>

#include <signals/slot.hpp>

namespace cppurses::slot {

/// Construct a sig::Slot with \p method and track \p widget's destroyed signal.
template <typename W,
          typename F,
          typename Signature = detail::Call_signature_t<F>>
auto tracks_lifetime(W& widget, F&& method) -> sig::Slot<Signature>
{
    auto slot = sig::Slot<Signature>{std::forward<F>(method)};
    slot.track(widget.destroyed);
    return slot;
}

}  // namespace cppurses::slot
#endif  // CPPURSES_WIDGET_DETAIL_TRACKS_LIFETIME_HPP
