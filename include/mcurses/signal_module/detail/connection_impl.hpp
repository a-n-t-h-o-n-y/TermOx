#ifndef CONNECTION_IMPL_HPP
#define CONNECTION_IMPL_HPP

#include "connection_impl_base.hpp"
#include "../slot.hpp"

namespace mcurses
{

template <typename Signature>
class connection_impl;

template <typename Ret, typename ... Args>
class connection_impl<Ret(Args...)> : public connection_impl_base {
public:
	connection_impl();
	connection_impl(const slot<Ret(Args...)>& x);

	void disconnect() override;

	bool connected() const override;
	bool blocked() const override;

	slot<Ret(Args...)>& get_slot();
	const slot<Ret(Args...)>& get_slot() const;

private:
	slot<Ret(Args...)> slot_;
	bool connected_;
	bool blocked_;
};

template <typename Ret, typename ... Args>
connection_impl<Ret(Args...)>::connection_impl()
:slot_{}, connected_{false}, blocked_{false}
{}

template <typename Ret, typename ... Args>
connection_impl<Ret(Args...)>::connection_impl(const slot<Ret(Args...)>& x)
:slot_{x}, connected_{true}, blocked_{false}
{}

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
bool connection_impl<Ret(Args...)>::blocked() const
{
	return blocked_;
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

} // namespace mcurses

#endif // CONNECTION_IMPL_HPP