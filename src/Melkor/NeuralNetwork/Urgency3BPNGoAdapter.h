#ifndef URGENCY3BPNGOADAPTER_H
#define URGENCY3BPNGOADAPTER_H

#pragma warning( disable : 4786 )

#include "BPN.h"
#include "BPNGoAdapterInterface.h"
using namespace std;

/*********************************************************************************/
/** This class uses the getInput and associated functions designed for it's     **/
/** source class type.                                                          **/
/*********************************************************************************/

class Urgency3BPNGoAdapter : public BPNGoAdapterInterface
{
	private:
		BPNGoAdapterInterface* goAdapter;

	public:

		// Use 0 nodes per board point and 0 extra nodes because
		// the real GoAdapter is held in goAdapter so this* does not
		// need to have nodesPerBoardPoint or extraNodes
		Urgency3BPNGoAdapter(BPN* tempBpn, int sourceType) : BPNGoAdapterInterface(tempBpn, 0, 0)
		{
			// what is the source type of this class?
			if(sourceType==BPN::BPNTYPE)
				goAdapter = new BPNGoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE)
				goAdapter = new newBPNGoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE2)
				goAdapter = new newBPN2GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE3)
				goAdapter = new newBPN3GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE4)
				goAdapter = new newBPN4GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE5)
				goAdapter = new newBPN5GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE6)
				goAdapter = new newBPN6GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE7)
				goAdapter = new newBPN7GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE8)
				goAdapter = new newBPN8GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE9)
				goAdapter = new newBPN9GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE10)
				goAdapter = new newBPN10GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE11)
				goAdapter = new newBPN11GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE12)
				goAdapter = new newBPN12GoAdapter(tempBpn);
			else if(sourceType==BPN::NEWBPNTYPE3GOPROBLEMS)
				goAdapter = new newBPN3GoAdapter(tempBpn);
			else
				throw "Urgency3BPNGoAdapter did not recognise source type...";

			patternWidth = goAdapter->calculatePatternWidth(getBPN());
			patternHeight = goAdapter->calculatePatternHeight(getBPN());;
		};
		virtual ~Urgency3BPNGoAdapter() {};

		virtual bool validateParameters(int inputNodes, vector<int>& hiddenNodes, int outputNodes, string& error) const
			{ return goAdapter->validateParameters(inputNodes, hiddenNodes, outputNodes, error); };
		void setPoint(Matrix<float>& m, int y, int value) const { goAdapter->setPoint(m, y, value); };

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const
			{ return goAdapter->getInput(x, y, b, input, colour); };

		virtual int calculatePatternWidth(const BPN& bpn) const 
			{ return goAdapter->calculatePatternWidth(bpn); };
		virtual int calculatePatternHeight(const BPN& bpn) const
			{ return goAdapter->calculatePatternHeight(bpn); };
		
/*		void setPoint(Matrix<float>& m, int y, int value) const;

		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const;

		virtual int calculatePatternWidth(const BPN& bpn) const { return sqrt((bpn.getWeights()[0].getHeight()-18)/4); };
		virtual int calculatePatternHeight(const BPN& bpn) const { return sqrt((bpn.getWeights()[0].getHeight()-18)/4); };
*/
};

#endif
