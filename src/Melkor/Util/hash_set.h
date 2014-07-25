#ifndef HASH_SET
#define HASH_SET

#include "hash_shared.h"
#include "LinkedList.h"
#include <vector>

// My implementation of hash_set based on hash_map in
// B.Stroustrup's "The C++ Programming Language" 3rd edition.

// NOTE: Since this is a set the objects stored are their 
// own key, hence no key value is required, unlike with hash_map

// Specialised function object for Node to perform hashing
// Therefore this hash_set cannot be used for other objects
// without supplying a specialised hashing function

template<class T, class H = Hash<T>, class EQ = equal_to<T>,
	class A = allocator<T> >
class hash_set {

	public:

		typedef T mapped_type;
		typedef H Hasher;
		typedef EQ Equal;

		typedef size_t size_type;

	private:

		struct Entry {
			mapped_type val;
			Entry* next;		// hash overflow linkn
			bool erased;
			Entry()
				: next(NULL), erased(false) {};
			Entry(mapped_type v, Entry* n = NULL)
				: val(v), next(n), erased(false) {};
			int getMemoryUsage(bool includeVectorReserved = false) const
			{
				int bytesUsed = 0;
				bytesUsed+=val.getMemoryUsage(includeVectorReserved);
				bytesUsed+=sizeof(next);
				bytesUsed+=sizeof(erased);
				return bytesUsed;
			}
		};

		// change this later to Matrix:
		//typedef Matrix<Entry> EntryTable;
		//typedef Matrix<Entry*> EntryTablePointers;
		/** @todo Replace the EntryTable type with a linked list implementation that
		  * implements the functions: operator[], v.end(), v.begin(), v.back(), v.push_back(), v.size(),
		  * v.reserve(), v.clear() 
		  * Try LinkedList. */
		typedef vector<Entry> EntryTable;
//		typedef LinkedList<Entry> EntryTable;
		typedef vector<Entry*> EntryTablePointers;

		EntryTable v;				// the actual entries
		EntryTablePointers b;		// the hash table: pointers into v
									// (each pointer is a linked list, a bucket)

		float max_load;			// keep v.size()<=b.size()*max_load
		float grow;				// when necessary, resize(bucket_count()*grow)

		size_type no_of_erased;	// number of erased entries in v occupied by erased elements

		Hasher hash;			// hash function
		Equal eq;			// equality

		const T default_value;	// default value used by []

		// we must monitor the size of vector<Entry*> b with this variable
		// because b.size() does not return true size when dealing with 
		// uninitialised pointers...
		int bSize;
		
		void rehash(size_type s)
		{
			for(size_type i=0;i<v.size();i++)		// rehash
			{
				size_type ii = hash(v[i].val)%bSize;	// hash
				v[i].next = b[ii];						// link
				b[ii] = &v[i];
			}
		}

	public:

		typedef EntryTableIterator<T, Entry, EntryTable> iterator;
		typedef const EntryTableIterator<T, Entry, EntryTable> const_iterator;
//		typedef EntryTableLinkedListIterator<LinkedList<Entry>::iterator, Entry, EntryTable> iterator;
//		typedef const EntryTableLinkedListIterator<LinkedList<Entry>::const_iterator, Entry, EntryTable> const_iterator;

		hash_set(size_type n = DEFAULT_BUCKETS, const H& h=H(), const EQ& e=EQ())
			: default_value(T()), b(n), hash(h), eq(e)
		{
			set_load();					// defaults
			bSize = n;
			// This is wrong in Stroustrup, he has v.reserve(max_load*size())
			// but size is (v.size()-no_of_erased) which will always be 0 at this point!!!
			v.reserve(max_load*n);	// reserve space for growth
		}
		virtual ~hash_set() {};

		void set_load(float m = 0.7, float g = 1.6) { max_load = m; grow = g; };

		inline const_iterator find(const mapped_type& k) const { return const_iterator(*(find(k))); };
		iterator find(const mapped_type& k)
		{
			size_type i = hash(k)%bSize;	// hash

			for(Entry* p=b[i];p;p=p->next)	// search among entries hashed to i
			{
				if(eq(k,p->val))	// found
				{
					if(p->erased)
					{
						p->erased = false;
						no_of_erased--;
						p->val = default_value;
						return iterator(*p, v);
					}
					return iterator(*p, v);
				}
			}

			// not found
			return iterator(*(v.end()), v);
		}

		mapped_type& insert(const mapped_type& k)
		{
			// assume element k is not in the hash table already
			if(int(bSize*max_load)<=v.size())	// if "too full"
			{
				resize(bSize*grow);		// grow
				return insert(k);
			}

			size_type i = hash(k)%bSize;	// hash
			v.push_back(Entry(k, b[i]));	// add Entry
			b[i] = &v.back();							// point to new element

			return b[i]->val;
		}

		iterator begin() { return iterator(*(v.begin()), v); };
		iterator end() { return iterator(*(v.end()), v); };

		void resize(size_type s)				// make the size of the hash table n
		{
			if(s<=bSize) return;
			b.clear();
			b.resize(s);				// add s-b.size() pointers
			bSize = s;
			v.reserve(s*max_load);		// if v needs to reallocate, let it happen now

			if(no_of_erased)			// really remove erased elements
			{
				for(size_type i=v.size()-1;0<=i;i--)
				{
					if(v[i].erased)
					{
						// Modified to work with LinkedList - should still work with STL classes
//						v.erase(&v[i]);
						v.erase(v.begin()+i);
						if(--no_of_erased==0) break;
					} // end erase v
				} // end for (erase all elements waiting to be erased)
			} // end if(no_of_erased)
			rehash(s);
		}

		void erase(iterator p)					// erase the element pointed to
		{
			if(p->erased==false) no_of_erased++;
			p->erased = true;
		}

		void clear(int s = DEFAULT_BUCKETS)		// clear the table and free up the memory
		{
			b.clear();
			b.resize(s);
			bSize = s;
			v.clear();
			v.reserve(max_load*s);
			no_of_erased = 0;
		}

		size_type size() const { return v.size()-no_of_erased; };

		size_type bucket_count() const 
		{
			return bSize;
		};

		Hasher hash_fun() const { return hash; };		// hash function used
		Equal val_eq() const { return eq; };		// equality function used

		/** Get the memory usage of this object.
		 *  @returns Memory usage in bytes. */
		int getMemoryUsage(bool includeVectorReserved = false) const
		{
			//EntryTable v;				// the actual entries
			//EntryTablePointers b;		// the hash table: pointers into v
			//float max_load;			// keep v.size()<=b.size()*max_load
			//float grow;				// when necessary, resize(bucket_count()*grow)
			//size_type no_of_erased;	// number of erased entries in v occupied by erased elements
			//Hasher hash;			// hash function
			//Equal eq;			// equality
			//const T default_value;	// default value used by []
			//int bSize;

			int bytesUsed = 0;

			for(int i=0;i<v.size();i++)
				bytesUsed+=v[i].getMemoryUsage(includeVectorReserved);
			if(includeVectorReserved)
				bytesUsed+=(v.capacity()-v.size())*default_value.getMemoryUsage(includeVectorReserved);

			if(includeVectorReserved)
				bytesUsed+=(b.capacity()*sizeof(Entry*));
			else
				bytesUsed+=(b.size()*sizeof(Entry*));
			bytesUsed+=sizeof(max_load);
			bytesUsed+=sizeof(grow);
			bytesUsed+=sizeof(no_of_erased);
			bytesUsed+=sizeof(hash);
			bytesUsed+=sizeof(eq);
			bytesUsed+=default_value.getMemoryUsage(includeVectorReserved);
			bytesUsed+=sizeof(bSize);

			return bytesUsed;
		}
};

#endif