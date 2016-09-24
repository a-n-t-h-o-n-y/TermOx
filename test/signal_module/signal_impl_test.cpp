#include <mcurses/signal_module/detail/signal_impl.hpp>
#include <gtest/gtest.h>

#include <mcurses/signal_module/optional_last_value.hpp>
#include <functional>

// make a fixture here with appropriate signal impls, and slots, 'extended' slots etc..
// anything that signal_impl objects might need to test their functions.

TEST(SignalImplTest, Constructor)
{
	mcurses::signal_impl<void(int), mcurses::optional_last_value<void>,
				int, std::less<int>, std::function<void(int)>>
					my_sig_impl{mcurses::optional_last_value<void>(), std::less<int>()};

	my_sig_impl.
}