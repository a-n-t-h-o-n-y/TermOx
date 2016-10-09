#include <mcurses/signal_module/slot_base.hpp>

#include <memory>
#include <vector>

namespace mcurses
{

bool slot_base::expired() const
{
	for(auto& wp : tracked_ptrs_)
	{
		if(wp.expired())
		{
			return true;
		}
	}
	return false;
}

slot_base::locked_container_type slot_base::lock() const
{
	std::vector<std::shared_ptr<void>> locked_vec;
	for(auto& wp : tracked_ptrs_)
	{
		locked_vec.push_back(wp.lock());
	}
	return locked_vec;
}

const std::vector<std::weak_ptr<void>>& slot_base::get_tracked_container() const
{
	return tracked_ptrs_;
}

} // namespace mcurses