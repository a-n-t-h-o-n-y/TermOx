#ifndef SLOT_ITERATOR_HPP
#define SLOT_ITERATOR_HPP

#include "../slot.hpp"

#include <functional>
#include <utility>

namespace mcurses
{

template <typename Signature>
class slot_iterator;

template <typename Ret, typename ... Args>
class slot_iterator<Ret(Args...)> {
public:
	slot_iterator() = default;

	slot_iterator(std::function<Ret(Args...)> s, Args&&... args)
	:func_{std::bind(s, std::forward<Args>(args)...)}
	{}

	Ret operator*()
	{
		if(!has_cache_)
		{
			cache_ = func_();
			has_cache_ = true;
		}
		return cache_;
	}

	slot_iterator& operator++()
	{
		return *(this + 1);	// iters stored in vector
	}

	bool operator==(const slot_iterator& x)
	{
		return this == &x;
	}

	bool operator!=(const slot_iterator& x)
	{
		return !(*this == x);
	}

private:
	std::function<Ret()> func_;
	bool has_cache_ = false;
	Ret cache_;
};


// VOID SPECIALIZATION - \ _

template <typename ... Args>
class slot_iterator<void(Args...)> {
public:
	slot_iterator() = default;

	slot_iterator(std::function<void(Args...)> s, Args&&... args)
	:func_{std::bind(s, std::forward<Args>(args)...)}
	{}

	void operator*()
	{
		if(!has_run_)
		{
			func_();
			has_run_ = true;
		}
		return;
	}

	slot_iterator& operator++()
	{
		return *(this + 1);	// iters stored in vector
	}

	bool operator==(const slot_iterator& x)
	{
		return this == &x;
	}

	bool operator!=(const slot_iterator& x)
	{
		return !(*this == x);
	}

private:
	std::function<void()> func_;
	bool has_run_ = false;
};

} // namespace mcurses

#endif // SLOT_ITERATOR_HPP