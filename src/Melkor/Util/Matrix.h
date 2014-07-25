// An alternative Matrix implementation using _type*** values
// instead of vector<vector<_type> > >* values
// to see if we save on memory

//#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "LogWriter.h"
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
//#include <crtdbg.h>
#include <assert.h>
using namespace std;

template <class _type>
class Matrix
{
	private:

		inline int coord(int j, int i) const { return i*width+j; };
		inline void deleteValues() { if(values!=NULL) delete [] values; };

	public:
		
		// If we assume width and height will be no bigger than
		// 65535 each then we can use unsigned shorts here
//		int width, height;
		unsigned short width, height;
		int size;
		_type* values;

		Matrix();
		Matrix(const int width, const int height);
		Matrix(const int width, const int height, const _type initialValue);
		Matrix(const vector<vector<_type> >& v, const int width, const int height);
		Matrix(const _type* v, const int width, const int height);
		Matrix(const Matrix<_type>& m);
		Matrix<_type>& operator=(const Matrix<_type>& m);
		virtual ~Matrix();
		void initMatrix();
		void resize(const int w, const int h);
		void setAllValues(const _type v);
		void setValues(const vector<vector<_type> >& v);
		void setValues(const _type* v, const int w, const int h);
		void setValue(const int x, const int y, const _type v);
		inline _type& getValue(const int x, const int y) const { return values[coord(x, y)]; };
		_type* getValues() { return values; };
		int getHeight() const { return height; };
		int getWidth() const { return width; };
		int getSize() const { return size; };

		void doTransform(Matrix<_type>& out, const int symmetry) const;
		void convert(Matrix<_type>& out, const int newWidth, const int newHeight) const;

		// scale by a single value
		Matrix<_type>& operator*=(const _type f);
		void multiply(_type f, Matrix<_type>& result) const;

		// multiply two matrices
		// does multiply to this and returns *this
		Matrix<_type>& operator*=(const Matrix<_type>& a);
		// does multiply of this and a to a temporary object
		// and returns temporary, Matrix(this->getValues(), w, h)
		void multiply(const Matrix<_type>& a, Matrix<_type>& result) const;
		//void multiplyWithPointers(const Matrix<_type>& a, Matrix<_type>& result) const;

		// add matrices
		Matrix<_type>& operator+=(const Matrix<_type>& a);
		void sum(const Matrix<_type>& a, Matrix<_type>& result) const;
		void subtract(const Matrix<_type>& a, Matrix<_type>& result) const;

		// return a temporary object, which is this matrix transposed
		Matrix<_type> transpose() const;
		Matrix<_type> transposeWithPointers() const;
		void transpose(Matrix<_type>& in) const;
		void transposeWithPointers(Matrix<_type>& in) const;

		bool equals(const Matrix<_type>* obj) const;
		bool operator==(const Matrix<_type>& obj) const { return equals(&obj); };
		bool operator!=(const Matrix<_type>& obj) const { return !equals(&obj); };
		void printInfo() const;
		static void save(ofstream& out, Matrix<_type>* matrix);
		static Matrix<_type>* load(ifstream& in, int* bytesRead = NULL);

		int getMemoryUsage(const bool includeVectorReserved = false) const;
};

#define Matrix_NONE 0
#define Matrix_ROTATE90 1
#define Matrix_ROTATE180 2
#define Matrix_ROTATE270 3
#define Matrix_REFLECT_Y_EQUALS_ZERO 4
#define Matrix_REFLECT_Y_EQUALS_MINUS_X 5
#define Matrix_REFLECT_X_EQUALS_ZERO 6
#define Matrix_REFLECT_Y_EQUALS_X 7

template <class _type> 
	Matrix<_type>::Matrix()
{
	width = 0;
	height = 0;
	size = 0;
	values = NULL;
}

/** Creates appropriate value array and intialises all values to 0.
@param w Width of matrix.
@param h Height of matrix.
*/
template <class _type> 
	Matrix<_type>::Matrix(const int w, const int h)
{
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		for(int i=0;i<size;i++)
				values[i] = 0;
	}
	else
		values = NULL;
}

/** Creates appropriate value array and intialises all values to initialValue.
@param w Width of matrix.
@param h Height of matrix.
@param initialValue A default value for all elements in the matrix
*/
template <class _type> 
	Matrix<_type>::Matrix(const int w, const int h, const _type initialValue)
{
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
	{
		values = new _type[size];
		for(int i=0;i<size;i++)
				values[i] = initialValue;
	}
	else
		values = NULL;
}

/** Copies all values from v and constructs a matrix object from them.
@param v Array of values to populate the matrix with, v should be
v[m][n] and so consistent with the other arguments.
@param w Width of the matrix.
@param h Height of the matrix. */
template <class _type> 
	Matrix<_type>::Matrix(const vector<vector<_type> >& v, const int w, const int h)
{
	if(v.size()!=w || (v.size()>0 && v[0].size()!=h))
	{
		LogWriter log;
		log.println("Bad Matrix assignment.");
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
	Matrix<_type>::Matrix(const _type* v, const int w, const int h)
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
	Matrix<_type>::Matrix(const Matrix<_type>& m)
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
	Matrix<_type>& Matrix<_type>::operator=(const Matrix<_type>& m)
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
	Matrix<_type>::~Matrix()
{
	deleteValues();
}

/** Initialise all values in the matrix to 0. */
template <class _type> 
	void Matrix<_type>::initMatrix()
{
	for(int i=0;i<size;i++)
			values[i] = (_type)0;
}

template <class _type> 
	void Matrix<_type>::resize(const int w, const int h)
{
	deleteValues();
	width = w;
	height = h;
	size = width*height;

	if(width>0 && height>0)
		values = new _type[size];
	else
		values = NULL;
	// best do this now so we don't forget during the rest of the program
	// otherwise uninitialised variables creep in here....
	initMatrix();
}

template <class _type> 
	void Matrix<_type>::setAllValues(const _type v)
{
	int c = 0;
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
			values[c++] = v;
	}
}

/** Set all the values to the contents of v. v must be the same
width and height as the matrix it is being copied to.
@param v A two dimensional array containing the values to be
copied into the matrix. */
template <class _type> 
	void Matrix<_type>::setValues(const vector<vector<_type> >& v)
{
	//if(v.size()!=width || v[0].size()!=height)
	//	return;
	//_ASSERT(v.size()==width && v[0].size()==height);
	assert(v.size()==width && v[0].size()==height);
	int c = 0;
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
			values[c++] = v[j][i];
	}
}

/** Set all the values to the contents of v. v must be the same
width and height as the matrix it is being copied to.
@param v A two dimensional array containing the values to be
copied into the matrix.
@param w Width of the matrix.
@param h Height of the matrix. */
template <class _type> 
	void Matrix<_type>::setValues(const _type* v, const int w, const int h)
{
	//if(w!=width || h!=height)
	//	return;
	//_ASSERT(w==width || h==height);
	assert(w==width || h==height);

	// NOTE: coord(x,y) = y*width+x
//	float* vPtr = v;
//	float* valuesPtr = values;
	for(int i=0;i<size;i++)
		values[i] = v[i];
//		(*valuesPtr++) = (*vPtr++);
}

/** Set a particular location to a value.
@param x The x coordinate in the matrix of the value to set.
@param y The y coordinate in the matrix of the value to set.
@param v The value to set the specified location to. */
template <class _type> 
	void Matrix<_type>::setValue(const int x, const int y, const _type v)
{
	//if(x<0 || x>=width || y<0 || y>=height)
	//	return;
	//_ASSERT(x>=0 && x<width && y>=0 && y<height);
	assert(x>=0 && x<width && y>=0 && y<height);
	values[coord(x, y)] = v;

	// NOTE: coord(x,y) = y*width+x
//	(*(values+(y*width)+x)) = v;
}

template <class _type> 
	void Matrix<_type>::doTransform(Matrix<_type>& out, const int symmetry) const
{
	//if(out.height!=height || out.width!=width)
//	{
//		LogWriter log;
//		log.println("Width and height must match for this and out in Matrix::doTransform().");
//		return;
//	}
	//_ASSERT(out.height==height && out.width==width);
	assert(out.height==height && out.width==width);
	int i = 0;
	int j = 0;
	switch(symmetry)
	{
		case Matrix_NONE:
			out = *this;
			break;
		case Matrix_ROTATE90:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord(i,(width-1)-j)];
			}
			break;
		case Matrix_ROTATE180:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord((width-1)-j, (height-1)-i)];
			}
			break;
		case Matrix_ROTATE270:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord((height-1)-i, j)];
			}
			break;
		case Matrix_REFLECT_Y_EQUALS_ZERO:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord(j, (height-1)-i)];
			}
			break;
		case Matrix_REFLECT_Y_EQUALS_MINUS_X:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord(i, j)];
			}
			break;
		case Matrix_REFLECT_X_EQUALS_ZERO:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord((width-1)-j, i)];
			}
			break;
		case Matrix_REFLECT_Y_EQUALS_X:
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
					out.values[out.coord(j, i)] = values[coord((height-1)-i, (width-1)-j)];
			}
			break;
		default:
			LogWriter log;
			log.println("Bad symmetry value in Matrix::doTransform().");
			break;
	}
}


// Copy contents of this into out, using the new size
template <class _type> 
	void Matrix<_type>::convert(Matrix<_type>& out, const int newWidth, const int newHeight) const
{

//	if(newHeight*newWidth!=width*height)
//	{
//		LogWriter log;
//		log.println("New dimensions must create same area, Matrix::doTransform().");
//		return;
//	}
//	if(out.height!=newHeight || out.width!=newWidth)
//	{
//		LogWriter log;
//		log.println("newWidth and newHeight must match for this and out in Matrix::doTransform().");
//		return;
//	}
	//_ASSERT(newHeight*newWidth==width*height);
	//_ASSERT(out.height==newHeight && out.width==newWidth);
	assert(newHeight*newWidth==width*height);
	assert(out.height==newHeight && out.width==newWidth);

	int x = 0;
	int y = 0;
	int j;
	for(int i=0;i<newHeight;i++){
		for(j=0;j<newWidth;j++){
			out.values[out.coord(j, i)] = values[coord(x, y)];
			x++;
			if(x>=width){
				x = 0;
				y++;
			}
		}
	}
}

// scale by a single value
template <class _type> 
	Matrix<_type>& Matrix<_type>::operator*=(const _type f)
{
	/*for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
			values[coord(j, i)]*=f;
	}*/
	for(int i=0;i<size;i++)
		values[i]*=f;

	return *this;
}

template <class _type> 
	void Matrix<_type>::multiply(const _type f, Matrix<_type>& result) const
{
#ifdef _DEBUG
	if(result.height!=height || result.width!=width) {
		LogWriter log;
		log.println("Result matrix does not match this.");
		return;
	}
#endif

	for(int i=0;i<size;i++)
		result.values[i]=values[i]*f;
}

// multiply two matrices
// does multiply to this and returns *this
template <class _type> 
	Matrix<_type>& Matrix<_type>::operator*=(const Matrix<_type>& a)
{
#ifdef _DEBUG
	// width must match height of a to multiply
	if(width!=a.height) {
		LogWriter log;
		log.println("Width of this does not match height of a.");
		return *this;
	}
#endif
	Matrix temp(a.width, height);
	// count through height of this and width of a
	int j, c;
	for(int i=0;i<height;i++) {
		for(j=0;j<a.width;j++) {
			// calculate vector ji for temp
			float sum = 0;
			for(c=0;c<width;c++)
				sum+=values[coord(c, i)]*a.values[a.coord(j, c)];
			temp.values[temp.coord(j, i)] = sum;
		} // end for j
	} // end for i
	setValues(temp.values, width, height);
	return *this;
}

/*// does multiply of this and a to a temporary object
// and returns temporary, Matrix(this->getValues(), w, h)
template <class _type> 
	void Matrix<_type>::multiply(const Matrix<_type>& a, Matrix<_type>& result) const
{

// ignore safety checks if hard optimising

#ifdef _DEBUG
	// width must match height of a to multiply
	if(width!=a.height)
	{
		LogWriter log;
		log.println("Width of this does not match height of a.");
		return;
	}
	// result should be a.width x height
	if(result.width!=a.width || result.height!=height)
	{
		LogWriter log;
		log.println("Size of result is incorrect.");
		return;
	}
#endif
	// count through height of this and width of a
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<a.width;j++)
		{
			// calculate vector ji for temp
			float sum = 0;
			for(int c=0;c<width;c++)
			{
				float t = values[coord(c, i)]*a.values[a.coord(j, c)];
				sum+=t;
			}
			result.values[result.coord(j, i)] = sum;
		} // end for j
	} // end for i
}
*/

/* This was multiplyWithPointers() default is above */
// does multiply of this and a to a temporary object
// and returns temporary, Matrix(this->getValues(), w, h)
template <class _type> 
	void Matrix<_type>::multiply(const Matrix<_type>& a, Matrix<_type>& result) const
{

// ignore safety checks if hard optimising

#ifdef _DEBUG
	// width must match height of a to multiply
	if(width!=a.height){
		LogWriter log;
		log.println("Width of this does not match height of a.");
		return;
	}
	// result should be a.width x height
	if(result.width!=a.width || result.height!=height){
		LogWriter log;
		log.println("Size of result is incorrect.");
		return;
	}
#endif

	_type* thisValuesPtr = values;
	_type* aValuesPtr = a.values;
	_type* resultValuesPtr = result.values;
	int aWidth = a.width;

	// count through height of this and width of a
	int j, c;
	float t;
	for(int i=0;i<height;i++){
		for(j=0;j<aWidth;j++){
			// calculate vector ji for temp
			float sum = 0;
			for(c=0;c<width;c++){
//				float t = values[coord(c, i)]*a.values[a.coord(j, c)];
				// NOTE: coord(x,y) = y*width+x
				t = (*(thisValuesPtr+(i*width+c))) * (*(aValuesPtr+(c*aWidth+j)));
				sum+=t;
			}
//			result.values[result.coord(j, i)] = sum;
			(*(resultValuesPtr+(i*aWidth+j))) = sum;
		} // end for j
	} // end for i
}

// add matrices
template <class _type> 
	Matrix<_type>& Matrix<_type>::operator+=(const Matrix<_type>& a)
{
	int h = height;
	int w = width;
	int i, j;

	// specially optimised for same size matrices
	if(a.height==h && a.width==w){
		_type* valuesPtr = values;
		_type* aValuesPtr = a.values;

		for(i=0;i<size;i++)
			(*valuesPtr++)+=(*aValuesPtr++);
//			values[i]+=a.values[i];
	}
	else{
		// get biggest width and height
		if(a.height>h) h = a.height;
		if(a.width>w) w = a.width;

		_type* valuesPtr = values;
		_type* aValuesPtr = a.values;

//		float aval, val;
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
/*				if(j>=width || i>=height) val = (_type)0;
				else val = values[coord(j, i)];
				if(j>=a.width || i>=a.height) aval = (_type)0;
				else aval = a.values[a.coord(j, i)];
				values[coord(j, i)] = val+aval;
*/
				// if this is true, val = 0
				if(j>=width || i>=height){
					// if this is true aval = 0
					if(j>=a.width || i>=a.height)
						(*(valuesPtr+(i*width+j))) = 0;
					else
						(*(valuesPtr+(i*width+j))) = (*(aValuesPtr+(i*a.width+j)));
				}
				else{
					// if this is true aval = 0, therefore values stays the same
//					if(j>=a.width || i>=a.height)
//						(*(valuesPtr+(i*width+j))) = (*(valuesPtr+(i*width+j)));
//					else

					// if aval is in range, aval!=0, therefore +=
					if(j<a.width && i<a.height)
						(*(valuesPtr+(i*width+j))) += (*(aValuesPtr+(i*a.width+j)));
				}
			}
		}
	}

	return *this;
}

template <class _type> 
	void Matrix<_type>::sum(const Matrix<_type>& a, Matrix<_type>& result) const
{
	int h = height;
	int w = width;


	// specially optimised for same size matrices
	if(a.height==h && a.width==w){
		if(result.width!=w || result.height!=h)
			result.resize(w, h);
		for(int i=0;i<size;i++)
			result.values[i] = values[i]+a.values[i];
	}
	else{
		// get biggest width and height
		if(a.height>h) h = a.height;
		if(a.width>w) w = a.width;
		if(result.width!=w || result.height!=h)
			result.resize(w, h);
	
		float aval, val;
		int j;
		for(int i=0;i<h;i++){
			for(j=0;j<w;j++){
				if(j>=width || i>=height) val = (_type)0;
				else val = values[coord(j, i)];
				if(j>=a.width || i>=a.height) aval = (_type)0;
				else aval = a.values[a.coord(j, i)];
				result.values[result.coord(j, i)] = val+aval;
			}
		}
	}
}

// subtract matrix a from this and return answer in result
template <class _type> 
	void Matrix<_type>::subtract(const Matrix<_type>& a, Matrix<_type>& result) const
{
	int h = height;
	int w = width;

	// specaially optimised for same size matrices
	if(a.height==h && a.width==w){
		if(result.width!=w || result.height!=h)
			result.resize(w, h);
		for(int i=0;i<size;i++)
			result.values[i] = values[i] - a.values[i];
	}
	else{
		// get biggest width and height
		if(a.height>h) h = a.height;
		if(a.width>w) w = a.width;
		if(result.width!=w || result.height!=h)
			result.resize(w, h);

		float aval, val;
		int j;
		for(int i=0;i<h;i++){
			for(j=0;j<w;j++){
				if(j>=width || i>=height) val = (_type)0;
				else val = values[coord(j, i)];
				if(j>=a.width || i>=a.height) aval = (_type)0;
				else aval = a.values[a.coord(j, i)];
				result.values[result.coord(j, i)] = val-aval;
			}
		}
	}
}

// return a temporary object, which is this matrix transposed
template <class _type> 
	Matrix<_type> Matrix<_type>::transpose() const
{
	Matrix<_type> temp(height, width);

	int j;
	for(int i=0;i<height;i++){
		for(j=0;j<width;j++)
			temp.values[temp.coord(i, j)] = values[coord(j, i)];
	}
	return Matrix<_type>(temp.values, height, width);
}

// return a temporary object, which is this matrix transposed
template <class _type> 
	Matrix<_type> Matrix<_type>::transposeWithPointers() const
{
	Matrix<_type> temp(height, width);

	// NOTE: coord(x,y) = y*width+x
	_type* tempPtr = temp.values;
	_type* thisPtr = values;

	// NOTE: temp.width is height
	int j;
	for(int i=0;i<height;i++)	{
		for(j=0;j<width;j++)
			*(tempPtr+(j*height)+i) = *(thisPtr+(i*width)+j);
//			temp.values[temp.coord(i, j)] = values[coord(j, i)];
	}
	return Matrix<_type>(temp.values, height, width);
}

// element aji = aij
template <class _type> 
	void Matrix<_type>::transpose(Matrix<_type>& in) const
{
	if(in.height!=width || in.width!=height)
		in.resize(height, width);

	int j;
	for(int i=0;i<height;i++){
		for(j=0;j<width;j++)
			in.values[in.coord(i, j)] = values[coord(j, i)];
	}
}

// element aji = aij
template <class _type> 
	void Matrix<_type>::transposeWithPointers(Matrix<_type>& in) const
{
	if(in.height!=width || in.width!=height)
		in.resize(height, width);

	// NOTE: coord(x,y) = y*width+x
	_type* thisPtr = values;
	_type* inPtr = in.values;

	// NOTE: in.width is height
	int j;
	for(int i=0;i<height;i++){
		for(j=0;j<width;j++)
			*(inPtr+(j*height)+i) = *(thisPtr+(i*width)+j);
//			in.values[in.coord(i, j)] = values[coord(j, i)];
	}
}

/** A comparison method for two matrices.
@param obj The matrix object to compare to this one.
@return True if the values stored in each matrix are the same
and are in the same positions in each matrix too. */
template <class _type> 
	bool Matrix<_type>::equals(const Matrix<_type>* obj) const
{
	if(obj->height!=height || obj->width!=width) return false;

	for(int i=0;i<size;i++){
		if(obj->values[i]!=values[i])
			return false;
	} // end for i
	return true;
} // end equals(Object obj)

template <class _type>
void Matrix<_type>::printInfo() const
{
	char buffer[50];
	LogWriter log;
	log.println("");
	int j;
	for(int i=0;i<height;i++){
		log.print("[ ");
		for(j=0;j<width;j++){
			// float to ascii, 7 digits
			sprintf(buffer, "%*g", 9, values[coord(j, i)]);
			log.print(buffer);
			log.print(" ");
		}
		log.println("]");
	}
	log.println("");
}

/** A routine to save a matrix object to a supplied
DataOutputStream.
@param out The DataOutputStream to write the matrix contents to.
@param matrix The matrix to write to the output stream. */
template <class _type> 
	void Matrix<_type>::save(ofstream& out, Matrix<_type>* matrix)
{
	// save m and n
	//out.writeInt(width);
	int tempInt = matrix->width;
	out.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	//out.writeInt(height);
	tempInt = matrix->height;
	out.write(reinterpret_cast<char*>(&tempInt), sizeof(int));
	
	float tempFloat = 0.0;

	// save data
	int j;
	for(int i=0;i<matrix->height;i++){
		for(j=0;j<matrix->width;j++){
			tempFloat = matrix->values[matrix->coord(j, i)];
			out.write(reinterpret_cast<char*>(&tempFloat), sizeof(_type));
		}
	} // end for i
} // end save

/** A routine to load a matrix object from a given DataInputStream.
@param in The DataInputStream to read the matrix object from.
@param bytesRead An optional argument. If not NULL the number of bytes read during load() will be returned in bytesRead.
@return A matrix object initialised with the data read from the
DataInputStream. */
template <class _type> 
	Matrix<_type>* Matrix<_type>::load(ifstream& in, int* bytesRead/*=NULL*/)
{	
	//int width = in.readInt();
	int w=0;
	// read n
	//int height = in.readInt();
	int h=0;

	int bytes = 0;

	int si = sizeof(int);
	
	in.read(reinterpret_cast<char*>(&w), si);
	bytes+=si;
	if(in.fail()){
		if(bytesRead!=NULL) *bytesRead = bytes;
		return NULL;
	}
	in.read(reinterpret_cast<char*>(&h), si);
	bytes+=si;
	if(in.fail()){
		if(bytesRead!=NULL) *bytesRead = bytes;
		return NULL;
	}
	
	Matrix<_type>* matrix;
	try {
		matrix = new Matrix<_type>(w, h);
	} catch(exception& e) {
		cerr << e.what();
		if(bytesRead!=NULL) *bytesRead = bytes;
		return NULL;
	}
	
	float t=0.0;
	int st = sizeof(_type);

	// read data
	int j;
	for(int i=0;i<h;i++){
		for(j=0;j<w;j++){
			in.read(reinterpret_cast<char*>(&t), st);
			bytes+=st;
			if(in.fail()){
				if(bytesRead!=NULL) *bytesRead = bytes;
				return NULL;
			}
			matrix->values[matrix->coord(j, i)] = t;
		}
	} // end for i
	if(bytesRead!=NULL) *bytesRead = bytes;
	return matrix;
	
} // end load

template <class _type> 
	int Matrix<_type>::getMemoryUsage(const bool includeVectorReserved/*=false*/) const
{
	//int width, height;
	//int size;
	//_type* values;

	int bytesUsed = 0;
	bytesUsed+=sizeof(width);
	bytesUsed+=sizeof(height);
	bytesUsed+=sizeof(size);
	bytesUsed+=sizeof(_type)*size;
	return bytesUsed;
}

#endif