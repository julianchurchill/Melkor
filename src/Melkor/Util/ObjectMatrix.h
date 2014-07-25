// Just like Matrix.h except this should work for objects not just simple
// data types such as float, int etc...
// Problem was in initMatrix().
// Also this should really just be a fast alternative to <vector>
// so we can ditch the Matrix calculation functions, multiply etc...

//#pragma once
#ifndef OBJECTMATRIX_H
#define OBJECTMATRIX_H
#pragma warning( disable : 4786 )

#include "LogWriter.h"
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;

template <class _type>
class ObjectMatrix
{
	private:

		inline int coord(int j, int i) const { return i*width+j; };
		inline void deleteValues() { if(values!=NULL) delete [] values; };

	public:
		
		// m is across, n is down
		int width, height;
		int size;
		_type* values;

		ObjectMatrix();
		ObjectMatrix(int width, int height);
		ObjectMatrix(vector<vector<_type> >& v, int width, int height);
		ObjectMatrix(_type* v, int width, int height);
		ObjectMatrix(const ObjectMatrix<_type>& m);
		ObjectMatrix<_type>& operator=(const ObjectMatrix<_type>& m);
		virtual ~ObjectMatrix();
		void initMatrix();
		void resize(int w, int h);
		void setValues(vector<vector<_type> >& v);
		void setValues(_type* v, int w, int h);
		void setValue(int x, int y, _type v);
		inline _type& getValue(int x, int y) const { return values[coord(x, y)]; };
		_type* getValues();
		int getHeight() const { return height; };
		int getWidth() const { return width; };

		bool equals(const ObjectMatrix<_type>* obj) const;
		bool operator==(const ObjectMatrix<_type>& obj) const { return equals(&obj); };

};

template <class _type> 
	ObjectMatrix<_type>::ObjectMatrix()
{
	width = 0;
	height = 0;
	size = 0;
	values = NULL;
}

/** Creates appropriate value array and intialises all values to 0.
@param w Width of matrix.
@param h Height of matrix. */
template <class _type> 
	ObjectMatrix<_type>::ObjectMatrix(int w, int h)
{
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		//for(int i=0;i<size;i++)
		//		values[i] = 0;
	}
	else
		values = NULL;
}

/** Copies all values from v and constructs a matrix object from
them.
@param v Array of values to populate the matrix with, v should be
v[m][n] and so consistent with the other arguments.
@param w Width of the matrix.
@param h Height of the matrix.
*/
template <class _type> 
	ObjectMatrix<_type>::ObjectMatrix(vector<vector<_type> >& v, int w, int h)
{
	if(v.size()!=w||v[0].size()!=h)
	{
		LogWriter log;
		log.println("Bad ObjectMatrix assignment.");
		return;
	}
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		setValues(v);
	}
	else
		values = NULL;
}

template <class _type> 
	ObjectMatrix<_type>::ObjectMatrix(_type* v, int w, int h)
{
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		setValues(v, width, height);
	}
	else
		values = NULL;
}

template <class _type> 
	ObjectMatrix<_type>::ObjectMatrix(const ObjectMatrix<_type>& m)
{
	width = m.width;
	height = m.height;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		for(int i=0;i<size;i++)
				values[i] = m.values[i];
	}
	else
		values = NULL;
}

template <class _type>
	ObjectMatrix<_type>& ObjectMatrix<_type>::operator=(const ObjectMatrix<_type>& m)
{
	// beware self assignment, n = n
	if(this!=&m)
	{
		deleteValues();
		width = m.width;
		height = m.height;
		size = width*height;

		if(width>0 && height>0)
		{
			values = new _type[size];
	
			for(int i=0;i<size;i++)
				values[i] = m.values[i];
		}
		else
			values = NULL;
	}
	return *this;
}

template <class _type> 
	ObjectMatrix<_type>::~ObjectMatrix()
{
	deleteValues();
}

template <class _type> 
	void ObjectMatrix<_type>::resize(int w, int h)
{
	deleteValues();
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
		values = new _type[size];
	else
		values = NULL;
}

/** Set all the values to the contents of v. v must be the same
width and height as the matrix it is being copied to.
@param v A two dimensional array containing the values to be
copied into the matrix. */
template <class _type> 
	void ObjectMatrix<_type>::setValues(vector<vector<_type> >& v)
{
	//if(v.size()!=width || v.at(0).size()!=height)
	//	return;
	//_ASSERT(v.size()==width && v.at(0).size()==height);
	assert(v.size()==width && v.at(0).size()==height);
	int c = 0;
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
			values[c++] = v.at(j).at(i);
	}
}

/** Set all the values to the contents of v. v must be the same
width and height as the matrix it is being copied to.
@param v A two dimensional array containing the values to be
copied into the matrix.
@param w Width of the matrix.
@param h Height of the matrix. */
template <class _type> 
	void ObjectMatrix<_type>::setValues(_type* v, int w, int h)
{
	//if(w!=width || h!=height)
	//	return;
	//_ASSERT(w==width || h==height);
	assert(w==width || h==height);
	for(int i=0;i<size;i++)
		values[i] = v[i];
}

/** Set a particular location to a value.
@param x The x coordinate in the matrix of the value to set.
@param y The y coordinate in the matrix of the value to set.
@param v The value to set the specified location to. */
template <class _type> 
	void ObjectMatrix<_type>::setValue(int x, int y, _type v)
{
	//if(x<0 || x>=width || y<0 || y>=height)
	//	return;
	//_ASSERT(x>=0 && x<width && y>=0 && y<height);
	assert(x>=0 && x<width && y>=0 && y<height);
	values[coord(x, y)] = v;
}

/** A comparison method for two matrices.
@param obj The matrix object to compare to this one.
@return True if the values stored in each matrix are the same
and are in the same positions in each matrix too. */
template <class _type> 
	bool ObjectMatrix<_type>::equals(const ObjectMatrix<_type>* obj) const
{
	if(obj->height!=height || obj->width!=width) return false;
	for(int i=0;i<size;i++)
	{
		if(obj->values[i]!=values[i])
			return false;
	} // end for i
	return true;
} // end equals(Object obj)

#endif