#include "PatternTest.h"
#include "Pattern.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PatternTest );

void PatternTest::setUp()
{
}

void PatternTest::tearDown()
{
}

void PatternTest::testSetAndGetPoint()
{
	Pattern p( 5, 5);

	p.setPoint( 0, 0, Pattern::PatternPoint::EMPTY_POINT );
	CPPUNIT_ASSERT( p.getPoint( 0, 0) == Pattern::PatternPoint::EMPTY_POINT );

	p.setPoint( 1, 0, Pattern::PatternPoint::OSTONE );
	CPPUNIT_ASSERT( p.getPoint( 1, 0) == Pattern::PatternPoint::OSTONE );

	p.setPoint( 2, 0, Pattern::PatternPoint::XSTONE );
	CPPUNIT_ASSERT( p.getPoint( 2, 0) == Pattern::PatternPoint::XSTONE );

	p.setPoint( 3, 0, Pattern::PatternPoint::EMPTY_OR_OSTONE );
	CPPUNIT_ASSERT( p.getPoint( 3, 0) == Pattern::PatternPoint::EMPTY_OR_OSTONE );

	p.setPoint( 4, 0, Pattern::PatternPoint::EMPTY_OR_XSTONE );
	CPPUNIT_ASSERT( p.getPoint( 4, 0) == Pattern::PatternPoint::EMPTY_OR_XSTONE );

	p.setPoint( 0, 1, Pattern::PatternPoint::DONTCARE );
	CPPUNIT_ASSERT( p.getPoint( 0, 1) == Pattern::PatternPoint::DONTCARE );

	p.setPoint( 1, 1, Pattern::PatternPoint::CORNER );
	CPPUNIT_ASSERT( p.getPoint( 1, 1) == Pattern::PatternPoint::CORNER );

	p.setPoint( 2, 1, Pattern::PatternPoint::VERTICAL_EDGE );
	CPPUNIT_ASSERT( p.getPoint( 2, 1) == Pattern::PatternPoint::VERTICAL_EDGE );

	p.setPoint( 3, 1, Pattern::PatternPoint::HORIZONTAL_EDGE );
	CPPUNIT_ASSERT( p.getPoint( 3, 1) == Pattern::PatternPoint::HORIZONTAL_EDGE );

	p.setPoint( 4, 1, Pattern::PatternPoint::KEY_POINT );
	CPPUNIT_ASSERT( p.getPoint( 4, 1) == Pattern::PatternPoint::KEY_POINT );
}

void PatternTest::testOriginal()
{
	Pattern p( 5, 5 );
	CPPUNIT_ASSERT( p.getOriginal().type() == Pattern::PatternSymmetry::NONE );
}

void PatternTest::testLoadAndSave()
{
	ifstream in( "testPattern.txt" );

	Pattern p( 0, 0 );
	p.load( in );
	CPPUNIT_ASSERT( p.getAllSymmetries().size() > 0 );

	in.close();

	ofstream out( "testPatternSave.txt" );
	p.save( out );

	in.open( "testPatternSave.txt" );
	Pattern p2( 0, 0 );
	p2.load( in );

	CPPUNIT_ASSERT( p == p2 );
}

void PatternTest::testSymmetry()
{
	// Make a non symmetric pattern
	// O.X
	// ...
	// ?.*
	Pattern p( 3, 3);
	p.setPoint( 0, 0, Pattern::PatternPoint::OSTONE );
	p.setPoint( 2, 0, Pattern::PatternPoint::XSTONE );
	p.setPoint( 0, 2, Pattern::PatternPoint::DONTCARE );
	p.setPoint( 2, 2, Pattern::PatternPoint::KEY_POINT );

	Pattern::PatternSymmetry sym = Pattern::PatternSymmetry::FIRST;
	for( ; sym != Pattern::PatternSymmetry::LAST; sym++ )
		p.addSymmetry( sym );

	Pattern::PatternDataType data;
	Pattern::PatternDataType symData;

	// For these symmetries the symmetry applied again should yield the original pattern
	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::Y, symData ) );
	Pattern* symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::Y );
	CPPUNIT_ASSERT( symmetryOfP->getSymmetry( Pattern::PatternSymmetry::Y, data ) );
	CPPUNIT_ASSERT( data == p.getOriginal().data() );
	delete symmetryOfP;

	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::X, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::X );
	CPPUNIT_ASSERT( symmetryOfP->getSymmetry( Pattern::PatternSymmetry::X, data ) );
	CPPUNIT_ASSERT( data == p.getOriginal().data() );
	delete symmetryOfP;

	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::X_EQUALS_Y, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::X_EQUALS_Y );
	CPPUNIT_ASSERT( symmetryOfP->getSymmetry( Pattern::PatternSymmetry::X_EQUALS_Y, data ) );
	CPPUNIT_ASSERT( data == p.getOriginal().data() );
	delete symmetryOfP;

	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::X_EQUALS_MINUS_Y, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::X_EQUALS_MINUS_Y );
	CPPUNIT_ASSERT( symmetryOfP->getSymmetry( Pattern::PatternSymmetry::X_EQUALS_MINUS_Y, data ) );
	CPPUNIT_ASSERT( data == p.getOriginal().data() );
	delete symmetryOfP;

	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::ROT180, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::ROT180 );
	CPPUNIT_ASSERT( symmetryOfP->getSymmetry( Pattern::PatternSymmetry::ROT180, data ) );
	CPPUNIT_ASSERT( data == p.getOriginal().data() );
	delete symmetryOfP;

	// Check ROT90 and ROT270
	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::ROT90, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::ROT90 );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 0, 0 ) == Pattern::PatternPoint::DONTCARE );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 2, 0 ) == Pattern::PatternPoint::OSTONE );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 0, 2 ) == Pattern::PatternPoint::KEY_POINT );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 2, 2 ) == Pattern::PatternPoint::XSTONE );
	delete symmetryOfP;

	CPPUNIT_ASSERT( p.getSymmetry( Pattern::PatternSymmetry::ROT270, symData ) );
	symmetryOfP = new Pattern( symData );
	symmetryOfP->addSymmetry( Pattern::PatternSymmetry::ROT270 );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 0, 0 ) == Pattern::PatternPoint::XSTONE );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 2, 0 ) == Pattern::PatternPoint::KEY_POINT );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 0, 2 ) == Pattern::PatternPoint::OSTONE );
	CPPUNIT_ASSERT( symmetryOfP->getOriginal().data().getValue( 2, 2 ) == Pattern::PatternPoint::DONTCARE );
	delete symmetryOfP;
}

void PatternTest::testDataSize()
{
	Pattern p( 5, 5 );

	// Pattern should always have a least 1 symmetry - the original
	CPPUNIT_ASSERT( p.getAllSymmetries().size() == 1 );
}
