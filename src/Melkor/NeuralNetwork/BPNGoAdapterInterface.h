#ifndef BPNGOADAPTERINTERFACE_H
#define BPNGOADAPTERINTERFACE_H

#pragma warning( disable : 4786 )

#include "BPN.h"
#include "CN.h"
#include "Matrix.h"
#include "BoardStruct.h"

class BPNGoAdapterInterface
{
	private:
		/** @todo Why don't we keep a copy of a BPN object for ourselves rather than
		  * relying on an external pointer that may be deleted...? */
		BPN* internalBPN;

	protected:
		int patternWidth, patternHeight;
		int nodesPerBoardPoint, extraNodes;
		int sourceType;

	public:

		static const float ACT_OURCOLOUR;
		static const float ACT_THEIRCOLOUR;
		static const float ACT_EMPTY;
		static const float ACT_OFFBOARD;

		static const float ACT_BLACK;
		static const float ACT_WHITE;

		static const float BLACK_TERRITORY_THRESHOLD;
		static const float WHITE_TERRITORY_THRESHOLD;
		static const float NEUTRAL_HIGH_THRESHOLD;
		static const float NEUTRAL_LOW_THRESHOLD;

		// NOTE: It is the child classes responsibilty to make sure that patternWidth, patternHeight
		// and inputHeight are all set in their own constructors using the calculate*() functions.
		BPNGoAdapterInterface(const string& filename, int np, int en) : patternWidth(-1), patternHeight(-1),
			nodesPerBoardPoint(np), extraNodes(en)
		{
			int id = BPN::getType(filename);
			if(id==-1) {
				string message = "Filename doesn't appear to be a valid BPN: ";
				message+=filename;
				throw message;
			}

			sourceType = -1;
			if(id==BPN::CNTYPE) {
				CN cn(filename);
				sourceType = cn.getSourceType();
				internalBPN = new CN(filename);
			}
			else
				internalBPN = new BPN(filename);
			
			if(internalBPN->getErrorCode()!=BPN::NONE)
				throw "Error encountered while initialising internalBPN";
		}
		virtual ~BPNGoAdapterInterface()
		{
			if(internalBPN!=NULL)
				delete internalBPN;
		};

		void getInputFieldPoints(const BoardStruct::contentsType& board, vector<SpecialPoint>& points, int x, int y) const;
		bool validateInputFieldSize(int numberOfBoardPoints, string& error) const;
		virtual bool validateParameters(int inputNodes, vector<int>& hiddenNodes, int outputNodes, string& error) const;

		virtual void setPoint(Matrix<float>& m, int y, int value) const {};

		// standard move scoring nets
		virtual bool getInput(int x, int y, const BoardStruct& b, Matrix<float>& input, int colour) const = 0;

		// this is a special case function for comparing two moves (x1, y1) and (x2, y2)
		// see urgency nets work for details and use
		virtual bool getInput(int x1, int y1, int x2, int y2, const BoardStruct& b, Matrix<float>& input,
			int colour) const { return false; };

		virtual int calculatePatternWidth(const BPN& bpn) const
		{
			if(bpn.getInputFieldShape()==IFS_SQUARE)
				return sqrt((bpn.getWeights()[0].getHeight()-extraNodes)/nodesPerBoardPoint);
			else if(bpn.getInputFieldShape()==IFS_DIAMOND)
				return sqrt((((bpn.getWeights()[0].getHeight()-extraNodes)/nodesPerBoardPoint)*2)-1);
			return 0;
		}
		virtual int calculatePatternHeight(const BPN& bpn) const 
		{
			if(bpn.getInputFieldShape()==IFS_SQUARE)
				return sqrt((bpn.getWeights()[0].getHeight()-extraNodes)/nodesPerBoardPoint);
			else if(bpn.getInputFieldShape()==IFS_DIAMOND)
				return sqrt((((bpn.getWeights()[0].getHeight()-extraNodes)/nodesPerBoardPoint)*2)-1);
			return 0;
		}

		int getPatternWidth() const { return patternWidth; };
		int getPatternHeight() const { return patternHeight; };

		const BPN& getBPN() const { return *internalBPN; };
		BPN& getBPN() { return *internalBPN; };

		virtual float getActivationValue(int c, int colour) const;
		virtual float getOutput(int x, int y, const BoardStruct& b, int colour);

		/***********/
		/** Tests **/
		/***********/

		// hiddenInfo vector must return average output of each hidden node
		bool rankTest(const vector<string>& sgffiles, float& result, vector<pair<int,float> >* distrib = NULL, 
			bool printDetails = false, bool hiddenNodeInfo = false, vector<float>* hiddenInfo = NULL, int printEveryN = -1,
			int everyNPercent = -1);
		float localRankTest(const vector<string>& sgffiles, vector<pair<int,float> >* distrib = NULL,
			bool printDetails = false, bool hiddenNodeInfo = false,
			vector<float>* hiddenInfo = NULL, int printEveryN = -1, int everyNPercent = -1);

		float topNMoveFrequencyTest(const vector<string>& sgffiles, int topNMoves, int printEveryN = -1);
		float localTopNMoveFrequencyTest(const vector<string>& sgffiles, int topNMoves, int manhattanSize, int printEveryN = -1);
		float twoMoveChoiceTest(const vector<string>& sgffiles, int manhattanSize);

		bool safetyTest(const vector<string>& sgffiles, float& result, bool printDetails = false);
		bool evalTest(const string& sgftestdir, float& result, bool hiddenNodeInfo = false,
			vector<float>* hiddenInfo = NULL);

		bool urgencyTest(const vector<string>& sgffiles, float& result, bool hiddenNodeInfo = false,
			vector<float>* hiddenInfo = NULL);

//		virtual bool trainOnSGF(SGFReader& sgf, vector<char>& results, bool momentum = true, float lambda = 0.0f)
//		{ return true; };
};

#endif