// coroutine.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <string>
#define __GETVALUE_DEMO__1
//#define __GETVALUE_DEMO__ 1
//#define __YIELD_DEMO__ 1
using namespace std;
using namespace std::chrono;
using namespace std::experimental;
#if __COUNTER_DEMO__
struct resumable_thing
{
	struct promise_type
	{
		resumable_thing get_return_object()
		{
			return resumable_thing(coroutine_handle<promise_type>::from_promise(*this));
		}
		auto initial_suspend() { return suspend_never{}; }
		auto final_suspend() { return suspend_never{}; }
		void return_void() {}
	};
	coroutine_handle<promise_type> _coroutine = nullptr;
	resumable_thing() = default;
	resumable_thing(resumable_thing const&) = delete;
	resumable_thing& operator=(resumable_thing const&) = delete;
	resumable_thing(resumable_thing&& other)
		: _coroutine(other._coroutine) {
		other._coroutine = nullptr;
	}
	resumable_thing& operator = (resumable_thing&& other) {
		if (&other != this) {
			_coroutine = other._coroutine;
			other._coroutine = nullptr;
		}
	}
	explicit resumable_thing(coroutine_handle<promise_type> coroutine) : _coroutine(coroutine)
	{
	}
	~resumable_thing()
	{
		if (_coroutine) { _coroutine.destroy(); }
	}
	void resume() { _coroutine.resume(); }
};
resumable_thing counter() {
	// compiler generates...
	//__counter_context* __context = new __counter_context{};
	//__return = __context->_promise.get_return_object();
	//co_await __context->_promise.initiali_suspend();
	cout << "counter: called\n";
	for (unsigned i = 1; ; i++)
	{
		co_await suspend_always{};
		cout << "counter:: resumed\n";
	}
	co_return;
	//__final_suspend_label:
	//     co_await __context->_promise.final_suspend();
	//     delete __context;
}
resumable_thing named_counter(std::string name)
{
	cout << "counter(" << name << ") was called\n";
	for (unsigned i = 1;; ++i)
	{
		co_await suspend_always{};
		cout << "counter(" << name << ") resumed #" << i << '\n';
	}
}
int main()
{
	//test().get();
	//auto result = test();
	//counter();
	//cout << this_thread::get_id() << ": back in main" << endl;
	resumable_thing counter_a = named_counter("a");
	resumable_thing counter_b = named_counter("b");
	counter_a.resume();
	counter_b.resume();
	counter_b.resume();
	counter_a.resume();
}
#endif
#if __GETVALUE_DEMO__
struct resumable_thing
{
	struct promise_type
	{
		int _value;
		resumable_thing get_return_object()
		{
			return resumable_thing(coroutine_handle<promise_type>::from_promise(*this));
		}
		auto initial_suspend() { return suspend_never{}; }
		//auto final_suspend() { return suspend_never{}; }
		auto final_suspend() { return suspend_always{}; }
		void return_value(int value) { _value = value; }
	};
	int get() { return _coroutine.promise()._value; }
	coroutine_handle<promise_type> _coroutine = nullptr;
	resumable_thing() = default;
	resumable_thing(resumable_thing const&) = delete;
	resumable_thing& operator=(resumable_thing const&) = delete;
	resumable_thing(resumable_thing&& other)
		: _coroutine(other._coroutine) {
		other._coroutine = nullptr;
	}
	resumable_thing& operator = (resumable_thing&& other) {
		if (&other != this) {
			_coroutine = other._coroutine;
			other._coroutine = nullptr;
		}
	}
	explicit resumable_thing(coroutine_handle<promise_type> coroutine) : _coroutine(coroutine)
	{
	}
	~resumable_thing()
	{
		if (_coroutine) { _coroutine.destroy(); }
	}
	void resume() { _coroutine.resume(); }
};
resumable_thing get_value() {
	// compiler generates...
	//__counter_context* __context = new __counter_context{};
	//__return = __context->_promise.get_return_object();
	//co_await __context->_promise.initiali_suspend();
	cout << "get_value: called\n";
	co_await suspend_always{};
	cout << "get_value: resume\n";
	co_return 30;
	// __context->promise.return_value(30);
	// goto __final_suspend_label;
	//__final_suspend_label:
	//     co_await __context->_promise.final_suspend();
	//     delete __context;
}
int main()
{
	cout << "main: calling get_value\n";
	resumable_thing value = get_value();
	cout << "main: resuming get_value\n";
	value.resume();
	cout << "main: value was " << value.get() << '\n';
}
#endif
#if __YIELD_DEMO__
struct int_generator
{
	struct iterator;
	struct promise_type {
		int const* _current = nullptr;
		int_generator get_return_object() {
			return int_generator(coroutine_handle<promise_type>::from_promise(*this));
		}
		auto initial_suspend() { return suspend_always{}; }
		auto final_suspend() { return suspend_always{}; }
		auto yield_value(int const& value) {
			_current = &value;
			return suspend_always{};
		}
	};
	coroutine_handle<promise_type> _coroutine = nullptr;
	explicit int_generator(coroutine_handle<promise_type> coroutine) : _coroutine(coroutine)
	{
	}
	int_generator(int_generator const&) = delete;
	int_generator& operator=(int_generator const&) = delete;
	int_generator(int_generator&& other)
		: _coroutine(other._coroutine) {
		other._coroutine = nullptr;
	}
	int_generator& operator = (int_generator&& other) {
		if (&other != this) {
			_coroutine = other._coroutine;
			other._coroutine = nullptr;
		}
	}
	~int_generator()
	{
		if (_coroutine) { _coroutine.destroy(); }
	}
	struct iterator : std::iterator<input_iterator_tag, int>
	{
		coroutine_handle<promise_type> _coroutine = nullptr;
		iterator(coroutine_handle<promise_type> coroutine) : _coroutine(coroutine) {}
		iterator() = default;
		iterator(iterator const&) = delete;
		iterator& operator=(iterator const&) = delete;
		iterator(iterator&& other)
			: _coroutine(other._coroutine) {
			other._coroutine = nullptr;
		}
		iterator& operator = (iterator&& other) {
			if (&other != this) {
				_coroutine = other._coroutine;
				other._coroutine = nullptr;
			}
		}
		iterator& operator++()
		{
			_coroutine.resume();
			if (_coroutine.done()) { _coroutine = nullptr; }
			return *this;
		}
		int const& operator*() const
		{
			return *_coroutine.promise()._current;
		}
		bool operator!=(const iterator& other)
		{
			return !(*this == other);
		}
		bool operator==(const iterator& other)
		{
			if (other._coroutine == _coroutine)
				return true;

			if (other._coroutine && _coroutine)
				return *_coroutine.promise()._current == *other._coroutine.promise()._current;
			else
				return false;
		}
	};
	iterator begin()
	{
		if (_coroutine)
		{
			_coroutine.resume();
			if (_coroutine.done()) { return end(); }
		}
		return iterator(_coroutine);
	}
	iterator end() { return iterator(); }
};
int_generator integers(int first, int last)
{
	for (int i = first; i <= last; ++i)
	{
		co_yield i;
		// co_await __promise.yield_value(i);
	}
}
int main()
{
	for (int x : integers(1, 5))
	{
		cout << x << '\n';
	}
	//int_generator the_integers = integers(1, 5);
	//for (auto it = the_integers.begin(); it != the_integers.end(); ++it)
	//{
	//     cout << *it << '\n';
	//}
}
#endif
