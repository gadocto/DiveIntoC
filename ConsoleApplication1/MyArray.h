#ifndef _INC_MY_ARRAY
#define _INC_MY_ARRAY

#include "Exception.h"
#include "Realloc.h"

const int START_SIZE = 1;

template<typename type>
class myArray
{
private:
	unsigned long startGuard_;
	unsigned int size_;
	type* data_;
	unsigned long endGuard_;

public:
	myArray();
	myArray(unsigned int size);
	myArray(type data[], unsigned int size);
	myArray(myArray<type>& that);
	~myArray();

	myArray& operator=(myArray& that);
	type& operator[](unsigned int i);
	explicit operator type*&();

	bool ok();
	void throwExc();
	unsigned int& size() { return size_; };
	void evalSize(unsigned int lastElement);
	void dump(unsigned int indent);
};

template<typename type>
myArray<type>::myArray() :
	startGuard_(LIVE),
	size_(START_SIZE),
	data_(new type[size_]),
	endGuard_(LIVE)
{
	(*data_) = 0;

	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		delete data_;
		throw new exc_("Constructor error", msg);
	}
}

template<typename type>
myArray<type>::myArray(unsigned int size) :
	startGuard_(LIVE),
	size_(size),
	data_(new type[size]),
	endGuard_(LIVE)
{
	if (size > 0) std::memset(data_, 0, size_ * sizeof(type));

	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		delete data_;
		throw new exc_("Constructor error", msg);
	}
}

template<typename type>
myArray<type>::myArray(type data[], unsigned int size) :
	startGuard(LIVE),
	data_(data),
	size_(size),
	endGuard(LIVE)
{
	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Constructor fucked up", msg);
	}
}

template <typename type>
myArray<type>::~myArray()
{
	if (ok())
	{
		delete data_;
		data_ = NULL;
		startGuard_ = DEAD;
		endGuard_ = DEAD;
	}
}

template <typename type>
myArray<type>::myArray(myArray& that) :
	startGuard_(that.startGuard_),
	size_(that.size_),
	data_(0),
	endGuard_(that.endGuard_)
{
	try
	{
		that.throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Invalid \"that\" argument", msg)
	}

	data_ = new type[size_];

	if (size_ > 0) std::copy(that.data_, that.data_ + size, data_);

	try
	{
		throwExc();
	}

	catch (exc* msg)
	{
		throw new exc_("Copy-constructor fucked up", msg);
	}
}

template <typename type>
myArray<type>& myArray<type>::operator=(myArray<type>& that)
{
	try
	{
		that.throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Invalid \"that\" argument", msg);
	}

	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Bad array to assignment", msg);
	}

	if (that.size_ != size_)
	{
		delete data_;

		size_ = that.size_;
		data_ = new type[size_];
	}

	startGuard_ = that.startGuard_;
	endGuard_ = that.endGuard_;
	if (size_ > 0) std::copy(that.data_, that.data_ + size_, data_);
	
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
type& myArray<type>::operator[](unsigned int i)
{
	try
	{
		throwExc();
	}

	catch (exc* msg)
	{
		throw new exc_("Bad array", msg);
	}

	if (i < 0) throw new deepExc_("Invalid counter (" + std::to_string(i) + ")");

	try
	{
		while (myRealloc(data_, i, size_));
	}

	catch (exc* msg)
	{
		throw new exc_("myRealloc fucked up", msg);
	}

	return data_[i];
}

template<typename type>
myArray<type>::operator type*&()
{
	try
	{
		throwExc();
	}
	catch (exc* msg)
	{
		throw new exc_("Bad array to type conversion", msg);
	}

	return data_;
}

template <typename type>
bool myArray<type>::ok()
{
	return startGuard_ == LIVE && endGuard_ == LIVE && data_ && size_ >= 0;
}

template <typename type>
void myArray<type>::throwExc()
{
	if (startGuard_ == DEAD || endGuard_ == DEAD) throw new deepExc_("Dtor was called");
	else if (startGuard_ != LIVE || endGuard_ != LIVE) throw new deepExc_("Class was shat on");
	if (!data_) throw new deepExc_("Null data pointer");
	if (size_ < 0) throw new deepExc_("Invalid size (" + std::to_string(size_) + ")");
}

template<typename type>
void myArray<type>::evalSize(unsigned int lastElement)
{
	try
	{
		while (myRealloc(data_, lastElement, size_));
	}
	catch (exc* msg)
	{
		throw new exc_("MyRealloc error", msg);
	}
}

template<typename type>
void myArray<type>::dump(unsigned int indent)
{
	for (unsigned int i = 0; i < indent; i++) printf("\t");
	printf("data[%d] = {\n", size_);

	for (unsigned int num = 0; num < size_; num++)
	{
		for (unsigned int i = 0; i < indent; i++) printf("\t");
		printf("\t[%d] = %d", num, data_[num]);
		printf("\n");
	}

	for (unsigned int i = 0; i < indent; i++) printf("\t");
	printf("}");
}


#endif