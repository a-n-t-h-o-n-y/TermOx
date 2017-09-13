/// \file
/// \brief Contains the definition of the Slot template class.
#ifndef SLOT_HPP
#define SLOT_HPP

#include "slot_base.hpp"
#include "expired_slot.hpp"
#include "signal_fwd.hpp"

#include "slot_fwd.hpp"

#include <functional>
#include <memory>
#include <tuple>
#include <vector>

namespace sig {

/// \brief Represents a function that can be connected to a Signal.
///
/// Slots can track other objects to mirror their lifetime. If one tracked
/// object is destroyed, *this will no longer be called by any Signal. Slots
/// can be called like any other function, with operator(). Is _not_ nothrow
/// move constructible/assignable because std::function is not.
/// \param Ret Return type of the function.
/// \param Args... Argument types to the function.
/// \param FunctionType Internally held type where the function will be stored.
template <typename Ret, typename... Args, typename FunctionType>
class Slot<Ret(Args...), FunctionType> : public Slot_base {
   public:
    using result_type = Ret;
    using argument_types = std::tuple<Args...>;
    using signature_type = Ret(Args...);
    using slot_function_type = FunctionType;

    /// Number of arguments to the function.
    static const int arity = std::tuple_size<argument_types>::value;

    /// Access to the type of argument number \p n.
    template <unsigned n>
    class arg {
       public:
        using type = typename std::tuple_element<n, argument_types>::type;
    };

    /// Creates an empty Slot, throws std::bad_function_call if call attempted.
    Slot() = default;

    /// \brief Construct from any type convertible to FunctionType.
    ///
    /// \param func Function pointer, lambda, functor, etc... to be stored.
    template <typename Function>
    Slot(const Function& func) : function_{func} {}

    /// \brief Constructs from a Signal, automatically tracks the Signal.
    ///
    /// \param sig Signal stored in *this. Requires same signature as *this.
    template <typename T,
              typename U,
              typename V,
              typename W,
              typename X,
              typename Y>
    Slot(const Signal<T, U, V, W, X, Y>& sig) : function_{*(sig.lock_impl())} {
        track(sig);
    }

    /// \brief Calls underlying function if no tracked objects are expired.
    ///
    /// Passes on args... to the FunctionType object. No-op if any tracked
    /// objects no longer exist. Throws std::bad_function_call if Slot is empty.
    /// \param args... Arguments passed onto the underlying function.
    /// \param result_type The value returned by the function call.
    template <typename... Arguments>
    result_type operator()(Arguments&&... args) const {
        if (this->expired()) {
            return result_type();
        }
        auto l = this->lock();
        return function_(std::forward<Arguments>(args)...);
    }

    /// A throwing version of operator(), if any tracked objects are expired.
    template <typename... Arguments>
    result_type call(Arguments&&... args) const {
        auto l = this->expired() ? throw Expired_slot() : this->lock();
        return function_(std::forward<Arguments>(args)...);
    }

    /// \brief Adds a shared object to the tracked objects list.
    ///
    /// \param obj_ptr a weak_ptr to the object to be tracked.
    /// \returns Reference to *this.
    Slot& track(const std::weak_ptr<void>& obj_ptr) {
        tracked_ptrs_.push_back(obj_ptr);
        return *this;
    }

    /// \brief Adds a Signal to the tracked objects list.
    ///
    /// \param sig Signal with same signature as *this.
    /// \returns Reference to *this.
    template <typename T,
              typename U,
              typename V,
              typename W,
              typename X,
              typename Y>
    Slot& track(const Signal<T, U, V, W, X, Y>& sig) {
        tracked_ptrs_.push_back(sig.lock_impl_as_void());
        return *this;
    }

    /// Copies the tracked objects from slot to *this.
    ///
    /// Does not track slot itself.
    /// \param slot Slot to copy the tracked objects from.
    /// \returns Reference to *this.
    Slot& track(const Slot_base& slot) {
        tracked_ptrs_.insert(std::begin(tracked_ptrs_),
                             std::begin(slot.get_tracked_container()),
                             std::end(slot.get_tracked_container()));
        return *this;
    }

    /// \returns Reference to the internally held FunctionType object.
    slot_function_type& slot_function() { return function_; }

    /// \returns const reference to the internally held FunctionType object.
    const slot_function_type& slot_function() const { return function_; }

   private:
    slot_function_type function_;
};

template <typename Ret, typename... Args, typename FunctionType>
const int Slot<Ret(Args...), FunctionType>::arity;

}  // namespace sig

#endif  // SLOT_HPP
