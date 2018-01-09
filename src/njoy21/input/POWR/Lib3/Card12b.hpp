class Card12b{

#include "njoy21/input/POWR/Card12b/Ia.hpp"
#include "njoy21/input/POWR/Card12b/L1.hpp"
#include "njoy21/input/POWR/Card12b/L2.hpp"

Ia ia;
L1 l1;
L2 l2;

template< typename Istream >
Card12b( Istream& is, const int ngnd )
try:
  ia( argument::extract< POWR::Lib3::Card12b::Ia >( is ) ),
  l1( argument::extract< POWR::Lib3::Card12b::L1 >( is, ngnd ) ),
  l2( argument::extract< POWR::Lib3::Card12b::L2 >( is, ngnd ) )
{
  Card::clear( is );
}
catch( std::exception& e ){
  Log::info( "Trouble reading Card 12b." );
  throw e;
}
};
