///	\file optional.hpp
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include "none_t.hpp"
#include "bad_optional_access.hpp"
#include <memory>

namespace mcurses
{

/**	\class optional
 *	\brief Used when a variable might not have a value.
 *
 *	This template will wrap the variable and allow users
 *	to test whether or not the variable contains a value.
 *	Useful when 0, -1, etc... does not suffice for 'no value'.
 */
template <typename T>
class optional {
public:
	typedef T 			value_type;
	typedef T&			reference_type;
	typedef const T&	reference_const_type;
	typedef T&&			rval_reference_type;
	typedef T*			pointer_type;
	typedef const T*	pointer_const_type;

	optional() noexcept;
	optional(none_t) noexcept;
	optional(const T& value);
	optional(T&& value);
	optional(bool condition, const T& value);

	optional(const optional& rhs);
	optional(optional&& rhs) noexcept;

	template <typename U>
	explicit optional(const optional<U>& rhs);

	template <typename U>
	explicit optional(optional<U>&& rhs);

	optional& operator=(none_t) noexcept;
	optional& operator=(const T& value);
	optional& operator=(T&& value);

	optional& operator=(const optional& rhs);
	optional& operator=(optional&& rhs) noexcept;

	template <typename U>
	optional& operator=(const optional<U>& rhs);

	template <typename U>
	optional& operator=(optional<U>&& rhs);

	template <typename ... Args>
	void emplace(Args&&... args);

	const T& get() const;
	T& get();

	const T* operator->() const;
	T* operator->();

	const T& operator*() const&;
	T& operator*() &;
	T&& operator*() &&;

	const T& value() const&;
	T& value() &;
	T&& value() &&;

	template <typename U>
	T value_or(U&& value) const&;

	template <typename U>
	T value_or(U&& value) &&;

	template <typename F>
	T value_or_eval(F func) const&;

	template <typename F>
	T value_or_eval(F func) &&;

	const T* get_ptr() const;
	T* get_ptr();

	explicit operator bool() const noexcept;
	bool operator!() const noexcept;

	template <typename U>
	friend class optional;

private:
	bool initialized_;
	std::unique_ptr<T> value_ptr_;	// create a copy of the internal object in a new unique_ptr for a copy constructor
};

template <typename T>
optional<T>::optional() noexcept
:initialized_{false}, value_ptr_{nullptr}
{}

template <typename T>
optional<T>::optional(none_t) noexcept
:initialized_{false}, value_ptr_{nullptr}
{}

template <typename T>
optional<T>::optional(const T& value)
:initialized_{true}, value_ptr_{std::make_unique<T>(value)}
{}

template <typename T>
optional<T>::optional(T&& value)
:initialized_{true}, value_ptr_{std::make_unique<T>(std::forward<T>(value))}
{}

template <typename T>
optional<T>::optional(bool condition, const T& value)
:initialized_{condition}, value_ptr_(condition ? std::make_unique<T>(value) : nullptr)
{}

template <typename T>
optional<T>::optional(const optional& rhs)
:initialized_{bool(rhs)}, value_ptr_{bool(rhs) ? std::make_unique<T>(*rhs) : nullptr}
{}

template <typename T>
optional<T>::optional(optional&& rhs) noexcept
:initialized_{bool(rhs)}, value_ptr_{bool(rhs) ? rhs.value_ptr_.release() : nullptr}
{
	rhs.initialized_ = false;
}

template <typename T>
template <typename U>
optional<T>::optional(const optional<U>& rhs)
:initialized_{bool(rhs)}, value_ptr_{bool(rhs) ? std::make_unique<T>(*rhs) : nullptr}
{}

template <typename T>
template <typename U>
optional<T>::optional(optional<U>&& rhs)
:initialized_{bool(rhs)}, value_ptr_{bool(rhs) ? std::make_unique<T>(std::move(rhs.get())) : nullptr}
{
	rhs.value_ptr_.release();
	rhs.initialized_ = false;
}

template <typename T>
optional<T>& optional<T>::operator=(none_t) noexcept
{
	initialized_ = false;
	value_ptr_.release();
	return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(const T& value)
{
	initialized_ = true;
	value_ptr_ = std::make_unique<T>(value);
	return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(T&& value)
{
	initialized_ = true;
	value_ptr_ = std::make_unique<T>(std::forward<T>(value));
	return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(const optional& rhs)
{
	initialized_ = bool(rhs);
	value_ptr_ = bool(rhs) ? std::make_unique<T>(*rhs) : nullptr;
	return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(optional&& rhs) noexcept
{
	initialized_ = bool(rhs);
	rhs.initialized_ = false;
	value_ptr_ = std::move(rhs.value_ptr_);
	return *this;
}

template <typename T>
template <typename U>
optional<T>& optional<T>::operator=(const optional<U>& rhs)
{
	initialized_ = bool(rhs);
	value_ptr_ = bool(rhs) ? std::make_unique<T>(*rhs) : nullptr;
	return *this;
}

template <typename T>
template <typename U>
optional<T>& optional<T>::operator=(optional<U>&& rhs)
{
	value_ptr_ = bool(rhs) ? std::make_unique<T>(std::move(rhs.get())) : nullptr;
	rhs.value_ptr_.release();
	initialized_ = bool(rhs);
	rhs.initialized_ = false;
	return *this;
}

template <typename T>
template <typename ... Args>
void optional<T>::emplace(Args&&... args)
{
	initialized_ = true;
	value_ptr_ = std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
const T& optional<T>::get() const
{
	return *value_ptr_;
}

template <typename T>
T& optional<T>::get()
{
	return *value_ptr_;
}

template <typename T>
const T* optional<T>::operator->() const
{
	return value_ptr_.get();
}

template <typename T>
T* optional<T>::operator->()
{
	return value_ptr_.get();
}

template <typename T>
const T& optional<T>::operator*() const&
{
	return *value_ptr_;
}

template <typename T>
T& optional<T>::operator*() &
{
	return *value_ptr_;
}

template <typename T>
T&& optional<T>::operator*() &&
{
	return std::move(*(value_ptr_.release()));
}

template <typename T>
const T& optional<T>::value() const&
{
	if(initialized_)
	{
		return *value_ptr_;
	}
	throw bad_optional_access("UnInitialized");
}

template <typename T>
T& optional<T>::value() &
{
	if(initialized_)
	{
		return *value_ptr_;
	}
	throw bad_optional_access("UnInitialized");
}

template <typename T>
T&& optional<T>::value() &&
{
	if(initialized_)
	{
		initialized_ = false;
		return std::move(*(value_ptr_.release()));
	}
	throw bad_optional_access("UnInitialized");
}

template <typename T>
template <typename U>
T optional<T>::value_or(U&& value) const&
{
	if(initialized_)
	{
		return *value_ptr_;
	}
	return std::forward<U>(value);
}

template <typename T>
template <typename U>
T optional<T>::value_or(U&& value) &&
{
	if(initialized_)
	{
		initialized_ = false;
		return std::move(*(value_ptr_.release()));
	}
	return std::forward<U>(value);
}

template <typename T>
template <typename F>
T optional<T>::value_or_eval(F func) const&
{
	if(initialized_)
	{
		return *value_ptr_;
	}
	return std::forward<T>(func());
}

template <typename T>
template <typename F>
T optional<T>::value_or_eval(F func) &&
{
	if(initialized_)
	{
		initialized_ = false;
		return std::move(*(value_ptr_.release()));
	}
	return func();
}

template <typename T>
const T* optional<T>::get_ptr() const
{
	return value_ptr_.get();
}

template <typename T>
T* optional<T>::get_ptr()
{
	return value_ptr_.get();
}

template <typename T>
optional<T>::operator bool() const noexcept
{
	return initialized_;
}

template <typename T>
bool optional<T>::operator!() const noexcept
{
	return !initialized_;
}

}	// namespace mcurses

#endif	// OPTIONAL_HPP