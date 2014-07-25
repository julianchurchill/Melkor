#include "AllLegalSuggesterTest.h"
#include "AllLegalSuggester.h"
#include "Melkor.h"
#include "StoneCountGoal.h"

CPPUNIT_TEST_SUITE_REGISTRATION( AllLegalSuggesterTest );

void AllLegalSuggesterTest::setUp()
{
}

void AllLegalSuggesterTest::tearDown()
{
}

void AllLegalSuggesterTest::testGetMoves()
{
	vector<Move> moves;
	BoardStruct test(9);

	// Settings aren't used by AllLegalSuggester
	SearchSettings settings( Goal( new StoneCountGoal() ) );

	settings.setMoveSuggester( new AllLegalSuggester() );

	BPNGoAdapterInterface* bpn = NULL;
	bpn = Melkor::loadBPNStatic(Melkor::defaultNetwork);

	// An empty 9x9 board show return 81 moves
	settings.getMoveSuggester().getMoves(moves, test, settings, BLACK, BLACK, *bpn);
	CPPUNIT_ASSERT( moves.size() != 81 );

	delete bpn;
}
