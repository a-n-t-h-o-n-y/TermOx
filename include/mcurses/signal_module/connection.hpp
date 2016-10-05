#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>
#include "detail/connection_impl_base.hpp"

namespace mcurses
{

class connection {
public:
	connection() = default;
	connection(std::weak_ptr<connection_impl_base> wp_cib);	// for signal constructor, undocumented
	
	connection(const connection& x);
	connection(connection&& x);

	connection& operator=(const connection& x);
	connection& operator=(connection&& x);

	void disconnect() const;

	bool connected() const;
	bool blocked() const;

	void swap(connection& x);

	bool operator==(const connection& x);
	bool operator!=(const connection& x);
	bool operator<(const connection& x);

	friend class shared_connection_block;

private:
	std::weak_ptr<connection_impl_base> pimpl_;
};

void swap(connection& x, connection& y);

} // namespace mcurses

#endif // CONNECTION_HPP