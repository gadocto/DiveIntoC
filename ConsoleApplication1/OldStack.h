#ifndef STACK_INC
#define STACK_INC

#include <windows.h>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "Exception.h"
#include "Realloc.h"

const int START_SIZE = 1;

template <typename type>
class stack
{
private:
	unsigned long startGuard_;
	unsigned int size_;
	type* data_;
	int counter_;
	unsigned long endGuard_;

	void throwExc();

public:
	stack();
	stack(unsigned int size);
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
	size_(START_SIZE),
	data_(new type [size_]),
	counter_(EMPTY),
	endGuard_(LIVE)
{
	*data_ = 0;
}

template <typename type>
stack<type>::stack(unsigned int size) :
	startGuard_(LIVE),
	size_(size),
	data_(new type[size_]),
	counter_(EMPTY),
	endGuard_(LIVE)
{
	std::memset(data_, 0, size * sizeof(int));
}

template <typename type>
stack<type>::stack(stack& that) :
	startGuard_(that.startGuard_),
	size_(that.size_),
	data_(new type[size_]),
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

	if (counter_ > EMPTY) std::copy(that.data_, that.data_ + that.counter_, data_);
}

template <typename type>
stack<type>::~stack()
{
	if (ok()) delete data_;
	startGuard_ = DEAD;
	endGuard_	= DEAD;
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
		myRealloc(data_, counter_, size_);
	}

	catch (exc* msg)
	{
		throw new exc_("myRealloc ERROR", msg);
	}
	
	counter_ = that.counter_;
	size_ = that.size_;
	startGuard_ = that.startGuard_;
	endGuard_ = that.endGuard_;
	if (that.counter_ > EMPTY) std::copy(that.data_, that.data_ + size_, data_);
	
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
	if (counter_ >= static_cast<int>(size_))
	{
		data_ = myRealloc(data_, size_ * 2, size_);
		size_ *= 2;
	}

	type tmp = data_[counter_];
	data_ [counter_] = val;

	try
	{
		throwExc();
	}

	catch(exc* msg)
	{
		data_[counter_] = tmp;
		counter_--;
		throw new exc_("Push error", msg);
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
		retVal = data_ [counter_];
		data_ [counter_] = 0;
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
		data_ = myRealloc(data_, counter_, size_);
	}

	catch (exc* msg)
	{
		if (counter_ > EMPTY)
		{
			data_[counter_] = retVal;
			counter_++;
		}
		throw new exc_("Realloc error", msg);
	}

	return retVal;
}

template <typename type>
void stack<type>::throwExc()
{
	if (startGuard_ == DEAD || endGuard_ == DEAD) throw new deepExc_("Dtor was called");
	else if (startGuard_ != LIVE || endGuard_ != LIVE) throw new deepExc_("Stack was shat on");
	if (!data_) throw new deepExc_("Invalid data pointer");
	if (counter_ < EMPTY) 
		throw new deepExc_("Counter too small (" + std::to_string(counter_) + "<=" + std::to_string(size_) + ")");
	if (counter_ >= static_cast<int>(size_)) 
		throw new deepExc_("Counter too large (" + std::to_string(counter_) + ">=" + std::to_string(size_) + ")");
}

template <typename type>
bool stack<type>::ok()
{
	return (data_ &&
		counter_ >= EMPTY &&
		counter_ < static_cast<int>(size_) &&
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
	
	printf("\n\n	data[%d] = {\n", size_);
	for (unsigned int i = 0; i < size_; i++)
	{
		printf("		[%d] = %d", i, data_[i]);
		if (static_cast<int>(i) <= counter_) printf(" *");
		printf("\n");
	}


	printf("	}\n}\n");
}

#endif