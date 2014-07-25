#ifndef LINK_H
#define LINK_H

#include <stdio.h>

template <class _type>
class Link
{
	public:

		Link<_type> *next, *prev;
		_type value;

		Link(_type v) : value(v), next(NULL), prev(NULL) {};
};

#endif
