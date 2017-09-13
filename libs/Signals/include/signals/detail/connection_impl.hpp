#ifndef DETAIL_CONNECTION_IMPL_HPP
#define DETAIL_CONNECTION_IMPL_HPP

#include "connection_impl_base.hpp"
#include "placeholder_template.hpp"
#include "../slot.hpp"

#include <functional>
#include <utility>

namespace sig {
class Connection;

template <typename Signature>
class Connection_impl;

// Implementation class for Connection. This class owns the Slot involved in
// the connection. Inherits from Connection_impl_base, which implements shared
// connection block counts.
// The base class also provides an interface for the Connection class to hold a
// pointer to a non-templated impl. This is required so that connection can
// remain a non-template, while having impls that vary depending on the type of
// Slot being connected.
template <typename Ret, typename... Args>
class Connection_impl<Ret(Args...)> : public Connection_impl_base {
   public:
    using extended_slot_t = Slot<Ret(const Connection&, Args...)>;

    Connection_impl() : Connection_impl_base{}, slot_{}, connected_{false} {}

    explicit Connection_impl(const Slot<Ret(Args...)>& s)
        : Connection_impl_base{}, slot_{s}, connected_{true} {}

    // Constructs a Connection_impl with extended_slot and connection. This
    // binds the connection to the first parameter of the extended_slot_t
    // function so that the signature of the function fits with the signal type.
    // Also copies over tracked items from the extended slot into the new slot.
    Connection_impl& emplace_extended(const extended_slot_t& es,
                                      const Connection& c) {
        connected_ = true;
        slot_.slot_function() = bind_connection(es.slot_function(), c);
        for (const std::weak_ptr<void>& wp : es.get_tracked_container()) {
            slot_.track(wp);
        }
        return *this;
    }

    void disconnect() override { connected_ = false; }

    bool connected() const override { return connected_; }

    Slot<Ret(Args...)>& get_slot() { return slot_; }

    const Slot<Ret(Args...)>& get_slot() const { return slot_; }

   private:
    std::function<Ret(Args...)> bind_connection(
        std::function<Ret(const Connection&, Args...)> f,
        const Connection& c) {
        return bind_connection(
            std::forward<std::function<Ret(const Connection&, Args...)>>(f),
            std::forward<const Connection&>(c),
            std::index_sequence_for<Args...>{});
    }

    template <typename IntType, IntType... Is>
    std::function<Ret(Args...)> bind_connection(
        std::function<Ret(const Connection&, Args...)> f,
        const Connection& c,
        std::integer_sequence<IntType, Is...>) {
        return std::bind(
            std::forward<std::function<Ret(const Connection&, Args...)>>(f),
            std::forward<const Connection&>(c), Placeholder_template<Is>{}...);
    }

    Slot<Ret(Args...)> slot_;
    bool connected_;
};

}  // namespace sig

#endif  // DETAIL_CONNECTION_IMPL_HPP
