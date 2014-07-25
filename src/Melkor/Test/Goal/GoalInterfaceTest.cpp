#include "GoalInterfaceTest.h"
#include "StoneCountGoal.h"

CPPUNIT_TEST_SUITE_REGISTRATION( GoalInterfaceTest );

void GoalInterfaceTest::setUp()
{
}

void GoalInterfaceTest::tearDown()
{
}

void GoalInterfaceTest::testGoalInterfaceAssign()
{
	// Just create a goal with a SmartPtr and let it get deleted
	// NOTE: For some reason we can't copy SmartPtr<>s and still
	//  have the benefit of reference counting - it will 
	//  throw an exception when it comes to deleting the copied pointer...
	SmartPtr<GoalInterface> sg(new StoneCountGoal());

	// These lines throw an exception at the end of the function
//	Goal sgGoal(sg);
//	Goal sgGoal((GoalInterface*)sg);

	GoalInterface* sgToadPtr = new StoneCountGoal();
	Goal* toadPtr = new Goal( sgToadPtr );

	// This copies the GoalInterface* sgToadPtr into a 2nd SmartPtr in toadPtr2
	// causing two reference counts of sgToadPtr and so a crash!
//	Goal* badToadPtr2 = new Goal( sgToadPtr );

	// This line throws an exception at the end of the function
//	Goal badToad( toadPtr->getGoals().begin()->goal );

	// This line throws an exception at the end of the function
//	Goal badToad2( sgToadPtr );

	// Try copying toad and its SmartPtrs
	Goal copyToad( *toadPtr );

	Goal copyToad2( NULL );

#ifdef _DEBUG
	_ASSERTE( _CrtCheckMemory() );
	_ASSERTE( _CrtIsValidHeapPointer( sgToadPtr ) );
	_ASSERTE( _CrtIsValidHeapPointer( toadPtr ) );
#endif

	// No need to delete sgToadPtr since GoalInterface implements
	// reference counting and has been copied into a SmartPtr in Goal
	// SmartPtr will delete it when done

	// mustDeletePtr on the otherhand is _not_ copied into a SmartPtr
	// therefore we must delete it explicitly
	GoalInterface* mustDeletePtr = new StoneCountGoal();
	delete mustDeletePtr;

	delete toadPtr;
//	delete badToadPtr2;
}
