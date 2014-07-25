#include "Pattern.h"

const string Pattern::PatternTypeID = "Generic";

Pattern::Pattern( const int width, const int height ) : number( -1 )
{
	// Create an original pattern of size width x height
	data.push_back( SymmetryData( NONE, PatternDataType( width, height, Pattern::PatternPoint::EMPTY_POINT ) ) );
}

Pattern::Pattern( const PatternDataType& value ) : number( -1 )
{
	data.push_back( SymmetryData( NONE, value ) );
}

Pattern::Pattern(const Pattern& p)
{
	data = p.data;
	number = p.number;
}

Pattern& Pattern::operator=(const Pattern& p)
{
	if( &p != this )
	{
		data = p.data;
		number = p.number;
	}
	return *this;
}

/** @brief This function compares the two patterns based on the patterns themselves. */
bool Pattern::operator==( const Pattern& p )
{
	return ( data == p.data );
}

/** @brief Get a point from the original pattern. */
Pattern::PatternPoint Pattern::getPoint(const int x, const int y) const 
{
	// Check the bounds
	if( x < 0 || y < 0 || x > data[0].data().getWidth() || y > data[0].data().getHeight() )
	{
		string message("Coordinates out of bounds in Pattern::getPoint: ");
		message += "x=";
		message += x;
		message += " y=";
		message += y;
		throw message;
	}

	return data[0].data().getValue( x, y );
}

/** @brief Set a point in the original pattern and update the available symmetries. */
void Pattern::setPoint( const int x, const int y, const Pattern::PatternPoint& value ) 
{
	data[0].data().setValue( x, y, value ); 
	updateSymmetricalVersions( x, y, value );
}

/** @brief This function adds the symmetrical version of original
  * specified in sym to the data vector. This is only if it does not 
  * already exist. */
void Pattern::addSymmetry( const Pattern::PatternSymmetry sym )
{
#error todo
	// Use Matrix::doTransform() to create symmetrical versions.

	// Unify Pattern::PatternSymmetry and Matrix::Symmetry types, perhaps
	// in Misc.h, include PatternSymmetry++ operator.
}

/** @brief This function updates all stored symmetrical versions
  * of the pattern given the original pattern has been updated at the 
  * point x,y with value 'value'. */
void Pattern::updateSymmetricalVersions( const int x, const int y, const Pattern::PatternPoint& value )
{
#error todo
}

/** @brief Return the specified symmetrical version of this pattern in data.
  * @param sym The symmetry to return.
  * @param data A reference to an object to store the symmetrical version in.
  * @return True if the symmetry is currently available for this pattern. */
bool Pattern::getSymmetry( const Pattern::PatternSymmetry& sym, Pattern::PatternDataType& data )
{
#error todo
}

/** @return True if there are valid symmetrical versions of this pattern available. */
bool Pattern::symmetriesAvailable()
{ 
	return (data.size()>1);
}

/** @return All the symmetrical versions of this pattern including the original as [0]. */
const Pattern::SymmetriesType& Pattern::getAllSymmetries()
{ 
	return data;
}

/** @return The original pattern - not one of it's symmetrical derivatives. */
const Pattern::SymmetryData& Pattern::getOriginal() const
{
	return data[0];
}

int Pattern::getWidth() const
{
	return data[0].data().getWidth();
}

int Pattern::getHeight() const
{
	return data[0].data().getHeight();
}

/** @brief Load up the pattern and populate data with it's valid symmetries. */
void Pattern::load(ifstream& in)
{
#error todo
	// Load the original pattern and then call addSymmetry() for each valid one.
}

/** @brief Save the original pattern and a statement of the available symmetries, not
  * a symmetrical representation of each. */
void Pattern::save(ofstream& out)
{
#error todo
}
