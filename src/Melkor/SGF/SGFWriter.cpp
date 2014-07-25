#include "SGFWriter.h"
#include <ostream>

/** @brief Construct an SGF tree from a list of moves and a game info
  * object. All other constructors assume that what they're copying
  * from is complete, i.e. has all the game info nodes needed etc... 
  * @param moves The list of moves to convert into an SGF tree.
  * @param info An SGF game info object containing setup info such 
  * as player names, rules, result... */
SGFWriter::SGFWriter(vector<Move>& moves, SGFGameInfo& info)
{
	st = new SGFTree();

	// put setup/handicap/game info nodes into st first
	st->setGameInfo(info);

	// then fill up st with moves as nodes
	SGFNode* current = st;
	SGFNode n;
	string m;
	string col;
	vector<Move>::iterator iter = moves.begin();
	for(;iter!=moves.end();iter++) {
		if(iter->getColour()==BLACK)
			col = "B";
		else
			col = "W";

		m = Move::xyToSGF(iter->getX(), iter->getY(), info.size);
		n.setProperty(col, m);
		current->addChild(n);
		current = current->getChild();
	}
}

/** @brief Save the stored SGF tree. 
  * @param filename The filename to save under. */
void SGFWriter::save(string filename)
{
	// open an ostream to filename
	ofstream fos(filename.c_str(), ios::out);
	if(!fos) {
		string message = "Could not open file for writing: ";
		message+=filename;
		LogWriter::printerr(message, "SGFWriter");
	}

	// start sgf game
	fos << "(";

	// walk through the st SGFTree
	st->save(fos);

	// end sgf game
	fos << ")";

	fos.close();
}

/** @brief Save the SGF tree specified.
  * @param filename The filename to save under. 
  * @param tree The SGF tree to save. */
void SGFWriter::save(string filename, SGFTree& tree)
{
	// open an ostream to filename
	ofstream fos(filename.c_str(), ios::out);
	if(!fos) {
		string message = "Could not open file for writing: ";
		message+=filename;
		LogWriter::printerr(message, "SGFWriter");
	}

	// start sgf game
	fos << "(";

	// walk through the st SGFTree
	tree.save(fos);

	// end sgf game
	fos << ")";

	fos.close();
}