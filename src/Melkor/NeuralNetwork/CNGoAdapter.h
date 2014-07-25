#ifndef CNGOADAPTER_H
#define CNGOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPN.h"
#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class uses the getInput and associated functions designed for it's     **/
/** source class type.                                                          **/
/*********************************************************************************/

class CNGoAdapter : public BPNGoAdapterInterface
{
	private:
		BPNGoAdapterInterface* goAdapter;

	public:

		// Use 0 nodes per board point and 0 extra nodes because
		// the real GoAdapter is held in goAdapter so this* does not
		// need to have nodesPerBoardPoint or extraNodes
		CNGoAdapter(const string& filename) : BPNGoAdapterInterface(filename, 0, 0)
		{
			// what is the source type of this class?
			if(sourceType==BPN::BPNTYPE)
				goAdapter = new BPNGoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE)
				goAdapter = new newBPNGoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE2)
				goAdapter = new newBPN2GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE3)
				goAdapter = new newBPN3GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE4)
				goAdapter = new newBPN4GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE5)
				goAdapter = new newBPN5GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE6)
				goAdapter = new newBPN6GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE7)
				goAdapter = new newBPN7GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE8)
				goAdapter = new newBPN8GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE9)
				goAdapter = new newBPN9GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE10)
				goAdapter = new newBPN10GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE11)
				goAdapter = new newBPN11GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE12)
				goAdapter = new newBPN12GoAdapter(filename);
			else if(sourceType==BPN::NEWBPNTYPE3GOPROBLEMS)
				goAdapter = new newBPN3GoAdapter(filename);
			else
				throw "CNGoAdapter did not recognise source type...";

			patternWidth = goAdapter->calculatePatternWidth(getBPN());
			patternHeight = goAdapter->calculatePatternHeight(getBPN());;
		};
		virtual ~CNGoAdapter() {};

		void setPoint(Matrix<float>& m, int y, int value) const { goAdapter->setPoint(m, y, value); };

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const
			{ return goAdapter->getInput(x, y, b, input, colour); };

		virtual int calculatePatternWidth(const BPN& bpn) const 
			{ return goAdapter->calculatePatternWidth(bpn); };
		virtual int calculatePatternHeight(const BPN& bpn) const
			{ return goAdapter->calculatePatternHeight(bpn); };
};

#endif
