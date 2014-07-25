#ifndef MY_VECTOR_H
#define MY_VECTOR_H

// This class represents vector of whatever
// but without std::vector annoying iterators
// and pointer swapping around stuff

// It is growable just like std::vector
// currently no functions for removing elements from the vector...

#define DEFAULT_ELEMENTS 10

template <class T> 
class my_vector
{
	public:
		my_vector()
		{
			set_load();
			// reserve some default amount of memory for this vector
			reserved = DEFAULT_ELEMENTS;
			contents = new T[reserved];
			if(contents==NULL)
				throw bad_alloc();
			size = 0;
		}

		~my_vector() { delete [] contents; };

#error do this
		// don't forget copy and '=' constructors
		// and '==' while you're at it

		void push_back(T& t)
		{
			// if reserved memory used up
			if(size==reserved)
			{
				// allocate more memory and copy contents over
				// increase storage by a factor of 'grow', i.e. current_size*grow
				reserved*=grow;
				T** temp = new T[reserved];
				if(temp==NULL)
					throw bad_alloc();
				for(int i=0;i<size;i++)
					temp[i] = contents[i];
				contents = temp;
			}

			contents[size] = t;
			size++;
		}

		size_t size() const { return size; };
		void clear();
		T& back() { return contents[size]; };

	private:
		// use an array of pointers to objects of type T
		// for efficiency
		T** contents;
		size_t reserved;
		size_t size;

		void set_growth(float g = 1.6) { grow = g; };

		float grow; // growth factor when more memory is needed, i.e. new_size = current_size*grow
};

#endif