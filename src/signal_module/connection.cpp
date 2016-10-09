#include <mcurses/signal_module/connection.hpp>
#include <mcurses/signal_module/detail/connection_impl_base.hpp>

#include <memory>

namespace mcurses
{

connection::connection(std::weak_ptr<connection_impl_base> wp_cib)
:pimpl_{wp_cib}
{}

connection::connection(const connection& x)	// don't need; =default
:pimpl_{x.pimpl_}
{}

connection::connection(connection&& x)	// don't need; =default
:pimpl_{std::move(x.pimpl_)}
{}

connection& connection::operator=(const connection& x)	// prob don't need; =default;
{
	if(this == &x)
	{
		return *this;
	}

	pimpl_ = x.pimpl_;

	return *this;
}

connection& connection::operator=(connection&& x)	// prob don't need; =default;
{
	if(this == &x)
	{
		return *this;
	}

	pimpl_ = std::move(x.pimpl_);

	return *this;
}


void connection::disconnect() const
{
	if(pimpl_.expired())
	{
		return;
	}
	else
	{
		pimpl_.lock()->disconnect();
	}
	return;
}

bool connection::connected() const
{
	if(pimpl_.expired())
	{
		return false;
	}
	else
	{
		return pimpl_.lock()->connected();
	}
}

bool connection::blocked() const
{
	if(pimpl_.expired())
	{
		return false;
	}
	else
	{
		return pimpl_.lock()->blocked();
	}
}

void connection::swap(connection& x)	// might be able to =default this
{
	if(this == &x)
	{
		return;
	}
	using std::swap;
	swap(pimpl_, x.pimpl_);
	return;
}

bool connection::operator==(const connection& x)
{
	return pimpl_.lock() == x.pimpl_.lock();
}

bool connection::operator!=(const connection& x)
{
	return !(*this == x);
}

bool connection::operator<(const connection& x)
{
	return pimpl_.lock() < x.pimpl_.lock();
}

void swap(connection& x, connection& y)
{
	x.swap(y);
	return;
}

} // namespace mcurses