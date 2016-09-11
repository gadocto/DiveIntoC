#ifndef INC_REALLOC
#define INC_REALLOC

#include <Windows.h>
#include <memory>
#include <string>
#include "Exception.h"

const double HESTERESIS_AMNT = 0.25;
const int EMPTY = -1;

template <typename type>
type* myRealloc(type* data, const int counter_, unsigned int& size)
{
	if (!data) throw new deepExc_("Null pointer");
	if (size <= 0) throw new deepExc_("Invalid size (" + std::to_string(size) + ")");
	if (counter_ < EMPTY)
		throw new deepExc_("Counter too small (" + std::to_string(counter_) + ")");

	unsigned int newSize = size;

	if (counter_ >= static_cast<int>(size)) newSize = size * 2;
	if (counter_ < static_cast<int>(size / 2 - size) * HESTERESIS_AMNT && size > 1) newSize = size / 2;

	type* buf = new type[newSize];
	std::copy(data, data + newSize, buf);

	if (newSize > size)
	{
		std::memset(buf + size, 0, (newSize - size) * sizeof(type));
	}

	return buf;
}

#endif