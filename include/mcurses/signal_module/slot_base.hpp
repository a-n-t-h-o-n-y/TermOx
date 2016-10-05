#ifndef SLOT_BASE_HPP
#define SLOT_BASE_HPP

#include <memory>
#include <vector>

namespace mcurses
{

class slot_base {
public:
	typedef std::vector<std::shared_ptr<void>> locked_container_type;

	slot_base() = default;

	slot_base(const slot_base&) = default;

	virtual ~slot_base() = default;

	bool expired() const;
	locked_container_type lock() const;
	const std::vector<std::weak_ptr<void>>& get_tracked_container() const;
	
protected:
	std::vector<std::weak_ptr<void>> tracked_ptrs_;
};

} // namespace mcurses

#endif // SLOT_BASE_HPP