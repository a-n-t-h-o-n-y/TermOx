#ifndef SIGNAL_VOID_RETURN_SPECIALIZATION_HPP
#define SIGNAL_VOID_RETURN_SPECIALIZATION_HPP

#include "../signal_fwd.hpp"

namespace mcurses
{

// signal class specialization for void return type
template <typename ... Args, typename Combiner, typename Group,
		  typename GroupCompare, typename SlotFunction, typename Mutex>

class signal<void(Args...), Combiner, Group, GroupCompare, SlotFunction, Mutex> {
public:
// Types
	typedef void (signature_type)(Args...);
	typedef typename Combiner::result_type 	result_type;
	typedef Combiner						combiner_type;
	typedef Group							group_type;
	typedef GroupCompare					group_compare_type;
	typedef SlotFunction					slot_function_type;
	typedef typename mcurses::slot<signature_type, SlotFunction> slot_type;
	typedef std::function<void(const connection&, Args...)>	extended_slot_function_type;
	typedef typename mcurses::slot<void(const mcurses::connection&, Args...), extended_slot_function_type> extended_slot_type;
	typedef typename SlotFunction::result_type slot_result_type;
	typedef int*	slot_call_iterator; // maybe? prob dont need this, when would a client need it?
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
	:pimpl_{std::move(x.pimpl_)}
	{}

	signal& operator=(const signal&) = delete;

	signal& operator=(signal&& x)
	{
		if(this == &x)
		{
			return *this;
		}
		pimpl_ = std::move(x.pimpl_);

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
		return connect_extended(g, es, pos);
	}

	void disconnect(const group_type& g)
	{
		pimpl_->disconnect(g);
		return;
	}

	template <typename S>
	void disconnect(const S& a_slot)
	{
		pimpl_->disconnect(a_slot);
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

	// Specialized
	result_type operator()(Args... args)
	{
		pimpl_->void_specialization_operator_call(std::forward<Args>(args)...);
		return;
	}

	// Specialized
	result_type operator()(Args... args) const
	{
		pimpl_->void_specialization_operator_call(std::forward<Args>(args)...);
		return;
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

	void swap(signal& x)
	{
		if(this == &x)
		{
			return;
		}
		using std::swap;
		swap(pimpl_, x.pimpl_);
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

private:
	std::shared_ptr<impl_type> pimpl_;
};

} // namespace mcurses

#endif // SIGNAL_VOID_RETURN_SPECIALIZATION_HPP