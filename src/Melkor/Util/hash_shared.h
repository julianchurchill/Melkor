#ifndef HASH_SHARED_H
#define HASH_SHARED_H

#include "Node.h"
#include <functional>
using namespace std;

#define DEFAULT_BUCKETS 101

template<class T> 
struct Hash : unary_function<T, size_t> 
{
	// Hashing function
	// Uses bytes of key's representation to create hash
	size_t operator() (const T& key) const
	{
		size_t res = 0;
		size_t len = sizeof(T);
		const char* p = reinterpret_cast<const char*>(&key);
		
		while (len--) res = (res<<1)^*p++;
		return res;
	}
};

// specialisation for string (which contains a pointer to characters)
/*template <class C>
struct Hash<basic_string<C> > : unary_function<basic_string<C>, size_t>
{
	size_t operator() (const basic_string<C>& key) const
	{
		size_t res = 0;
		typedef basic_string<C>::const_iterator CI;
		CI p = key.begin();
		CI end = key.end();
	
		// use int value of characters
		while(p!=end) res = (res<<1)^*p++;
		return res;
	}
};*/

template <>
struct Hash<Node> : unary_function<Node, size_t>
{
	size_t operator() (const Node& val) const
	{
		return val.getKey();
	};
};

// specialisation for char* (c-style string, i.e. pointers to characters)
template <>
struct Hash<char*> : unary_function<char*, size_t>
{
	size_t operator() (const char* key) const
	{
		size_t res = 0;
		// use int value of characters
		while(*key) res = (res<<1)^*key++;
		return res;
	}
};

/** @brief This iterator class assumes the entry table has entries
  * consecutively in memory, i.e. vector<>. Use EntryTableLinkedListIterator
  * for non-consecutive container classes. */
template<class ETI_T, class ETI_N, class ETI_E>
class EntryTableIterator {

	private:
		ETI_N* current;
		const ETI_E* entryTable;
			
	public:
		EntryTableIterator() { current = NULL; entryTable = NULL; };
		EntryTableIterator(ETI_N& c, const ETI_E& e) { current = &c; entryTable = &e; };
		virtual ~EntryTableIterator() {};
		
		// define ->, *, =, ++, ==
		ETI_T* operator->() const { return &(current->val); };
		ETI_T& operator*() const { return (current->val); };

		ETI_N* getEntry() { return current; };
				
		// prefix
		EntryTableIterator& operator++() { ++current; return *this; };
		// postfix
		EntryTableIterator operator++(int)
		{
			EntryTableIterator t(*current, *entryTable); 
			++current;
			return t;
		}
				
		bool operator==(const EntryTableIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator!=(const EntryTableIterator& e) const { return !(*this==e); };
};

template<class ETI_T, class ETI_N, class ETI_E>
class ConstEntryTableIterator {

	private:
		const ETI_N* current;
		const ETI_E* entryTable;
			
	public:
		ConstEntryTableIterator() { current = NULL; entryTable = NULL; };
		ConstEntryTableIterator(const ETI_N& c, const ETI_E& e) { current = &c; entryTable = &e; };
		ConstEntryTableIterator(EntryTableIterator& e)
		{
			current = e.current;
			entryTable = e.entryTable;
		}
		virtual ~ConstEntryTableIterator() {};
		
		// define ->, *, =, ++, ==
		const ETI_T* operator->() const { return &(current->val); };
		const ETI_T& operator*() const { return (current->val); };

		bool operator==(const ConstEntryTableIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator==(const EntryTableIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator!=(const ConstEntryTableIterator& e) const { return !(*this==e); };
		bool operator!=(const EntryTableIterator& e) const { return !(*this==e); };
};

/** @brief This iterator class is for entry tables that don't store 
  * entries in consecutive memory, i.e. ++current in operator++ won't work. */
template<class ETI_T, class ETI_N, class ETI_E>
class EntryTableLinkedListIterator {

	private:
		ETI_N* current;
		const ETI_E* entryTable;
			
	public:
		EntryTableLinkedListIterator() { current = NULL; entryTable = NULL; };
		EntryTableLinkedListIterator(ETI_N& c, const ETI_E& e) { current = &c; entryTable = &e; };
		virtual ~EntryTableLinkedListIterator() {};
		
		// define ->, *, =, ++, ==
		ETI_T* operator->() const { return &(current->val); };
		ETI_T& operator*() const { return (current->val); };

		ETI_N* getEntry() { return current; };
				
		// prefix
		EntryTableLinkedListIterator& operator++() { ++(current->val); return *this; };
		// postfix
		EntryTableLinkedListIterator operator++(int)
		{
			EntryTableLinkedListIterator t(*current, *entryTable); 
			++(current->val);
			return t;
		}
				
		bool operator==(const EntryTableLinkedListIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator!=(const EntryTableLinkedListIterator& e) const { return !(*this==e); };
};

/** @brief This iterator class is for entry tables that don't store 
  * entries in consecutive memory, i.e. ++current in operator++ won't work. */
template<class ETI_T, class ETI_N, class ETI_E>
class ConstEntryTableLinkedListIterator {

	private:
		const ETI_N* current;
		const ETI_E* entryTable;
			
	public:
		ConstEntryTableLinkedListIterator() { current = NULL; entryTable = NULL; };
		ConstEntryTableLinkedListIterator(const ETI_N& c, const ETI_E& e) { current = &c; entryTable = &e; };
		ConstEntryTableLinkedListIterator(EntryTableLinkedListIterator& e)
		{
			current = e.current;
			entryTable = e.entryTable;
		}
		virtual ~ConstEntryTableLinkedListIterator() {};
		
		// define ->, *, =, ++, ==
		const ETI_T* operator->() const { return &(current->val); };
		const ETI_T& operator*() const { return (current->val); };
						
		bool operator==(const ConstEntryTableLinkedListIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator==(const EntryTableLinkedListIterator& e) const
		{
			return (e.current==current && e.entryTable==entryTable);
		}
		bool operator!=(const ConstEntryTableLinkedListIterator& e) const { return !(*this==e); };
		bool operator!=(const EntryTableLinkedListIterator& e) const { return !(*this==e); };
};

#endif