#ifndef PATTERN_H
#define PATTERN_H

#pragma warning( disable : 4786 )

#include "globals.h"
#include "Matrix.h"
#include <vector>
#include <fstream>

/** @brief This class is designed to hold information about generic patterns
  * related to Go positions. */
class Pattern
{
	public:
		// An enum of values that the pattern can hold, OSTONE are defending, XSTONE are attacking
		enum PatternPoint { EMPTY_POINT, OSTONE, XSTONE, EMPTY_OR_OSTONE, EMPTY_OR_XSTONE,
			DONTCARE, CORNER, VERTICAL_EDGE, HORIZONTAL_EDGE, KEY_POINT };
		enum PatternSymmetry { NONE = 0, FIRST = 0, Y, X, X_EQUALS_Y, X_EQUALS_MINUS_Y, ROT90, ROT180, ROT270, LAST = ROT270 };

		// Use matrix since we have a fast transpose function available in Matrix::transpose()
		typedef Matrix<PatternPoint> PatternDataType;

		class SymmetryData {
			private:
				PatternSymmetry m_symmetry;
				PatternDataType m_data;
			public:
				SymmetryData( const PatternSymmetry sym, const PatternDataType& data)
					: m_symmetry( sym ), m_data( data ) {};
				const PatternSymmetry type() const { return m_symmetry; };
				const PatternDataType data() const { return m_data; };
				PatternDataType data() { return m_data; };
		};

		typedef vector<SymmetryData> SymmetriesType;

	private:
		// A list of the valid symmetries for this pattern where data[0] is the original pattern
		SymmetriesType data;

		// A pattern number unique within the file it's loaded from.
		int number;

	public:

		static const string PatternTypeID;

		Pattern( const int width, const int height );
		Pattern( const PatternDataType& value );
		Pattern(const Pattern& p);

		virtual Pattern& operator=(const Pattern& p) ;
		virtual bool operator==( const Pattern& p );

		virtual PatternPoint getPoint(const int x, const int y) const;
		virtual void setPoint( const int x, const int y, const PatternPoint& value );

		virtual void addSymmetry( const PatternSymmetry sym );
		virtual void updateSymmetricalVersions( const int x, const int y, const PatternPoint& value );

		virtual bool getSymmetry( const PatternSymmetry& sym, PatternDataType& data );
		virtual bool symmetriesAvailable();
		virtual const SymmetriesType& getAllSymmetries();

		const SymmetryData& getOriginal() const;

		virtual int getWidth() const;
		virtual int getHeight() const;

		virtual void load(ifstream& in);
		virtual void save(ofstream& out);
};

/** @brief Define a postfix increment operator for PatternPoint enum. */
inline Pattern::PatternSymmetry operator++( Pattern::PatternSymmetry& p, int )
{
	return p = (Pattern::PatternSymmetry)(p + 1);
}

#endif