#ifndef INC_REALLOC
#define INC_REALLOC

#include <Windows.h>
#include <memory>
#include <string>
#include "Exception.h"

const double HESTERESIS_AMNT = 0.25;
const int EMPTY = -1;

template <typename type>
bool myRealloc(type*& data, const int counter, unsigned int& size)
{
	if (!data) throw new deepExc_("Null pointer");
	if (size <= 0) throw new deepExc_("Invalid size (" + std::to_string(size) + ")");
	if (counter < EMPTY)
		throw new deepExc_("Counter too small (" + std::to_string(counter) + ")");

	unsigned int newSize = size;

	if (counter >= static_cast<int>(size)) newSize = size * 2;
	else if (counter < size / 2 - size * HESTERESIS_AMNT && size > 0) newSize = size / 2;
	else return false;

	type* buf = new type[newSize];
	std::copy(data, data + min(size, newSize), buf);
	delete data;

	data = buf;

	if (newSize > size)
		std::memset(data + size, 0, (newSize - size) * sizeof(type));

	size = newSize;
	return true;
}

#endif