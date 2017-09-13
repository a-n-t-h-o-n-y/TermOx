/// \file
/// \brief Contains the definition of the Connection class.
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "detail/connection_impl_base.hpp"

#include <memory>

namespace sig {

/// \brief Represents the connection made when a Slot is connected to a Signal.
///
/// Can be queried to check if the Slot is still connected to the Signal. The
/// Connection can be blocked by constructing a Shared_connection_block.
/// \sa Shared_connection_block
class Connection {
   public:
    /// Default constructors a connection that refers to no real connection.
    Connection() = default;

    /// \brief Constructor used by the Signal::connect() function.
    ///
    /// Signal uses this constructor to build Connection objects.
    /// Each Signal owns the Connection_impl, and each Connection_impl owns the
    /// connecte Slot.
    /// \param wp_cib Implementation object that holds Connection's state.
    /// \sa Signal Slot
    explicit Connection(std::weak_ptr<Connection_impl_base> wp_cib)
        : pimpl_{std::move(wp_cib)} {}

    /// \brief Disconnections the connection.
    ///
    /// The Slot associated with this connection will no longer be called by the
    /// associated Signal. If connection is already disconnected, this is a
    /// no-op.
    void disconnect() const {
        if (!this->pimpl_.expired()) {
            pimpl_.lock()->disconnect();
        }
    }

    /// \brief Query whether the connection is connected or not.
    /// \returns True if *this is connected, false otherwise.
    bool connected() const {
        if (!this->pimpl_.expired()) {
            return pimpl_.lock()->connected();
        }
        return false;
    }

    /// \brief Query whether the connection is currently blocked or not.
    ///
    /// Blocking can happen from initializing a Shared_connection_block with
    /// *this.
    /// \returns True if the connection is blocked, false otherwise.
    bool blocked() const {
        if (!this->pimpl_.expired()) {
            return pimpl_.lock()->blocked();
        }
        return false;
    }

    /// \returns True if both parameters refer to the same Signal/Slot
    /// connection.
    bool operator==(const Connection& x) const {
        return pimpl_.lock() == x.pimpl_.lock();
    }

    /// Does pointer less than comparison of underlying implementation.
    bool operator<(const Connection& x) const {
        return pimpl_.lock() < x.pimpl_.lock();
    }

    friend class Shared_connection_block;

   private:
    std::weak_ptr<Connection_impl_base> pimpl_;
};

/// \returns !(*this == x)
inline bool operator!=(const Connection& x, const Connection& y) {
    return !(x == y);
}

}  // namespace sig

#endif  // CONNECTION_HPP
