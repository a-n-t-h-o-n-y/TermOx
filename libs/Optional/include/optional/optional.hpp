///	\file
/// \brief Contains Optional template class definition.
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP
#include "bad_optional_access.hpp"
#include "none.hpp"
#include "detail/aligned_storage.hpp"
#include <utility>
#include <new>

namespace opt {

/// \brief Wraps a type to provide an optional 'null', or empty state.
///
/// A wrapped object is accessed by dereferencing an Optional object. Optional
/// objects can be tested as a boolean for whether or not they contain a value.
/// Useful when 0, -1, or default constructed does not suffice for 'no value'.
///
/// Typical usage:
/// \code
/// Optional<int> opt_i{5};
/// if(opt_i) {
///     foo(*opt_i);
/// }
/// \endcode
template <typename T>
class Optional {
   public:
    using value_type = T;
    using reference_type = T&;
    using const_reference_type = const T&;
    using rval_reference_type = T&&;
    using pointer_type = T*;
    using pointer_const_type = const T*;

    /// \brief Default constructs an Optional.
    ///
    /// *this is _not_ initialized, T's default constructor is _not_ called.
    Optional() noexcept : initialized_{false} {}

    /// \brief Constructs an uninitialized Optional.
    ///
    /// *this is _not_ initialized, T's default constrcutor is _not_ called.
    /// \param n    Use opt::none provided in none.hpp.
    /// \sa none
    Optional(None_t n) noexcept : initialized_{false} {}

    /// \brief Constructs an initialized Optional from a T object.
    ///
    /// *this is initialized with a copy of \p value.
    /// \param value    Value which is copied into the Optional.
    Optional(const_reference_type value) { this->construct(value); }

    /// \brief Constructs an initialized Optional from a moveable T object.
    ///
    /// \p value is move constructed into the Optional object.
    /// \param value    Value which is moved into the Optional.
    Optional(rval_reference_type value) { this->construct(std::move(value)); }

    /// \brief Conditionally constructs an initialized Optional.
    ///
    /// If condition is true, \p value is copy constructed into *this, otherwise
    /// *this is not initialized.
    /// \param condition    Determines if Optional is initialized or not.
    /// \param value        Object copied into *this if \p condition is true.
    Optional(bool condition, const_reference_type value) {
        if (condition) {
            this->construct(value);
        }
    }

    /// \brief Conditionally constructs an Optional from a moveable T object.
    ///
    /// If condition is true, \p value is move constructed into *this, otherwise
    /// *this is not initialized.
    /// \param condition    Determines if Optional is initialized or not.
    /// \param value        Object moved into *this if \p condition is true.
    Optional(bool condition, rval_reference_type value) {
        if (condition) {
            this->construct(std::move(value));
        }
    }

    /// Destructs the underlying object
    ~Optional() { this->destroy(); }

    /// \brief Copy constructs an Optional.
    ///
    /// If \p rhs is initialized, then a copy of its value will be constructed
    /// within *this, and *this will be initialized, otherwise *this is default
    /// constructed.
    /// \param rhs  Optional to copy, T must be copy constructible.
    Optional(const Optional& rhs) {
        if (rhs.initialized_) {
            this->construct(rhs.get());
        }
    }

    /// \brief Move constructs an Optional.
    ///
    /// If \p rhs is initialized, then its internally held value is moved into
    /// *this, and \p rhs will be empty, otherwise *this is default constructed.
    /// \param rhs  Optional to move, T must be move constructible.
    Optional(Optional&& rhs) noexcept {
        if (rhs.initialized_) {
            this->construct(std::move(rhs.get()));
            rhs.initialized_ = false;
        }
    }

    /// \brief Copy constructs from implicitly convertible type.
    ///
    /// If \p rhs is initialized and U is implicitly convertible to T, then
    /// *this is initialized with a copy of \p rhs.
    /// \param rhs  Object to be copied into *this.
    template <typename U>
    explicit Optional(const Optional<U>& rhs) {
        if (rhs.initialized_) {
            this->construct(rhs.get());
        }
    }

    /// \brief Move constructs from implicitly convertible type.
    ///
    /// If \p rhs is initialized and U is implicitly convertible to T, then
    /// *this is initialized with \p rhs. T must have a move constructor defined
    /// from type U.
    /// \param rhs  Object to be moved into *this.
    template <typename U>
    explicit Optional(Optional<U>&& rhs) {
        if (rhs.initialized_) {
            this->construct(std::move(rhs.get()));
            rhs.initialized_ = false;
        }
    }

    /// \brief Copy assignment operator.
    ///
    /// The object held by *this is destroyed and replaced with the contents of
    /// \p rhs. If \p rhs is empty, *this is left empty.
    /// \param rhs  Object to be copied into *this.
    Optional& operator=(const Optional& rhs) {
        this->destroy();
        if (rhs.initialized_) {
            this->construct(rhs.get());
        }
        return *this;
    }

    /// \brief Move assignement operator.
    ///
    /// The object held by *this is destroyed and replaced with the contents of
    /// \p rhs. If \p rhs is empty, *this is left empty. rhs is left in an
    /// invalid state.
    /// \param rhs  Object to be moved into *this.
    Optional& operator=(Optional&& rhs) noexcept {
        this->destroy();
        if (rhs.initialized_) {
            this->construct(std::move(rhs.get()));
            rhs.initialized_ = false;
        }
        return *this;
    }

    /// \brief None_t assignement operator.
    ///
    /// Leaves *this in an uninitialized state. If *this was previously
    /// initialized, the held object is destroyed.
    /// \param n    Use opt::none provided in none.hpp.
    /// \sa none
    Optional& operator=(None_t) noexcept {
        this->destroy();
        return *this;
    }

    /// \brief Value copy assignment operator.
    ///
    /// If *this is initialized, the object held is destroyed and replaced with
    /// a copy of \p value. T must be copy constructible.
    /// \param value    Value to be copied into *this.
    Optional& operator=(const_reference_type value) {
        this->destroy();
        this->construct(value);
        return *this;
    }

    /// \brief Value move assignment operator.
    ///
    /// If *this is initialized, the object held is destroyed and replaced with
    /// \p value. T must be move constructible.
    /// \param value    Value to be moved into *this.
    Optional& operator=(rval_reference_type value) {
        this->destroy();
        this->construct(std::move(value));
        return *this;
    }

    /// \brief Converting copy assignment operator.
    ///
    /// If *this is initialized, the object held is destroyed and replaced with
    /// a copy of \p rhs. U must be implicitly convertible to T.
    /// \param rhs  Optional to be copied to *this.
    template <typename U>
    Optional& operator=(const Optional<U>& rhs) {
        this->destroy();
        if (rhs.initialized_) {
            this->construct(rhs.get());
        }
        return *this;
    }

    /// \brief Converting move assignment operator.
    ///
    /// If *this is initialized, the object held is destroyed and replace with
    /// \p rhs. T must have a move constructor from type U.
    /// \param rhs  Optional to be moved to *this.
    template <typename U>
    Optional& operator=(Optional<U>&& rhs) {
        this->destroy();
        if (rhs.initialized_) {
            this->construct(std::move(rhs.get()));
            rhs.initialized_ = false;
        }
        return *this;
    }

    /// \brief Directly construct a value inside of an existing Optional.
    ///
    /// If *this is initialized, the held object is destroyed. The \p args are
    /// forwarded to the constructor of T.
    template <typename... Args>
    void emplace(Args&&... args) {
        this->destroy();
        this->emplace_construct(std::forward<Args>(args)...);
    }

    /// \brief Returns a reference to the held value.
    ///
    /// Undefined if *this is uninitialized.
    /// \returns const reference to the underlying object.
    const_reference_type get() const { return storage_.ref(); }

    /// \brief Returns a reference to the held value.
    ///
    /// Undefined if *this is uninitialized.
    /// \returns Reference to the underlying object.
    reference_type get() { return storage_.ref(); }

    /// \brief Member access overload to underlying object.
    ///
    /// Undefined if *this is uninitialized.
    /// \returns const pointer to the underlying object.
    pointer_const_type operator->() const { return storage_.ptr_ref(); }

    /// \brief Member access overload to underlying object.
    ///
    /// Undefined if *this is uninitialized.
    /// \returns Pointer to the underlying object.
    pointer_type operator->() { return storage_.ptr_ref(); }

    /// \brief Provides direct access to the underlying object.
    ///
    /// Undefined if *this is uninitialized. Overloaded on const &.
    /// \returns const l-value reference to the held object.
    const_reference_type operator*() const & { return storage_.ref(); }

    /// \brief Provides direct access to the underlying object.
    ///
    /// Undefined if *this is uninitialized. Overloaded on &.
    /// \returns l-value reference to the held object.
    reference_type operator*() & { return storage_.ref(); }

    /// \brief Provides direct access to the underlying object.
    ///
    /// Undefined if *this is uninitialized. Overloaded on &&.
    /// \returns r-value reference to the underlying object
    rval_reference_type operator*() && { return std::move(storage_.ref()); }

    /// \brief Direct access to the underlying object, or throw exception.
    ///
    /// Throws Bad_optional_access if *this is uninitialized. Overloaded on
    /// const &.
    /// \returns const l-value reference to the underlying object.
    const_reference_type value() const & {
        if (initialized_) {
            return storage_.ref();
        }
        throw Bad_optional_access("Optional is emtpy.");
    }

    /// \brief Direct access to the underlying object, or throw exception.
    ///
    /// Throws Bad_optional_access if *this is uninitialized. Overloaded on
    /// &.
    /// \returns l-value reference to the underlying object.
    reference_type value() & {
        if (initialized_) {
            return storage_.ref();
        }
        throw Bad_optional_access("Optional is emtpy.");
    }

    /// \brief Direct access to the underlying object, or throw exception.
    ///
    /// Throws Bad_optional_access if *this is uninitialized. Overloaded on
    /// &&.
    /// \returns r-value reference to the underlying object.
    rval_reference_type value() && {
        if (initialized_) {
            return std::move(storage_.ref());
        }
        throw Bad_optional_access("Optional is emtpy.");
    }

    /// \brief Direct access to the underlying object, or return \p val.
    ///	Returns \p val if *this is uninitialized, otherwise returns a reference
    /// to the value stored in *this. Overloaded on const &.
    /// \param	val Value to be returned if *this is uninitialized.
    /// \returns Either the value stored in *this, or \p val.
    template <typename U>
    value_type value_or(U&& val) const & {
        if (initialized_) {
            return storage_.ref();
        }
        return val;
    }

    /// \brief Direct access to the underlying object, or return \p val.
    ///	Returns \p val if *this is uninitialized, otherwise returns an r-value
    /// reference to the value stored in *this. Overloaded on &&.
    /// \param	val Value to be returned if *this is uninitialized.
    /// \returns Either the value stored in *this, or \p val.
    template <typename U>
    value_type value_or(U&& val) && {
        if (initialized_) {
            initialized_ = false;
            return std::move(storage_.ref());
        }
        return val;
    }

    /// \brief Direct access to the underlying object, or evaluate \p func and
    /// return its value.
    ///
    /// If *this is uninitialized, \p func is evaluated and it's value returned.
    /// Otherwise returns a reference to the underlying object in *this.
    /// Overloaded on const &.
    ///	\param	func    Function with signature T func().
    ///	\returns The value stored in *this, or the result of \p func().
    template <typename F>
    value_type value_or_eval(F f) const & {
        if (initialized_) {
            return storage_.ref();
        }
        return f();
    }

    /// \brief Direct access to the underlying object, or evaluate \p func and
    /// return its value.
    ///
    /// If *this is uninitialized, \p func is evaluated and it's value returned.
    /// Otherwise returns an r-value reference to the underlying object in
    /// *this. Overloaded on &&.
    ///	\param	func    Function with signature T func().
    ///	\returns The value stored in *this, or the result of \p func().
    template <typename F>
    value_type value_or_eval(F f) && {
        if (initialized_) {
            initialized_ = false;
            return std::move(storage_.ref());
        }
        return f();
    }

    /// \brief Access to the underlying object's pointer.
    ///
    /// *this still owns the object, do not delete the object via this pointer.
    /// \returns const pointer to the underlying object.
    pointer_const_type get_ptr() const { return storage_.ptr_ref(); }

    /// \brief Access to the underlying object's pointer.
    ///
    /// *this still owns the object, do not delete the object via this pointer.
    /// \returns Pointer to the underlying object.
    pointer_type get_ptr() { return storage_.ptr_ref(); }

    /// \brief Safe conversion to bool.
    /// \returns True if object contains a value, false otherwise.
    explicit operator bool() const noexcept { return this->is_initialized(); }

    /// \brief Convinience function
    ///
    /// Explicit operator bool makes ! operation verbose with !bool(opt).
    /// \returns Opposite of operator bool.
    bool operator!() const noexcept { return !this->is_initialized(); }

    bool is_initialized() const { return initialized_; }

    template <typename U>
    friend class Optional;

   private:
    bool initialized_ = false;
    detail::Aligned_storage<T> storage_;

    void construct(const value_type& value) {
        ::new (storage_.address()) value_type(value);
        initialized_ = true;
    }
    void construct(rval_reference_type value) {
        ::new (storage_.address()) value_type(std::move(value));
        initialized_ = true;
    }
    template <typename... Args>
    void emplace_construct(Args&&... args) {
        ::new (storage_.address()) value_type(std::forward<Args>(args)...);
        initialized_ = true;
    }
    void destroy() {
        if (initialized_) {
            storage_.ptr_ref()->~T();
            initialized_ = false;
        }
    }
};

/// Optional<void> specialization.
template <>
class Optional<void> {
   public:
    bool is_initialized() const { return false; }
    explicit operator bool() const noexcept { return this->is_initialized(); }
    bool operator!() const noexcept { return !this->is_initialized(); }
};

/// T must have operator== defined.
/// \returns If both x and y are initialized, (*x == *y).
/// \returns If only x _or_ y is initialized, false.
/// \returns If both are uninitialized, true.
template <typename T>
bool operator==(const Optional<T>& x, const Optional<T>& y) {
    if (x && y) {
        return *x == *y;
    }
    return !x && !y;
}

/// \returns !(x == y).
template <typename T>
bool operator!=(const Optional<T>& x, const Optional<T>& y) {
    return !(x == y);
}

/// T must have operator< defined.
/// \returns If both are initialized, *x < *y.
/// \returns If y is empty, false.
/// \returns If x and y are both empty, true.
template <typename T>
bool operator<(const Optional<T>& x, const Optional<T>& y) {
    if (!y) {
        return false;
    }
    if (!x) {
        return true;
    }
    return *x < *y;
}

/// \returns y < x
template <typename T>
bool operator>(const Optional<T>& x, const Optional<T>& y) {
    return (y < x);
}

/// \returns !(y < x)
template <typename T>
bool operator<=(const Optional<T>& x, const Optional<T>& y) {
    return !(y < x);
}

/// \returns !(x < y)
template <typename T>
bool operator>=(const Optional<T>& x, const Optional<T>& y) {
    return !(x < y);
}

/// \returns !x
template <typename T>
bool operator==(const Optional<T>& x, None_t) noexcept {
    return !x;
}

/// \returns !x
template <typename T>
bool operator==(None_t, const Optional<T>& x) noexcept {
    return !x;
}

/// \returns True if x is initialized, false if x is empty.
template <typename T>
bool operator!=(const Optional<T>& x, None_t) noexcept {
    return bool(x);
}

/// \returns True if x is initialized, false if x is empty.
template <typename T>
bool operator!=(None_t, const Optional<T>& x) noexcept {
    return bool(x);
}

/// \param opt  An Optional to extract the value from.
/// \returns Reference to the underlying object
template <typename T>
const T& get(const Optional<T>& opt) {
    return opt.get();
}

/// \param opt  An Optional to extract the value from.
/// \returns Reference to the underlying object
template <typename T>
T& get(Optional<T>& opt) {
    return opt.get();
}

/// \param opt  A pointer to Optional to extract the value from.
/// \returns Pointer to the underlying object
template <typename T>
const T* get(const Optional<T>* opt) {
    return opt->get_ptr();
}

/// \param opt  A pointer to Optional to extract the value from.
/// \returns Pointer to the underlying object
template <typename T>
T* get(Optional<T>* opt) {
    return opt->get_ptr();
}

/// \param opt  Optional object to get underlying object's pointer from.
/// \returns Pointer to the underlying object.
template <typename T>
auto get_pointer(const Optional<T>& opt) ->
    typename Optional<T>::pointer_const_type {
    return opt.get_ptr();
}

/// \param opt  Optional object to get underlying object's pointer from.
/// \returns Pointer to the underlying object.
template <typename T>
auto get_pointer(Optional<T>& opt) -> typename Optional<T>::pointer_type {
    return opt.get_ptr();
}

}  // namespace opt

#endif  // OPTIONAL_HPP
