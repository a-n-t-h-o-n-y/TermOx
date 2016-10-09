#ifndef CONNECTION_IMPL_HPP
#define CONNECTION_IMPL_HPP

#include "connection_impl_base.hpp"
#include "placeholder_template.hpp"
#include "../slot.hpp"

class connection;

#include <utility>
#include <functional>

namespace mcurses
{

template <typename Signature>
class connection_impl;

template <typename Ret, typename ... Args>
class connection_impl<Ret(Args...)> : public connection_impl_base {
public:
	typedef slot<Ret(const connection&, Args...)> extended_slot_type;

	connection_impl();
	connection_impl(const slot<Ret(Args...)>& x);

	// add a test
	connection_impl& emplace_extended(const extended_slot_type& es, const connection& c);

	void disconnect() override;

	bool connected() const override;

	slot<Ret(Args...)>& get_slot();
	const slot<Ret(Args...)>& get_slot() const;

private:
	std::function<Ret(Args...)>
	bind_connection(std::function<Ret(const connection&, Args...)> f, const connection& c);

	template <typename IntType, IntType ... Is>
	std::function<Ret(Args...)>
	bind_connection(std::function<Ret(const connection&, Args...)> f, const connection& c, std::integer_sequence<IntType, Is...>);

	slot<Ret(Args...)> slot_;
	bool connected_;
};

template <typename Ret, typename ... Args>
connection_impl<Ret(Args...)>::connection_impl()
:slot_{}, connected_{false}
{}

template <typename Ret, typename ... Args>
connection_impl<Ret(Args...)>::connection_impl(const slot<Ret(Args...)>& x)
:slot_{x}, connected_{true}
{}

template <typename Ret, typename ... Args>
connection_impl<Ret(Args...)>&
connection_impl<Ret(Args...)>::emplace_extended(const extended_slot_type& es, const connection& c)
{
	connected_ = true;
	slot_.slot_function() = bind_connection(es.slot_function(), c);
	for(const std::weak_ptr<void>& wp : es.get_tracked_container())	// copy over tracked items
	{
		slot_.track(wp);
	}
	return *this;
}

template <typename Ret, typename ... Args>
void connection_impl<Ret(Args...)>::disconnect()
{
	if(connected_)
	{
		connected_ = false;
	}
	return;
}

template <typename Ret, typename ... Args>
bool connection_impl<Ret(Args...)>::connected() const
{
	return connected_;
}

template <typename Ret, typename ... Args>
slot<Ret(Args...)>& connection_impl<Ret(Args...)>::get_slot()
{
	return slot_;
}

template <typename Ret, typename ... Args>
const slot<Ret(Args...)>& connection_impl<Ret(Args...)>::get_slot() const
{
	return slot_;
}

template <typename Ret, typename ... Args>
std::function<Ret(Args...)>
connection_impl<Ret(Args...)>::bind_connection(std::function<Ret(const connection&, Args...)> f, const connection& c)
{
	return bind_connection(std::forward<std::function<Ret(const connection&, Args...)>>(f), std::forward<const connection&>(c), std::index_sequence_for<Args...>{});
}

template <typename Ret, typename ... Args>
template <typename IntType, IntType ... Is>
std::function<Ret(Args...)>
connection_impl<Ret(Args...)>::bind_connection(std::function<Ret(const connection&, Args...)> f, const connection& c, std::integer_sequence<IntType, Is...>)
{
	return std::bind(std::forward<std::function<Ret(const connection&, Args...)>>(f), std::forward<const connection&>(c), placeholder_template<Is>{}...);
}


} // namespace mcurses

#endif // CONNECTION_IMPL_HPP