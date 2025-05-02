#ifndef __MY_LIB_GENERATOR_HEADER_H__
#define __MY_LIB_GENERATOR_HEADER_H__

#include <my-lib/macros.h>
#include <my-lib/std.h>

namespace Mylib
{

// ---------------------------------------------------

// still have to work on this
// for now it is only a draft I designed using Deepseek

#include <iostream>

template <typename T, typename NextFn>
class StackGenerator
{
private:
	T current_;
	NextFn next_fn_;  // State transition logic (stored by value)

public:
	StackGenerator (T init, NextFn next_fn) 
		: current_(init), next_fn_(next_fn)
	{
	}

	// Iterator interface
	class Iterator
	{
	private:
		StackGenerator* parent_;
	
	public:
		Iterator (StackGenerator* parent)
			: parent_(parent)
		{
		}

		T operator* () const
		{
			return parent_->current_;
		}

		Iterator& operator++ ()
		{
			parent_->current_ = parent_->next_fn_(parent_->current_);
			return *this;
		}

		bool operator!= (const Iterator&) const
		{
			return true;
		}
	};

	Iterator begin () { return Iterator(this); }
	Iterator end () { return Iterator(nullptr); }  // Dummy sentinel
};

// Example: Fibonacci sequence
int next_fib (int prev)
{
	static int a = 0, b = 1;
	int next = a + b;
	a = b;
	b = next;
	return next;
}

int main() {
// No heap allocations! All state is on the stack.
StackGenerator<int, decltype(&next_fib)> fib(1, next_fib);

int count = 0;
for (int val : fib) {
std::cout << val << " ";
if (++count >= 10) break;  // Stop after 10 elements
}
// Output: 1 1 2 3 5 8 13 21 34 55
}

// statefull generator

struct Counter {
	int step_;
	int operator()(int x) { return x + step_; }
};

Counter counter{2};  // Increment by 2 each time
StackGenerator<int, Counter> gen(0, counter);

for (int val : gen) {
	if (val >= 10) break;
	std::cout << val << " ";  // 0 2 4 6 8
}

// ---------------------------------------------------

} // end namespace Mylib

#endif