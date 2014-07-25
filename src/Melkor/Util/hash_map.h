#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "hash_shared.h"
#include <vector>

// Implementation of hash_map taken from in
// B.Stroustrup's "The C++ Programming Language" 3rd edition.

template<class Key, class T, class H = Hash<Key>, class EQ = equal_to<Key>,
	class A = allocator<T> >
class hash_map {

	public:

		typedef T mapped_type;
		typedef Key key_type;
		typedef H Hasher;
		typedef EQ key_equal;

		typedef size_t size_type;

	private:

		struct Entry {
			key_type key;
			mapped_type val;
			Entry* next;		// hash overflow linkn
			bool erased;
			Entry(key_type k, mapped_type v, Entry* n)
				: key(k), val(v), next(n), erased(false) {};
		};

		typedef vector<Entry> EntryTable;
		typedef vector<Entry*> EntryTablePointers;

		EntryTable v;				// the actual entries
		EntryTablePointers b;		// the hash table: pointers into v
									// (each pointer is a linked list, a bucket)

		float max_load;			// keep v.size()<=b.size()*max_load
		float grow;				// when necessary, resize(bucket_count()*grow)

		size_type no_of_erased;	// number of erased entries in v occupied by erased elements

		Hasher hash;			// hash function
		key_equal eq;			// equality

		const T default_value;	// default value used by []

		void rehash(size_type s)
		{
			b.clear();
			b.resize(s);				// add s-b.size() pointers
			for(size_type i=0;i<v.size();i++)		// rehash
			{
				size_type ii = hash(v[i].key)%b.size();	// hash
				v[i].next = b[ii];						// link
//				b[ii] = &v[i];
				b[ii] = v.begin()+i;
			}
		}

	public:

		typedef EntryTableIterator<T, Entry, EntryTable> iterator;
		typedef const ConstEntryTableIterator<T, Entry, EntryTable> const_iterator;

		hash_map(const T& dv = T(), size_type n = DEFAULT_BUCKETS, const H& h=H(), const EQ& e=EQ())
			: default_value(dv), b(n), hash(h), eq(e), no_of_erased(0)
		{
			set_load();					// defaults
			// This is wrong in Stroustrup, he has v.reserve(max_load*size())
			// but size is (v.size()-no_of_erased) which will always be 0 at this point!!!
			v.reserve(max_load*n);	// reserve space for growth
		}
		virtual ~hash_map() {};

		hash_map& operator=(const hash_map &m)
		{
			if(this!=&m)
			{
				max_load = m.max_load;
				grow = m.grow;
				no_of_erased = m.no_of_erased;
				hash = m.hash;
				eq = m.eq;
				v = m.v;
				// the entry table pointers need to be recalculated
				// for our entry table not the copied one
				rehash(m.b.size());
			}
			return *this;
		}

		// return memory used by this object in bytes
		// NOTE: This is usually only an estimate
		// NOTE: This function will not work if the hash map is storing
		// non-primitive types, i.e. objects
		int getMemoryUsage() const
		{
			int bytesUsed = 0;

			// float max_load, float grow;
			bytesUsed+=sizeof(float)*2;
			// size_type no_of_erased;
			bytesUsed+=sizeof(size_type);
			// Hasher hash;    Skip this since sizeof() will be unreliable
//			bytesUsed+=sizeof(hash);
			// key_equal eq;   Skip this since sizeof() will be unreliable
//			bytesUsed+=sizeof(eq);

			// vector<Entry> v;				// the actual entries
			bytesUsed+=sizeof(Entry)*v.capacity();

			// vector<Entry*> b;		// the hash table: pointers into v
			bytesUsed+=sizeof(Entry*)*b.capacity();

			return bytesUsed;
		}

		void set_load(float m = 0.7, float g = 1.6) { max_load = m; grow = g; };

//		const_iterator find(const key_type& k) const { return const_iterator(*(find(k))); };
//		iterator find(const key_type& k) const

		const_iterator find(const key_type& k) const
		{
			size_type i = hash(k)%b.size();	// hash

			for(Entry* p=b[i];p;p=p->next)	// search among entries hashed to i
			{
				if(eq(k,p->key))	// found
				{
					// but it's been erased!
					if(p->erased)
					{
						return const_iterator(*(v.end()), v);
					}
					return const_iterator(*p, v);
				}
			}

			// not found
			return const_iterator(*(v.end()), v);
		}

		iterator find(const key_type& k)
		{
			size_type i = hash(k)%b.size();	// hash

			for(Entry* p=b[i];p;p=p->next)	// search among entries hashed to i
			{
				if(eq(k,p->key))	// found
				{
					// but it's been erased!
					if(p->erased)
					{
						return iterator(*(v.end()), v);
					}
					return iterator(*p, v);
				}
			}

			// not found
			return iterator(*(v.end()), v);
		}

		mapped_type& operator[](const key_type& k)
		{
/*			// Code to help debugging
			int vSize = v.size();
			int vCapacity = v.capacity();
			int bSize = b.size();
			if(vSize==70)
				vSize = vSize;
			if(vSize==vCapacity)
			{
				int a = b.size()*max_load;
				float f = b.size()*max_load;
				vSize = vSize;
			}
			if(vSize>=(b.size()*max_load))
				vSize = vSize;
			if(vSize>=size_type(b.size()*max_load))
				vSize = vSize;
			if(b.size()*max_load<=v.size())
				vSize = vSize;
*/
			size_type i = hash(k)%b.size();	// hash

			for(Entry* p=b[i];p;p=p->next)	// search among entries hashed to i
			{
				if(eq(k,p->key))	// found
				{
					if(p->erased)
					{
						p->erased = false;
						no_of_erased--;
						return p->val = default_value;
					}
					return p->val;
				}
			}
			// else not found
			if(size_type(b.size()*max_load)<=v.size())	// if "too full"
			{
				resize(b.size()*grow);		// grow
				return operator[](k);
			}

			v.push_back(Entry(k, default_value, b[i]));	// add Entry
			b[i] = &v.back();							// point to new element

			return b[i]->val;
		}

		iterator begin() { return iterator(*(v.begin()), v); };
		const_iterator begin() const { return const_iterator(*(v.begin()), v); };
		iterator end() { return iterator(*(v.end()), v); };
		const_iterator end() const { return const_iterator(*(v.end()), v); };

		void resize(size_type s)				// make the size of the hash table n
		{
			if(s<=b.size()) return;
			v.reserve(s*max_load);		// if v needs to reallocate, let it happen now

			if(no_of_erased)			// really remove erased elements
			{
				for(size_type i=v.size()-1;0<=i;i--)
				{
					if(v[i].erased)
					{
						v.erase(&v[i]);
						if(--no_of_erased==0) break;
					} // end erase v
				} // end for (erase all elements waiting to be erased)
			} // end if(no_of_erased)

			rehash(s);
		}

		void erase(iterator p)					// erase the element pointed to
		{
			if(p.getEntry()->erased==false) no_of_erased++;
			p.getEntry()->erased = true;
		}

		void clear(int s = DEFAULT_BUCKETS)		// clear the table and free up the memory
		{
			b.clear();
			b.resize(s);
			v.clear();
			v.reserve(max_load*s);
			no_of_erased = 0;
		}

		size_type size() const { return v.size()-no_of_erased; };

		size_type bucket_count() const 
		{
			return b.size();
		};

		Hasher hash_fun() const { return hash; };		// hash function used
		key_equal val_eq() const { return eq; };		// equality function used
};

#endif