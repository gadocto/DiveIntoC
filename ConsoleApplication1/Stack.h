#ifndef STACK_INC
#define STACK_INC

#include <windows.h>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "Exception.h"
#include "Realloc.h"
#include "MyArray.h"

template <typename type>
class stack
{
private:
	unsigned long startGuard_;
	myArray<type> data_;
	int counter_;
	unsigned long endGuard_;

	void throwExc();

public:
	stack();
	stack(unsigned int size);
	stack(myArray<type> data);
	stack(stack& that);
	~stack();

	stack& operator=(stack& that);

	void push(type val);
	type pop();

	bool ok();
	void dump();
};

template <typename type>
stack<type>::stack() :
	startGuard_(LIVE),
	data_(),
	counter_(EMPTY),
	endGuard_(LIVE)
{
}

template <typename type>
stack<type>::stack(unsigned int size) :
	startGuard_(LIVE),
	data_(size),
	counter_(EMPTY),
	endGuard_(LIVE)
{
}

template<typename type>
stack<type>::stack(myArray<type> data) :
	startGuard_(LIVE),
	data_(data),
	counter_(data.size()),
	endGuard_(LIVE)
{
}

template <typename type>
stack<type>::stack(stack& that) :
	startGuard_(that.startGuard_),
	data_(that.data_),
	counter_(that.counter_),
	endGuard_(that.endGuard_)
{
	try
	{
		that.throwExc();
	}
	catch (exc* msg)
	{
		delete data_;
		throw new exc_("Invalid stack to copy constructor", msg);
	}
}

template <typename type>
stack<type>::~stack()
{
	if (ok())
	{
		startGuard_ = DEAD;
		endGuard_ = DEAD;
	}
}

template <typename type>
stack<type>& stack<type>::operator= (stack<type>& that)
{
	try
	{
		that.throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Invalid \"that\" stack argument", msg);
	}

	try
	{
		data_ = that.data_;
	}
	catch (exc* msg)
	{
		throw new exc_("Assignment error", msg);
	}

	counter_ = that.counter_;
	startGuard_ = that.startGuard_;
	endGuard_ = that.endGuard_;

	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Copy-assignment fucked up", msg);
	}
	return *this;
}

template <typename type>
void stack<type>::push(type val)
{
	try
	{
		throwExc();
	}

	catch (exc* msg)
	{
		throw new exc_("Bad stack pushed", msg);
	}

	counter_++;
	try
	{
		myRealloc(static_cast<type*&>(data_), counter_, data_.size());
	}
	catch (exc* msg)
	{
		counter_--;
		throw new exc_("myRealloc error", msg);
	}

	type tmp = data_[counter_];
	data_[counter_] = val;
	
	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		data_[counter_] = tmp;
		counter_--;
		throw new exc_("Push fucked up", msg);
	}
}

template <typename type>
type stack<type>::pop()
{
	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Bad stack popped", msg);
	}

	type retVal = 0;
	if (counter_ > EMPTY)
	{
		retVal = data_[counter_];
		data_[counter_] = 0;
		counter_--;
	}
	else if (counter_ == EMPTY) throw new deepExc_("Popping from an empty stack");
	else if (counter_ <  EMPTY) throw new deepExc_("Invalid counter (" + std::to_string(counter_) + ")");

	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		if (counter_ > EMPTY)
		{
			data_[counter_] = retVal;
			counter_++;
		}
		throw new exc_("Pop error", msg);
	}

	try
	{
		data_.evalSize(counter_);
	}
	catch (exc* msg)
	{
		if (counter_ > EMPTY)
		{
			data_[counter_] = retVal;
			counter_++;
		}
		throw new exc_("Resize error", msg);
	}

	return retVal;
}

template <typename type>
void stack<type>::throwExc()
{
	if (startGuard_ == DEAD || endGuard_ == DEAD) throw new deepExc_("Dtor was called");
	else if (startGuard_ != LIVE || endGuard_ != LIVE) throw new deepExc_("Stack was shat on");
	
	try
	{
		data_.throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Invalid array", msg);
	}
	
	if (counter_ < EMPTY)
		throw new deepExc_("Counter too small (" + std::to_string(counter_) + "<=" + std::to_string(data_.size()) + ")");
	if (counter_ >= static_cast<int>(data_.size()))
		throw new deepExc_("Counter too large (" + std::to_string(counter_) + ">=" + std::to_string(data_.size()) + ")");
}

template <typename type>
bool stack<type>::ok()
{
	return (data_.ok() &&
		counter_ >= EMPTY &&
		counter_ < static_cast<int>(data_.size()) &&
		endGuard_ == LIVE &&
		startGuard_ == LIVE);
}

template <typename type>
void stack<type>::dump()
{
	printf("CStack");
	if (ok()) printf("(ok)\n");
	else printf("(ERROR)\n");

	printf("{\n");
	printf("	guard_ = %08x | %08x", startGuard_, endGuard_);
	if (startGuard_ == LIVE && endGuard_ == LIVE) printf("(everything is fine)");
	else
	{
		if (startGuard_ == DEAD && endGuard_ == DEAD) printf("(dtor was called!!!)");
		else printf("(stack was shat on!!!)");
	}

	printf("\n	counter_ = %d", counter_);
	if (counter_ == EMPTY) printf(" (empty)");
	else if (counter_ < EMPTY) printf(" (OOPS!)");
	else printf(" (%d filled)", counter_ + 1);
	
	data_.dump(1);
	
	printf("\n}\n");
}

#endif