#ifndef SHARED_CONNECTION_BLOCK_HPP
#define SHARED_CONNECTION_BLOCK_HPP

#include "connection.hpp"
class connection_impl_base;

#include <memory>

namespace mcurses
{

class shared_connection_block {
public:
	shared_connection_block(const connection& conn = mcurses::connection{}, bool initially_blocking = true);
	shared_connection_block(const shared_connection_block& x);
	shared_connection_block& operator=(const shared_connection_block& x);
	~shared_connection_block();

	void unblock();
	void block();
	bool blocking() const;

	connection connection() const;

private:
	void reset(const shared_connection_block& x);

	std::weak_ptr<connection_impl_base> weak_conn_impl_base_;
	bool blocking_;
};

} // namespace mcurses

#endif // SHARED_CONNECTION_BLOCK_HPP