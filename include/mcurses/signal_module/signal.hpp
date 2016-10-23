#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "signal_fwd.hpp"
#include "slot_fwd.hpp"

#include "position.hpp"
#include "detail/signal_impl.hpp"
#include "connection.hpp"
#include "detail/connection_impl.hpp"
#include "detail/slot_iterator.hpp"

#include <functional>
#include <tuple>
#include <vector>
#include <memory>
#include <cstddef>
#include <utility>

namespace mcurses
{

// A specialization, signal_fwd.hpp has origianl declaration.
template <typename Ret,
		  typename ... Args,
		  typename Combiner,
		  typename Group,
		  typename GroupCompare,
		  typename SlotFunction,
		  typename Mutex>

class signal<Ret(Args...),
			 Combiner,
			 Group,
			 GroupCompare,
			 SlotFunction,
			 Mutex>
{
public:
	// Types
	typedef Ret (signature_type)(Args...);
	typedef typename Combiner::result_type 	result_type;
	typedef Combiner						combiner_type;
	typedef Group							group_type;
	typedef GroupCompare					group_compare_type;
	typedef SlotFunction					slot_function_type;
	typedef typename mcurses::slot<signature_type, SlotFunction> slot_type;
	typedef std::function<Ret(const connection&, Args...)>	extended_slot_function_type;
	typedef typename mcurses::slot<Ret(const mcurses::connection&, Args...), extended_slot_function_type> extended_slot_type;
	typedef typename SlotFunction::result_type slot_result_type;
	// typedef slot_iterator					slot_call_iterator;
	typedef std::tuple<Args...> 			argument_types;
	typedef signal_impl<signature_type, combiner_type, group_type,
						group_compare_type, slot_function_type> impl_type;

	static const int arity = std::tuple_size<argument_types>::value;
	
	template <unsigned n>
	class arg {
	public:
		typedef typename std::tuple_element<n, argument_types>::type type;
	};

	signal(const combiner_type& combiner = combiner_type(),
			const group_compare_type& group_compare = group_compare_type())
	:pimpl_{std::make_shared<impl_type>(combiner, group_compare)}
	{}

	signal(const signal&) = delete;

	signal(signal&& x)
	:pimpl_{std::move(x.pimpl_)}, enabled_{x.enabled_}
	{ x.enabled_ = false; }

	signal& operator=(const signal&) = delete;

	signal& operator=(signal&& x)
	{
		if(this == &x)
		{
			return *this;
		}
		pimpl_ = std::move(x.pimpl_);
		enabled_ = x.enabled_;
		x.enabled_ = false;

		return *this;
	}

	connection connect(const slot_type& s, position pos = position::at_back)
	{
		return pimpl_->connect(s, pos);
	}

	connection connect(const group_type& g, const slot_type& s, position pos = position::at_back)
	{
		return pimpl_->connect(g, s, pos);
	}

	connection connect_extended(const extended_slot_type& es, position pos = position::at_back)
	{
		return pimpl_->connect_extended(es, pos);
	}

	connection connect_extended(const group_type& g, const extended_slot_type& es, position pos = position::at_back)
	{
		return pimpl_->connect_extended(g, es, pos);
	}

	void disconnect(const group_type& g)
	{
		pimpl_->disconnect(g);
		return;
	}

	void disconnect_all_slots()
	{
		pimpl_->disconnect_all_slots();
		return;
	}

	bool empty() const
	{
		return pimpl_->empty();
	}

	std::size_t num_slots() const
	{
		return pimpl_->num_slots();
	}

	result_type operator()(Args&&... args)
	{
		if(enabled_) {
			return pimpl_->operator()(std::forward<Args>(args)...);
		} else {
			return result_type();
		}
	}

	result_type operator()(Args&&... args) const
	{
		if(enabled_) {
			return pimpl_->operator()(std::forward<Args>(args)...);
		} else {
			return result_type();
		}
	}

	combiner_type combiner() const
	{
		return pimpl_->combiner();
	}

	void set_combiner(const combiner_type& comb) // check if you are doing the forward correctly, and if you need to.
	{
		pimpl_->set_combiner(std::forward<const combiner_type&>(comb));
		return;
	}

	std::shared_ptr<void> lock_impl_as_void() const
	{
		return pimpl_;
	}

	std::shared_ptr<impl_type> lock_impl() const
	{
		return pimpl_;
	}

	friend void swap(signal& x, signal& y)
	{
		if(&x == &y)
		{
			return;
		}
		using std::swap;
		swap(x.pimpl_, y.pimpl_);
		swap(x.enabled_, y.enabled_);
		return;
	}

	bool enabled() const { return enabled_; }
	void enable() { enabled_ = true; }
	void disable() { enabled_ = false; }

private:
	std::shared_ptr<impl_type> pimpl_;
	bool enabled_ = true;
};

template <typename Ret, typename ... Args, typename Combiner, typename Group,
		  typename GroupCompare, typename SlotFunction, typename Mutex>
const int signal<Ret(Args...), Combiner, Group, GroupCompare, SlotFunction, Mutex>::arity;

} // namespace mcurses

// #include "detail/signal_void_return_specialization.hpp"

#endif // SIGNAL_HPP