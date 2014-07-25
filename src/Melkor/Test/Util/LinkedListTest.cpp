#include "LinkedListTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( LinkedListTest );

void LinkedListTest::setUp()
{
}

void LinkedListTest::tearDown()
{
}

void LinkedListTest::testCapacity()
{
	LinkedList<int> list;

	CPPUNIT_ASSERT( list.size() == 0 );

	list.reserve(100);

	CPPUNIT_ASSERT( list.size() == 0 );
	CPPUNIT_ASSERT( list.capacity() == 100 );
}

void LinkedListTest::testPointerIntegrity()
{
	LinkedList<int> list(10, 0);

	CPPUNIT_ASSERT( list.size() == 10 );
	CPPUNIT_ASSERT( list.capacity() >= list.size() );

	int* iPtr = &(list.push_back(-1));

	CPPUNIT_ASSERT( list.size() == 11 );
	CPPUNIT_ASSERT( list.capacity() >= list.size() );

	list.reserve(100);

	CPPUNIT_ASSERT( list.size() == 11 );
	CPPUNIT_ASSERT( list.capacity() == 100 );

	// Check the pointer iPtr is still valid after the resize
	CPPUNIT_ASSERT( &(list[10]) == iPtr );
}

void LinkedListTest::testIteration()
{
	LinkedList<int> list;

	for(int i=0;i<10;i++)
		list.push_back(i);

	LinkedList<int>::iterator iter(list.begin());

	CPPUNIT_ASSERT( iter == list.begin() );

	// Iterate through the list but do nothing
	for(;iter!=list.end();iter++) 
		*iter;

	CPPUNIT_ASSERT( iter == list.end() );
}

void LinkedListTest::testPushBack()
{
	LinkedList<int> list;

	for(int i=0;i<10;i++) {
		list.push_back(i);
		CPPUNIT_ASSERT( list.size() == i+1 );
		CPPUNIT_ASSERT( list.capacity() >= list.size() );
	}
}

void LinkedListTest::testErase()
{
	LinkedList<int> list;

	for(int i=0;i<10;i++)
		list.push_back(i);

	list.erase(list.begin());

	CPPUNIT_ASSERT( list.size() == 9 );
	CPPUNIT_ASSERT( list.capacity() >= list.size() );

	list.erase(list.begin()+(list.size()-1));

	CPPUNIT_ASSERT( list.size() == 8 );
	CPPUNIT_ASSERT( list.capacity() >= list.size() );

	list.erase(list.end()-1);

	CPPUNIT_ASSERT( list.size() == 7 );
	CPPUNIT_ASSERT( list.capacity() >= list.size() );
}