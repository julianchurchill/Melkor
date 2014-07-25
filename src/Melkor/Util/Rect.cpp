#pragma warning( disable : 4786 )

#include "Rect.h"

Rect::Rect()
{
	top = 0;
	left = 0;
	bottom = 0;
	right = 0;
}

Rect::Rect(int t, int l, int b, int r)
{
	top = t;
	left = l;
	bottom = b;
	right = r;
}
