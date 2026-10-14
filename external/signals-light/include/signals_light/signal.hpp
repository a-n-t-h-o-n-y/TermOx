#ifndef SIGNALS_LIGHT_SIGNAL_HPP
#define SIGNALS_LIGHT_SIGNAL_HPP
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace sl {

/// Provides a const view of a std::weak_ptr, providing an is_expired() check.
/** Always contains a valid lifetime, unless moved from. */
class Lifetime_observer {
   public:
    Lifetime_observer() = delete;

    // Note: The below overloads are provided so a Lifetime_observer can be
    // implicitly constructed from std::..._ptr objects in a function parameter.

    /// Construct a view of the lifetime of the object pointed to by \p p.
    /** Throws std::invalid_argument if \p p is nullptr. */
    template <typename T>
    Lifetime_observer(std::weak_ptr<T> p) noexcept(false) : handle_{sanitize(p)}
    {}

    /// Construct a view of the lifetime of the object pointed to by \p p.
    /** Throws std::invalid_argument if \p p is nullptr. */
    template <typename T>
    Lifetime_observer(std::shared_ptr<T> p) noexcept(false)
        : handle_{sanitize(p)}
    {}

    /// Creates a new observer to the same object.
    Lifetime_observer(Lifetime_observer const&) = default;

    /// Moving will leave the moved from Lifetime_observer in an undefined state
    Lifetime_observer(Lifetime_observer&&) = default;

    /// Overwrites *this to track the same lifetime as the rhs.
    auto operator=(Lifetime_observer const&) -> Lifetime_observer& = default;

    /// Moving will leave the moved from Lifetime_observer in an undefined state
    auto operator=(Lifetime_observer&&) -> Lifetime_observer& = default;

   public:
    /// Return true if the tracked object has been deleted.
    auto is_expired() const noexcept -> bool { return handle_.expired(); }

    /// Return a unique id that is associated with the tracked object
    /** Returns zero if the tracked object has been destroyed. */
    auto get_id() const noexcept -> std::uintptr_t
    {
        if (this->is_expired())
            return 0;
        return reinterpret_cast<std::uintptr_t>(handle_.lock().get());
    }

   private:
    std::weak_ptr<void> handle_;

   private:
    /// Throws std::invalid_argument if p == nullptr, returns \p p otherwise.
    template <typename Pointer>
    static auto sanitize(Pointer p) noexcept(false) -> Pointer
    {
        auto constexpr message = "Lifetime_observer: Can't observer a nullptr.";
        return p == nullptr ? throw std::invalid_argument{message} : p;
    }
};

/// A class to keep track of an object's lifetime.
/** A Lifetime_observer can check if a Lifetime has ended. */
class Lifetime {
   public:
    /// Create a new lifetime to track.
    Lifetime() noexcept(false) : life_{std::make_shared<bool>(true)} {}

    /// Create a new lifetime to track.
    /** Tracking does not split across multiple Lifetime objects. */
    Lifetime(Lifetime const&) noexcept(false)
        : life_{std::make_shared<bool>(true)}
    {}

    /// Transfers the lifetime tracking to the new instance.
    /** Existing trackers will now track the newly constructed lifetime. */
    Lifetime(Lifetime&&) = default;

    /// Create a new lifetime to track, destroying the existing lifetime.
    /** Tracking does not split across multiple Lifetime objects. */
    auto operator=(Lifetime const& rhs) noexcept(false) -> Lifetime&
    {
        if (this == &rhs)
            return *this;
        life_ = std::make_shared<bool>(true);
        return *this;
    }

    /// Transfers the lifetime of the moved from object to *this.
    /** Anything tracking the moved from object will now be tracking *this. */
    auto operator=(Lifetime&& rhs) noexcept -> Lifetime&
    {
        if (this == &rhs)
            return *this;
        life_ = std::move(rhs.life_);
        return *this;
    }

   public:
    /// Return a Lifetime_observer, to check if *this has been destroyed.
    /** The returned object is valid even after *this i\ destroyed. Even though
     *  this should never throw an exception, if a memory allocation fails at
     *  construction, it might not throw std::bad_alloc, returning nullptr. */
    auto track() const noexcept(false) -> Lifetime_observer { return life_; }

   private:
    std::shared_ptr<bool> life_;
};

template <typename Signature>
class Slot;

/// An invocable type with object lifetime tracking.
/** An object of this type always holds a valid std::function object.
 *  The call operator will throw if any of the tracked objects are destroyed. */
template <typename R, typename... Args>
class Slot<R(Args...)> {
   public:
    using Signature_t = R(Args...);
    using Function_t  = std::function<Signature_t>;

    /// Exception thrown if any object being tracked has been destroyed.
    class Expired {};

   public:
    Slot() = delete;

    Slot(std::nullptr_t) = delete;

    /// Construct a Slot with the slot function \p f and no tracked objects.
    /** F must be invocable with Args... and return convertible to R. */
    template <typename F>
    Slot(F f) : f_{f}
    {
        static_assert(std::is_invocable_r_v<R, F, Args...>,
                      "Slot initialization with invalid function type.");
    }

    /// Construct a Slot with the slot function \p f and no tracked objects.
    /** Throws std::invalid_argument if the given function is empty. */
    Slot(Function_t f) noexcept(false) : f_{sanitize(std::move(f))} {}

    /// Copies the slot function and the tracked list into the new Slot.
    /** Tracked objects by *this are tracked by the new Slot instance. */
    Slot(Slot const&) = default;

    /// Moves the slot function and the tracked list into the new Slot.
    Slot(Slot&&) = default;

    /// Copies the slot function and the tracked object reference container.
    auto operator=(Slot const&) -> Slot& = default;

    /// Moves the slot function and the tracked object reference container.
    auto operator=(Slot&&) -> Slot& = default;

   public:
    /// Track any object owned by a std::shared_ptr<...>.
    /** Returns *this. Tracking the same item multiple times is not checked. */
    auto track(Lifetime_observer const& x) noexcept(false) -> Slot&
    {
        observers_.push_back(x);
        return *this;
    }

    /// Track Lifetime object. Convenience.
    /** Returns *this. Tracking the same item multiple times is not checked. */
    auto track(Lifetime const& x) noexcept(false) -> Slot&
    {
        this->track(x.track());
        return *this;
    }

    /// Remove the observed object from the tracked objects list.
    /** Removes on Lifetime_observer::get_id() equality. Returns *this. Throws
     *  std::invalid_argument if \p x is not being tracked by *this. */
    auto untrack(Lifetime_observer const& x) noexcept(false) -> Slot&
    {
        auto const iter =
            std::find_if(std::cbegin(observers_), std::cend(observers_),
                         [&x](Lifetime_observer const& y) {
                             return x.get_id() == y.get_id();
                         });
        if (iter == std::cend(observers_))
            throw std::invalid_argument{"Slot::untrack: x not found."};
        observers_.erase(iter);
        return *this;
    }

    /// Remove the given Lifetime object from the tracked objects list.
    /** Removes on Lifetime_observer::get_id() equality. Returns *this. Throws
     *  std::invalid_argument if \p x is not being tracked by *this.*/
    auto untrack(Lifetime const& x) noexcept(false) -> Slot&
    {
        return this->untrack(x.track());
    }

    /// Invokes the internally held function
    /** Throws Expired if any tracked object has been destroyed. */
    template <typename... Arguments>
    auto operator()(Arguments&&... args) const noexcept(false) -> R
    {
        if (is_expired())
            throw Expired{};
        return f_(std::forward<Arguments>(args)...);
    }

    /// Check whether any of the tracked lifetimes has expired.
    /** Returns true if no lifetimes are being tracked. */
    auto is_expired() const noexcept -> bool
    {
        return std::any_of(
            std::cbegin(observers_), std::cend(observers_),
            [](Lifetime_observer const& x) { return x.is_expired(); });
    }

    /// Return a const reference to the internal std::function.
    /** Always returns a valid Function_t object that can be called. */
    auto slot_function() const noexcept -> Function_t const& { return f_; }

   private:
    Function_t f_;
    std::vector<Lifetime_observer> observers_;

   private:
    /// Throws std::invalid_argument if f == nullptr, returns \p f otherwise.
    static auto sanitize(Function_t f) noexcept(false) -> Function_t
    {
        auto constexpr message = "Slot must be initialized with valid function";
        return f ? std::move(f) : throw std::invalid_argument{message};
    }
};

/// Objects of this type can be unique and compared against other identifiers.
class Identifier {
   public:
    using Underlying_int = std::uint32_t;

   public:
    /// Construct the initial value.
    Identifier() noexcept : value_{0} {}

    /// Generate the next identifier value, this is an increment.
    static auto next(Identifier x) noexcept -> Identifier
    {
        return {x.value_ + 1};
    }

   public:
    /// Return true if both Identifiers have the same internal value.
    friend auto operator==(Identifier x, Identifier y) noexcept -> bool
    {
        return x.value_ == y.value_;
    }

    /// Return true if both Identifiers do not have the same internal value.
    friend auto operator!=(Identifier x, Identifier y) noexcept -> bool
    {
        return !(x == y);
    }

   private:
    /// Used by next(...).
    Identifier(Underlying_int value) noexcept : value_{value} {}

   private:
    Underlying_int value_;
};

template <typename Signature>
class Signal;

/// An observer type that calls registered callbacks(Slots) when emitted.
template <typename R, typename... Args>
class Signal<R(Args...)> {
   public:
    using Signature_t = R(Args...);
    using Emit_result_t =
        std::conditional_t<std::is_same_v<void, R>, void, std::optional<R>>;

   public:
    /// Construct a Signal with no connected Slots.
    Signal() = default;

    /// Construct a Signal with the given Slot connected.
    Signal(Slot<Signature_t> slot) { this->connect(std::move(slot)); }

    /// Construct a Signal with the given Slot function connected.
    /// Seems to be the only way to pass a lambda in certain contexts.
    template <typename F>
    Signal(F slot_fn)
    {
        static_assert(std::is_invocable_r_v<R, F, Args...>,
                      "Slot initialization with invalid function type.");
        this->connect(Slot<Signature_t>{std::move(slot_fn)});
    }

    /// Create a Signal with the same Slots connected, and the same Identifiers.
    Signal(Signal const& other) : next_id_{other.next_id_}
    {
        slots_.reserve(other.slots_.size());
        for (auto const& [id, slot] : other.slots_) {
            slots_.push_back({id, std::make_shared<Entry>(*slot)});
        }
    }

    /// Move the connected Slots from the existing Signal to the new one.
    /** The moved from Signal will be empty afterwards. */
    Signal(Signal&&) noexcept = default;

    /// Overwrite the existing Signal with the Slots and Identifiers of the rhs.
    auto operator=(Signal other) noexcept -> Signal&
    {
        slots_.swap(other.slots_);
        std::swap(next_id_, other.next_id_);
        return *this;
    }
    ~Signal()
    {
        for (auto const& [id, slot] : slots_) { slot->second = false; }
    }

   public:
    /// Invoke all non-expired Slots.
    /** Returns the return value of the last connected Slot or std::nullopt if
     *  none. Expired Slots are ignored, rather than throwing an exception. */
    auto emit(Args const&... args) const -> Emit_result_t
    {
        // Snapshot shared entries, not std::functions: mutable callbacks retain state.
        // Removal/owner destruction cancels pending entries; additions wait until the
        // next emission. No access to this is needed after invoking user code.
        auto const snapshot = slots_;
        if constexpr (std::is_same_v<void, R>) {
            for (auto const& [id, entry] : snapshot) {
                if (entry->second && !entry->first.is_expired()) {
                    entry->first.slot_function()(args...);
                }
            }
        }
        else {
            auto result = std::optional<R>{};
            for (auto const& [id, entry] : snapshot) {
                if (entry->second && !entry->first.is_expired()) {
                    result = entry->first.slot_function()(args...);
                }
            }
            return result;
        }
    }

    /// Alternative notation for Signal::emit.
    auto operator()(Args const&... args) const -> Emit_result_t
    {
        return this->emit(args...);
    }

    /// Register a Slot with *this, will be invoked when *this is emitted.
    /** Returns a unique Identifier, to be used with Signal::disconnect. */
    auto connect(Slot<Signature_t> s) noexcept(false) -> Identifier
    {
        auto const id = next_id_;
        auto entry = std::make_shared<Entry>(std::move(s), true);
        slots_.push_back({id, std::move(entry)});
        next_id_ = Identifier::next(next_id_);
        return id;
    }

    /// Removes and returns the Slot associated with the given Identifier.
    /** Throws std::invalid_argument if no connected Slot is found with id. */
    auto disconnect(Identifier id) noexcept(false) -> Slot<Signature_t>
    {
        auto const iter = std::find_if(
            std::cbegin(slots_), std::cend(slots_),
            [id](auto const& id_slot) { return id_slot.first == id; });
        if (iter == std::cend(slots_))
            throw std::invalid_argument{"Signal::disconnect: No matching id."};
        auto slot = iter->second->first;
        iter->second->second = false;
        slots_.erase(iter);
        return slot;
    }

    /// Return the number of connected Slots.
    auto slot_count() const noexcept -> std::size_t { return slots_.size(); }

    /// Return true if there are no connected Slots.
    auto is_empty() const noexcept -> bool { return slots_.empty(); }

   private:
    using Entry = std::pair<Slot<R(Args...)>, bool>;
    std::vector<std::pair<Identifier, std::shared_ptr<Entry>>> slots_;
    Identifier next_id_;
};

}  // namespace sl
#endif  // SIGNALS_LIGHT_SIGNAL_HPP
