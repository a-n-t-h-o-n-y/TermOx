#ifndef SLOT_ITERATOR_HPP
#define SLOT_ITERATOR_HPP

#include <iterator>

namespace mcurses
{

template <typename InputIterator>
class slot_iterator {
public:
	typedef typename std::iterator_traits<InputIterator>::value_type 	iter_result_type;
	typedef typename iter_result_type::result_type 						result_type;

	slot_iterator() = default;	// might never be used?

	slot_iterator(InputIterator iter)
	:iter_{iter}{}

	result_type operator*()
	{
		auto slt = *iter_;
		return slt();
	}

	slot_iterator& operator++()
	{
		++iter_;
		return *this;
	}

	bool operator==(const slot_iterator& x)
	{
		return iter_ == x.iter_;
	}

	bool operator!=(const slot_iterator& x)
	{
		return !operator==(x);
	}


private:
	InputIterator iter_;
};

} // namespace mcurses

#endif // SLOT_ITERATOR_HPP