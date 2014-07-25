#ifndef POINT_H
#define POINT_H

#pragma warning( disable : 4786 )

template <class _type = char>
struct Point
{
	public:

		_type x;
		_type y;

		Point() { x = 0; y = 0; };
		Point(_type tx, _type ty) { x = tx; y = ty; };
//		virtual ~Point() {};
		
		bool operator==(const Point& p1) const {	return (x==p1.x&&y==p1.y) ? true : false; };
};

#endif