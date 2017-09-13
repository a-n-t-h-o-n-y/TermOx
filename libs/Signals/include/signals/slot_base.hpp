/// \file
/// \brief Contains definition of Slot_base class.
#ifndef SLOT_BASE_HPP
#define SLOT_BASE_HPP

#include <memory>
#include <vector>

namespace sig {

/// \brief Base class for Slot, handles tracked objects.
class Slot_base {
   public:
    using locked_container_type = std::vector<std::shared_ptr<void>>;
    using tracked_container_type = std::vector<std::weak_ptr<void>>;

    /// \returns True if any tracked object has been destroyed.
    bool expired() const {
        for (auto& tracked : tracked_ptrs_) {
            if (tracked.expired()) {
                return true;
            }
        }
        return false;
    }

    /// \brief Locks the tracked objects so they cannot be destroyed.
    ///
    /// \returns Container of locked objects, as long as this container is alive
    /// all of the tracked objects will be alive, as long as they have not
    /// already expired.
    locked_container_type lock() const {
        locked_container_type locked_vec;
        for (auto& tracked : tracked_ptrs_) {
            locked_vec.push_back(tracked.lock());
        }
        return locked_vec;
    }

    /// \returns The internally held container of tracked objects.
    const tracked_container_type& get_tracked_container() const {
        return tracked_ptrs_;
    }

   protected:
    tracked_container_type tracked_ptrs_;
};

}  // namespace sig

#endif  // SLOT_BASE_HPP
