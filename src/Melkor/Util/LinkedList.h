#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Link.h"
#include <stdio.h>

template<class _type>
class LinkedListIterator
{
	private:
		Link<_type>* m_link;

	public:
		LinkedListIterator(Link<_type>* l) : m_link(l) {};
		LinkedListIterator(const LinkedListIterator& l) : m_link(l.m_link) {};

		// Prefix
		LinkedListIterator& operator++() 
		{ 
			if(m_link!=NULL) m_link = m_link->next;
			return *this; 
		}
		// Postfix
		LinkedListIterator operator++(int)
		{
			LinkedListIterator t(*this);
			if(m_link!=NULL) ++(*this);
			return t;
		}
		// Prefix
		LinkedListIterator& operator--() 
		{ 
			if(m_link!=NULL) m_link = m_link->prev; 
			return *this; 
		}

		LinkedListIterator& operator+(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->next;
			}
			return *this;
		}
		LinkedListIterator& operator-(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->prev;
			}
			return *this;
		}

		Link<_type>* getLink() { return m_link; };

		_type& operator->()	const { return m_link->value; };
		_type& operator*() const { return m_link->value; };

		bool operator==(const LinkedListIterator& e) const { return (e.m_link==m_link); };
		bool operator!=(const LinkedListIterator& e) const { return !(*this==e); };
};

template <>
class LinkedListIterator<int>
{
	private:
		Link<int>* m_link;

	public:
		LinkedListIterator(Link<int>* l) : m_link(l) {};
		LinkedListIterator(const LinkedListIterator& l) : m_link(l.m_link) {};

		// Prefix
		LinkedListIterator& operator++() 
		{ 
			if(m_link!=NULL) m_link = m_link->next; 
			return *this; 
		}
		// Postfix
		LinkedListIterator operator++(int)
		{
			LinkedListIterator t(*this);
			if(m_link!=NULL) ++(*this);
			return t;
		}
		// Prefix
		LinkedListIterator& operator--() 
		{ 
			if(m_link!=NULL) m_link = m_link->prev; 
			return *this; 
		}

		LinkedListIterator& operator+(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->next;
			}
			return *this;
		}
		LinkedListIterator& operator-(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->prev;
			}
			return *this;
		}

		Link<int>* getLink() { return m_link; };

		int& operator*() const { return m_link->value; };

		bool operator==(const LinkedListIterator& e) const { return (e.m_link==m_link); };
		bool operator!=(const LinkedListIterator& e) const { return !(*this==e); };
};

template<class _type>
class ConstLinkedListIterator
{
	private:
		const Link<_type>* m_link;

	public:
		ConstLinkedListIterator(Link<_type>* l) : m_link(l) {};
		ConstLinkedListIterator(const ConstLinkedListIterator& l) : m_link(l.m_link) {};

		// Prefix
		ConstLinkedListIterator& operator++() const 
		{
			if(m_link!=NULL) m_link = m_link->next; 
			return *this;
		}
		// Postfix
		ConstLinkedListIterator operator++(int)
		{
			ConstLinkedListIterator t(*this);
			if(m_link!=NULL) ++(*this);
			return t;
		}
		// Prefix
		ConstLinkedListIterator& operator--() const 
		{
			if(m_link!=NULL) m_link = m_link->prev;
			return *this; 
		}

		ConstLinkedListIterator& operator+(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->next;
			}
			return *this;
		}
		ConstLinkedListIterator& operator-(int n)
		{
			for(int i=0;i<n;i++) {
				if(m_link!=NULL) 
					m_link = m_link->prev;
			}
			return *this;
		}

		const _type& operator->() const { return m_link->value; };
		const _type& operator*() const { return m_link->value; };

		bool operator==(const ConstLinkedListIterator& e) const { return (e.m_link==m_link); };
		bool operator!=(const ConstLinkedListIterator& e) const { return !(*this==e); };
		bool operator==(const LinkedListIterator& e) const { return (e.m_link==m_link); };
		bool operator!=(const LinkedListIterator& e) const { return !(*this==e); };
};

/** @brief This linked list class will be slow for access ([]) operations but has the advantage
  * over STL vector<> in that pointers into it are guaranteed not to change. */
template <class _type>
class LinkedList
{
	private:

		int m_size, m_capacity;
		float m_load, m_grow;
		/** m_being is the start of the user objects. m_back is the last user object. m_first is
		  * the first reserved object - will == m_begin when m_size > 0. */
		Link<_type> *m_begin, *m_back, *m_first;
		_type m_defValue;

		int initialCapacity;

	public:
		typedef LinkedListIterator<_type> iterator;
		typedef ConstLinkedListIterator<_type> const_iterator;

		LinkedList() : initialCapacity(10), m_defValue(_type()), m_begin(NULL), m_back(NULL), m_first(NULL), 
			m_size(0), m_capacity(0), m_load(0.7f), m_grow(1.6f)
		{
			reserve(initialCapacity);
		}
		LinkedList(const int initialSize, _type defValue) : initialCapacity(10), m_defValue(defValue), m_begin(NULL),
			m_back(NULL), m_first(NULL), m_size(0), m_capacity(0), m_load(0.7f), m_grow(1.6f)
		{
			resize(initialSize);
		}
		~LinkedList()
		{
			clear();
		}

		int size() const { return m_size; };
		int capacity() const { return m_capacity; };
		/** @brief Note: end() is _not_ guaranteed to point after the last element in
		  * the linked list, unlike the usual STL classes. */
		iterator end() { return iterator(m_back)+1; };
		const_iterator end() const { return const_iterator(m_back)+1; };
		iterator begin() { return iterator(m_begin); };
		const_iterator begin() const { return const_iterator(m_begin); };

		_type& back() { return m_back->value; };
		const _type& back() const { return m_back->value; };

		void erase(iterator it)
		{
			if(it.getLink()->prev!=NULL)
				it.getLink()->prev->next = it.getLink()->next;
			if(it.getLink()->next!=NULL)
				it.getLink()->next->prev = it.getLink()->prev;

			// If m_begin...
			if(it.getLink()==m_begin)
				m_first = m_begin = it.getLink()->next;
			if(it.getLink()==m_back)
				m_back = it.getLink()->next;

			m_size--;
			m_capacity--;

			delete it.getLink();
		}
		
		_type& push_back(const _type& t)
		{
//			if(m_size==m_capacity) reserve(((float)m_capacity)*((float)m_grow));
			m_size++;
			if(m_size>=m_capacity) 
				reserve(m_capacity*m_grow);
			if(m_back==NULL) m_back = m_begin = m_first;
			m_back->next->value = t;
			m_back = m_back->next;
			return m_back->value;
		}

		_type& operator[](int i)
		{
			if(i>m_size) return *end();

			Link<_type>* current = m_begin;
			for(int index=1;index<=i;index++)
				current = current->next;
			return current->value;
		}

		const _type& operator[](int i) const
		{
			if(i>m_size) return *end();

			Link<_type>* current = m_begin;
			for(int index=0;index<i;index++)
				current = current->next;
			return current->value;
		}

		void resize(int r)
		{
			if(r > m_capacity) reserve(r);
			m_size = r;
			// Update m_back and m_begin
			m_back = m_begin = m_first;
			for(int i=1;i<m_size;i++)
				m_back = m_back->next;
		}

		void reserve(int r)
		{
			if(r<=0) return;

			// If we've got an empty list start it up
			if(m_first==NULL) {
				m_first = new Link<_type>(m_defValue);
				m_capacity++;
			}

			// Find the end of the list capacity
			Link<_type>* current = m_first;
			for(int i=1;i<m_capacity;i++)
				current = current->next;

			for(i=m_capacity;i<r;i++) {
				current->next = new Link<_type>(m_defValue);
				current->next->prev = current;
				current = current->next;
			}
			m_capacity = r;
		}

		/** @brief Delete all objects in this list, size and capacity will be 0. */
		void clear()
		{
//			Link<_type>* current = m_begin;
			Link<_type>* current = m_first;
			Link<_type>* next;
			while(current!=NULL) {
				next = current->next;
				delete current;
				current = next;
			}
			m_begin = m_back = m_first = NULL;
			m_size = 0;
			m_capacity = 0;
		}
};

#endif