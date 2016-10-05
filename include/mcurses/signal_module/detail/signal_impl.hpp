#ifndef SIGNAL_IMPL_HPP
#define SIGNAL_IMPL_HPP

#include "../position.hpp"
#include "../connection.hpp"
#include "connection_impl.hpp"
#include "slot_iterator.hpp"

#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <functional>

// Forward Declarations
namespace mcurses
{
	template <typename Signature>
	class connection_impl;
}

namespace mcurses
{

template <typename Signature,
		  typename Combiner,
		  typename Group,
		  typename GroupCompare,
		  typename SlotFunction>
class signal_impl;

template <typename Ret, typename ... Args,
		  typename Combiner,
		  typename Group,
		  typename GroupCompare,
		  typename SlotFunction>
class signal_impl<Ret(Args...), Combiner, Group, GroupCompare, SlotFunction> {
public:
	// Types
	typedef typename Combiner::result_type 	result_type;
	typedef Ret(signature_type)(Args...) ;
	typedef Group group_type;
	typedef GroupCompare group_compare_type;
	typedef std::vector<std::shared_ptr<connection_impl<signature_type>>> positioned_connection_container_type;
	typedef std::map<group_type, positioned_connection_container_type, group_compare_type> grouped_connection_container_type;
	typedef Combiner combiner_type;
	typedef SlotFunction slot_function_type;
	typedef mcurses::slot<signature_type, slot_function_type> slot_type;
	typedef mcurses::slot<Ret(const mcurses::connection&, Args...)> extended_slot_type;


	signal_impl(const combiner_type& combiner, const group_compare_type& group_compare)
	:combiner_{combiner}
	{}

	signal_impl(const signal_impl&) = default;
	signal_impl(signal_impl&& x) = default;
	signal_impl& operator=(const signal_impl&) = default;
	signal_impl& operator=(signal_impl&& x) = default;

	connection connect(const slot_type& s, position pos)
	{
		auto c = std::make_shared<connection_impl<signature_type>>(s);
		if(pos == position::at_front)
		{
			at_front_connections_.insert(std::begin(at_front_connections_), c);
			// at_front_connections_.push_back(c);
			return connection(c);
		}

		if(pos == position::at_back)
		{
			at_back_connections_.push_back(c);
			return connection(c);
		}
		return connection();
	}

	connection connect(const group_type& g, const slot_type& s, position pos)
	{
		auto c = std::make_shared<connection_impl<signature_type>>(s);
		if(pos == position::at_front)
		{
			grouped_connections_[g].insert(std::begin(grouped_connections_[g]), c);
			return connection(c);
		}
		if(pos == position::at_back)
		{
			grouped_connections_[g].push_back(c);
			return connection(c);
		}
		return connection();
	}

	connection connect_extended(const extended_slot_type& es, position pos)
	{
		auto conn_impl = std::make_shared<connection_impl<signature_type>>();
		connection conn = connection(conn_impl);
		conn_impl->emplace_extended(es, conn);	// this only takes the slot function, not the tracked items..!

		if(pos == position::at_front)
		{
			at_front_connections_.insert(std::begin(at_front_connections_), conn_impl);
			return conn;
		}

		if(pos == position::at_back)
		{
			at_back_connections_.push_back(conn_impl);
			return conn;
		}
		return connection();
	}

	connection connect_extended(const group_type& g, const extended_slot_type& es, position pos)
	{
		auto conn_impl = std::make_shared<connection_impl<signature_type>>();
		connection conn = connection(conn_impl);
		conn_impl->emplace_extended(es, conn);

		if(pos == position::at_front)
		{
			grouped_connections_[g].insert(std::begin(grouped_connections_[g]), conn_impl);
			return conn;
		}
		if(pos == position::at_back)
		{
			grouped_connections_[g].push_back(conn_impl);
			return conn;
		}
		return connection();
	}

	void disconnect(const group_type& g)
	{
		for(auto& ci_ptr : grouped_connections_[g])
		{
			ci_ptr->disconnect();
		}
		// It should be safe to destroy the vector at this key; g.
	}

	void disconnect_all_slots()
	{
		for(auto& ci_ptr : at_front_connections_)
		{
			ci_ptr->disconnect();
		}

		for(auto& kv : grouped_connections_)
		{			// vector
			for(auto& ci_ptr : kv.second)
			{
				ci_ptr->disconnect();
			}
		}

		for(auto& ci_ptr : at_back_connections_)
		{
			ci_ptr->disconnect();
		}
	}

	bool empty() const
	{
		for(auto& ci_ptr : at_front_connections_)
		{
			if(ci_ptr->connected())
			{
				return false;
			}
		}

		for(auto& kv : grouped_connections_)
		{			// vector
			for(auto& ci_ptr : kv.second)
			{
				if(ci_ptr->connected())
				{
					return false;
				}
			}
		}

		for(auto& ci_ptr : at_back_connections_)
		{
			if(ci_ptr->connected())
			{
				return false;
			}
		}
		// no slots that are not disconnected
		return true;
	}

	std::size_t num_slots() const
	{
		std::size_t size{0};
		for(auto& ci_ptr : at_front_connections_)
		{
			if(ci_ptr->connected())
			{
				++size;
			}
		}

		for(auto& kv : grouped_connections_)
		{			// vector
			for(auto& ci_ptr : kv.second)
			{
				if(ci_ptr->connected())
				{
					++size;
				}
			}
		}

		for(auto& ci_ptr : at_back_connections_)
		{
			if(ci_ptr->connected())
			{
				++size;
			}
		}
		return size;
	}

	result_type operator()(Args&&... args)
	{
		typedef std::vector<std::function<Ret()>> container_type;
		container_type bound_slot_container;
		for(auto& c : at_front_connections_)
		{
			if(c->connected() && !c->blocked())
			{
				bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
			}
		}

		for(auto& kv : grouped_connections_)
		{
			for(auto& c : kv.second)
			{
				if(c->connected() && !c->blocked())
				{
					bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
				}
			}
		}

		for(auto& c : at_back_connections_)
		{
			if(c->connected() && !c->blocked())
			{
				bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
			}
		}
		return combiner_(slot_iterator<typename container_type::iterator>(std::begin(bound_slot_container)),
							slot_iterator<typename container_type::iterator>(std::end(bound_slot_container)));
	}

	result_type operator()(Args&&... args) const
	{
		typedef std::vector<std::function<Ret()>> container_type;
		container_type bound_slot_container;
		for(auto& c : at_front_connections_)
		{
			if(c->connected() && !c->blocked())
			{
				bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
			}
		}

		for(auto& kv : grouped_connections_)
		{
			for(auto& c : kv.second)
			{
				if(c->connected() && !c->blocked())
				{
					bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
				}
			}
		}

		for(auto& c : at_back_connections_)
		{
			if(c->connected() && !c->blocked())
			{
				bound_slot_container.push_back(std::bind(std::function<Ret(Args...)>{c->get_slot()}, std::forward<Args>(args)...));
			}
		}
		const combiner_type const_comb = combiner_;
		return const_comb(slot_iterator<typename container_type::iterator>(std::begin(bound_slot_container)),
							slot_iterator<typename container_type::iterator>(std::end(bound_slot_container)));
	}

	combiner_type combiner() const
	{
		return combiner_;
	}

	void set_combiner(const combiner_type& comb)
	{
		combiner_ = comb;
		return;
	}

private:
	positioned_connection_container_type at_front_connections_;
	grouped_connection_container_type grouped_connections_;
	positioned_connection_container_type at_back_connections_;

	combiner_type combiner_;
};

} // namespace mcurses

#endif // SIGNAL_IMPL_HPP