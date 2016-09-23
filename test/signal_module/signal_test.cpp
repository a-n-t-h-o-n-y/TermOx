#include <mcurses/signal_module/signal.hpp>
#include <gtest/gtest.h>

TEST(SignalTest, DefaultConstructor)
{
	mcurses::signal<double(int)> sig;
	mcurses::signal<void(int)> sig2;
	mcurses::signal<void()> sig3;
}

TEST(SignalTest, SignalInSlot)
{
	// create signal
	// add some slots to it
	// create a slot
	// add the above signal to the slot
	// add that slot to another signal
	// connect another slot to the first signal
	// the slot you just added to the first signal will only be run when you run the first
	// signal directly, when you run it via the second signal, it will not
	// have the connection in it, since it only has a copy of the connection container
	// at the point it was turned into a slot.
	// connections to previous slots will still work though, you can disconnect
	// and that will be recognized by runing the first signal via the second.
}

TEST(SignalTest, ExtendedSlots)
{
	// create an extended slot and track items with it
	// then add that as an extended slot to a signal
	// destroy a tracked object and check to see if the 
	// extended slot is expired, or if the function will no 
	// longer run when the signal is called, which would actually
	// throw an expired_slot exception
}