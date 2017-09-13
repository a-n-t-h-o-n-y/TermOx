#ifndef DETAIL_SIGNAL_IMPL_HPP
#define DETAIL_SIGNAL_IMPL_HPP

#include "../connection.hpp"
#include "../position.hpp"
#include "connection_impl.hpp"
#include "slot_iterator.hpp"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <deque>

namespace sig {

// Forward Declaration
template <typename Signature>
class Connection_impl;

template <typename Signature,
          typename Combiner,
          typename Group,
          typename GroupCompare,
          typename SlotFunction>
class Signal_impl;

template <typename Ret,
          typename... Args,
          typename Combiner,
          typename Group,
          typename GroupCompare,
          typename SlotFunction>
class Signal_impl<Ret(Args...), Combiner, Group, GroupCompare, SlotFunction> {
   public:
    // Types
    using result_type = typename Combiner::result_type;
    using signature_type = Ret(Args...);
    using group_type = Group;
    using group_compare_type = GroupCompare;
    using combiner_type = Combiner;
    using slot_function_type = SlotFunction;
    using slot_type = Slot<signature_type, slot_function_type>;
    using extended_slot_type = Slot<Ret(const Connection&, Args...)>;

    Signal_impl(combiner_type combiner, const group_compare_type& group_compare)
        : combiner_{std::move(combiner)} {}

    Connection connect(const slot_type& slot, Position position) {
        auto c_impl = std::make_shared<Connection_impl<signature_type>>(slot);
        position == Position::at_front ? front_connections_.push_front(c_impl)
                                       : back_connections_.push_back(c_impl);
        return Connection(c_impl);
    }

    Connection connect(const group_type& group,
                       const slot_type& slot,
                       Position position) {
        auto c_impl = std::make_shared<Connection_impl<signature_type>>(slot);
        position == Position::at_front
            ? grouped_connections_[group].push_front(c_impl)
            : grouped_connections_[group].push_back(c_impl);
        return Connection(c_impl);
    }

    Connection connect_extended(const extended_slot_type& ext_slot,
                                Position position) {
        auto c_impl = std::make_shared<Connection_impl<signature_type>>();
        auto c = Connection(c_impl);
        c_impl->emplace_extended(ext_slot, c);
        position == Position::at_front ? front_connections_.push_front(c_impl)
                                       : back_connections_.push_back(c_impl);
        return c;
    }

    Connection connect_extended(const group_type& group,
                                const extended_slot_type& ext_slot,
                                Position position) {
        auto c_impl = std::make_shared<Connection_impl<signature_type>>();
        auto c = Connection(c_impl);
        c_impl->emplace_extended(ext_slot, c);
        position == Position::at_front
            ? grouped_connections_[group].push_front(c_impl)
            : grouped_connections_[group].push_back(c_impl);
        return c;
    }

    void disconnect(const group_type& group) {
        for (auto& c_impl_ptr : grouped_connections_[group]) {
            c_impl_ptr->disconnect();
        }
        grouped_connections_.erase(group);
    }

    void disconnect_all_slots() {
        for (auto& c_impl_ptr : front_connections_) {
            c_impl_ptr->disconnect();
        }
        for (auto& gc_pair : grouped_connections_) {
            for (auto& c_impl_ptr : gc_pair.second) {
                c_impl_ptr->disconnect();
            }
        }
        for (auto& c_impl_ptr : back_connections_) {
            c_impl_ptr->disconnect();
        }
        front_connections_.clear();
        grouped_connections_.clear();
        back_connections_.clear();
    }

    bool empty() const {
        for (auto& c_impl_ptr : front_connections_) {
            if (c_impl_ptr->connected()) {
                return false;
            }
        }
        for (auto& gc_pair : grouped_connections_) {
            for (auto& c_impl_ptr : gc_pair.second) {
                if (c_impl_ptr->connected()) {
                    return false;
                }
            }
        }
        for (auto& c_impl_ptr : back_connections_) {
            if (c_impl_ptr->connected()) {
                return false;
            }
        }
        return true;
    }

    std::size_t num_slots() const {
        std::size_t size{0};
        for (auto& c_impl_ptr : front_connections_) {
            if (c_impl_ptr->connected()) {
                ++size;
            }
        }
        for (auto& gc_pair : grouped_connections_) {
            for (auto& c_impl_ptr : gc_pair.second) {
                if (c_impl_ptr->connected()) {
                    ++size;
                }
            }
        }
        for (auto& c_impl_ptr : back_connections_) {
            if (c_impl_ptr->connected()) {
                ++size;
            }
        }
        return size;
    }

    template <typename... Arguments>
    result_type operator()(Arguments&&... args) {
        auto slots = bind_args(std::forward<Arguments>(args)...);
        return combiner_(
            Slot_iterator<typename container_t::iterator>(std::begin(slots)),
            Slot_iterator<typename container_t::iterator>(std::end(slots)));
    }

    template <typename... Arguments>
    result_type operator()(Arguments&&... args) const {
        auto slots = bind_args(std::forward<Arguments>(args)...);
        const combiner_type const_comb = combiner_;
        return const_comb(
            Slot_iterator<typename container_t::iterator>(std::begin(slots)),
            Slot_iterator<typename container_t::iterator>(std::end(slots)));
    }

    combiner_type combiner() const { return combiner_; }

    void set_combiner(const combiner_type& comb) { combiner_ = comb; }

   private:
    using container_t = std::vector<std::function<Ret()>>;
    using position_container_t =
        std::deque<std::shared_ptr<Connection_impl<signature_type>>>;
    using group_container_t =
        std::map<group_type, position_container_t, group_compare_type>;

    // Prepares the functions to be processed by the combiner.
    // Returns a container of std::functions with signature Ret().
    template <typename... Arguments>
    container_t bind_args(Arguments&&... args) const {
        container_t bound_slot_container;
        for (auto& c_impl_ptr : front_connections_) {
            if (c_impl_ptr->connected() && !c_impl_ptr->blocked() &&
                !c_impl_ptr->get_slot().expired()) {
                bound_slot_container.push_back(std::bind(
                    std::function<Ret(Args...)>{c_impl_ptr->get_slot()},
                    std::forward<Arguments>(args)...));
            }
        }
        for (auto& gc_pair : grouped_connections_) {
            for (auto& c_impl_ptr : gc_pair.second) {
                if (c_impl_ptr->connected() && !c_impl_ptr->blocked() &&
                    !c_impl_ptr->get_slot().expired()) {
                    bound_slot_container.push_back(std::bind(
                        std::function<Ret(Args...)>{
                            c_impl_ptr->get_slot()},
                        std::forward<Arguments>(args)...));
                }
            }
        }
        for (auto& c_impl_ptr : back_connections_) {
            if (c_impl_ptr->connected() && !c_impl_ptr->blocked() &&
                !c_impl_ptr->get_slot().expired()) {
                bound_slot_container.push_back(std::bind(
                    std::function<Ret(Args...)>{c_impl_ptr->get_slot()},
                    std::forward<Arguments>(args)...));
            }
        }
        return bound_slot_container;
    }

    // Connections are stored here
    position_container_t front_connections_;
    group_container_t grouped_connections_;
    position_container_t back_connections_;

    combiner_type combiner_;
};

}  // namespace sig

#endif  // DETAIL_SIGNAL_IMPL_HPP
