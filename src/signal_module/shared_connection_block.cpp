#include <mcurses/signal_module/shared_connection_block.hpp>
#include <mcurses/signal_module/connection.hpp>

namespace mcurses
{

shared_connection_block::shared_connection_block(const mcurses::connection& conn, bool initially_blocking)
:weak_conn_impl_base_{conn.pimpl_}, blocking_{initially_blocking}
{
	if(!weak_conn_impl_base_.expired() && blocking_)
	{
		weak_conn_impl_base_.lock()->add_block();
	}
}

shared_connection_block::shared_connection_block(const shared_connection_block& x)
:weak_conn_impl_base_{x.weak_conn_impl_base_}, blocking_{x.blocking_}
{
	if(!weak_conn_impl_base_.expired() && blocking_)
	{
		weak_conn_impl_base_.lock()->add_block();
	}
}

shared_connection_block&
shared_connection_block::operator=(const shared_connection_block& x)
{
	if(this == &x)
	{
		return *this;
	}

	this->reset(x);

	return *this;
}

void
shared_connection_block::reset(const shared_connection_block& x)
{
	unblock();
	weak_conn_impl_base_ = x.weak_conn_impl_base_;
	blocking_ = x.blocking_;
	if(!weak_conn_impl_base_.expired() && blocking_)
	{
		weak_conn_impl_base_.lock()->add_block();
	}
	return;
}

shared_connection_block::~shared_connection_block()
{
	unblock();
}

void
shared_connection_block::unblock()
{
	if(!weak_conn_impl_base_.expired() && blocking_)
	{
		weak_conn_impl_base_.lock()->remove_block();
		blocking_ = false;
	}
	return;
}

void
shared_connection_block::block()
{
	if(!weak_conn_impl_base_.expired() && !blocking_)
	{
		weak_conn_impl_base_.lock()->add_block();
		blocking_ = true;
	}
	return;
}

bool
shared_connection_block::blocking() const
{
	if(!weak_conn_impl_base_.expired())
	{
		return blocking_;
	}
	return false;
}


connection
shared_connection_block::connection() const
{
	return mcurses::connection(weak_conn_impl_base_);
}

} // namespace mcurses