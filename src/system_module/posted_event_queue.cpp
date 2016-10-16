#include <mcurses/system_module/detail/posted_event_queue.hpp>
#include <mcurses/system_module/detail/posted_event.hpp>
#include <mcurses/system_module/event.hpp>

#include <algorithm>
#include <functional>
#include <utility>

namespace mcurses {
namespace detail {

// sorts by priority when adding Posted_event
void Posted_event_queue::add_event(Posted_event& pe)
{
	int priority = pe.priority();
	if(this->empty() || this->back().priority() >= priority || (pe.event().type() == Event::Type::DeferredDelete))
	{
		this->emplace_back(std::move(pe));
	}
	else
	{
		auto gt = [](const Posted_event& e, const int& i){return e > i;};
		auto at = std::lower_bound(this->begin(), this->end(), priority, gt);
		this->emplace(at, std::move(pe));
	}
	return;
}

Posted_event Posted_event_queue::next_posted_event()
{
	auto temp = std::move(this->front());
	this->pop_front();
	return temp;
}



} // namespace detail
} // namespace mcurses