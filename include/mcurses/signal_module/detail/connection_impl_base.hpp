#ifndef CONNECTION_IMPL_BASE_HPP
#define CONNECTION_IMPL_BASE_HPP

// Forward Declarations
namespace mcurses
{
	class connection;
}

namespace mcurses
{
	
// access to connections without template parameters
// interface for templated connection_implementation 
// this is used by connnecton class via a pointer.
class connection_impl_base {
public:
	virtual ~connection_impl_base() = default;

	virtual void disconnect() = 0;

	virtual bool connected() const = 0;
	inline bool blocked() const {return blocking_object_count_ < 1? false : true;}

	inline void add_block() {++blocking_object_count_; return;}
	inline void remove_block() {--blocking_object_count_; return;}

protected:
	int blocking_object_count_ = 0;
};

} // namespace mcurses

#endif // CONNECTION_IMPL_BASE_HPP