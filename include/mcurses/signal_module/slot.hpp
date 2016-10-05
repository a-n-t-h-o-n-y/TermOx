#ifndef SLOT_HPP
#define SLOT_HPP

#include "slot_base.hpp"
#include "expired_slot.hpp"
// #include "signal.hpp" // delete this
#include "signal_fwd.hpp"

#include "slot_fwd.hpp"

#include <functional>
#include <memory>
#include <tuple>
#include <vector>

// Forward Declarations


namespace mcurses
{

// slot_fwd.hpp has original template declaration for slot.
template <typename Ret, typename ... Args, typename FunctionType>
class slot<Ret (Args...), FunctionType>: public slot_base {
public:
	typedef Ret 				result_type;
	typedef std::tuple<Args...> argument_types;
	typedef Ret(signature_type)(Args...) ;
	typedef FunctionType 		slot_function_type;

	static const int arity;

	template <unsigned n>
	class arg {
	public:
		typedef typename std::tuple_element<n, argument_types>::type type;
	};

	slot() = default;

	slot(const slot&) = default;

	template <typename Function>
	slot(const Function& func);

	template <typename T, typename U, typename V, typename W, typename X, typename Y>
	slot(const signal<T, U, V, W, X, Y>& sig);

	result_type operator()(Args&&... args);	// how is this diff? do you need non-const version?
	result_type operator()(Args&&... args) const;

	slot& track(const std::weak_ptr<void>& obj_ptr);
	template <typename T, typename U, typename V, typename W, typename X, typename Y>
	slot& track(const signal<T, U, V, W, X, Y>& sig);
	slot& track(const slot_base& slt);
	
	slot_function_type& slot_function();
	const slot_function_type& slot_function() const;

private:
	FunctionType function_;
};

template <typename Ret, typename ... Args, typename FunctionType>
const int slot<Ret (Args...), FunctionType>::arity = std::tuple_size<argument_types>::value;

template <typename Ret, typename ... Args, typename FunctionType>
template <typename Function>
slot<Ret (Args...), FunctionType>::slot(const Function& func)
:function_{func}
{}

template <typename Ret, typename ... Args, typename FunctionType>
template <typename T, typename U, typename V, typename W, typename X, typename Y>
slot<Ret (Args...), FunctionType>::slot(const signal<T, U, V, W, X, Y>& sig)
:function_{*(sig.lock_impl())}
{
	track(sig);
}

template <typename Ret, typename ... Args, typename FunctionType>
typename slot<Ret (Args...), FunctionType>::result_type 
slot<Ret (Args...), FunctionType>::operator()(Args&&... args)
{
	// make sure to first check if expired, then lock all object tracked
	auto l = expired() ? throw expired_slot() : lock();	// might be able to not assign to l if it will last until the brackets close
	return function_(std::forward<Args>(args)...);
}

template <typename Ret, typename ... Args, typename FunctionType>
typename slot<Ret (Args...), FunctionType>::result_type 
slot<Ret (Args...), FunctionType>::operator()(Args&&... args) const
{
	// make sure to first check if expired, then lock all object tracked
	auto l = expired() ? throw expired_slot() : lock();	// might be able to not assign to l if it will last until the brackets close
	return function_(std::forward<Args>(args)...);
}

template <typename Ret, typename ... Args, typename FunctionType>
slot<Ret (Args...), FunctionType>&
slot<Ret (Args...), FunctionType>::track(const std::weak_ptr<void>& obj_ptr)
{
	tracked_ptrs_.push_back(obj_ptr);
	return *this;
}

template <typename Ret, typename ... Args, typename FunctionType>
template <typename T, typename U, typename V, typename W, typename X, typename Y>
slot<Ret (Args...), FunctionType>&
slot<Ret (Args...), FunctionType>::track(const signal<T, U, V, W, X, Y>& sig)
{
	tracked_ptrs_.push_back(sig.lock_impl_as_void());
	return *this;
}

template <typename Ret, typename ... Args, typename FunctionType>
slot<Ret (Args...), FunctionType>&
slot<Ret (Args...), FunctionType>::track(const slot_base& slt)
{
	// only transfer objects being tracked by slt
	tracked_ptrs_.insert(std::begin(tracked_ptrs_), std::begin(slt.get_tracked_container()), std::end(slt.get_tracked_container()));
	return *this;
}

template <typename Ret, typename ... Args, typename FunctionType>
typename slot<Ret (Args...), FunctionType>::slot_function_type& slot<Ret (Args...), FunctionType>::slot_function()
{
	return function_;
}

template <typename Ret, typename ... Args, typename FunctionType>
const typename slot<Ret (Args...), FunctionType>::slot_function_type& slot<Ret (Args...), FunctionType>::slot_function() const
{
	return function_;
}

}	// namespace mcurses

#endif	// SLOT_HPP