#ifndef RECT_H
#define RECT_H

#pragma warning( disable : 4786 )

class Rect
{
	public:

		int top, left, bottom, right;

		Rect();
		Rect(int t, int l, int b, int r);
		virtual ~Rect() {};
};

#endif