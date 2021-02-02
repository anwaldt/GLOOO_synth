/// @class LinearSlope
///
/// @author Henrik von Coler
/// @date   2017-10-23
///

#include "LinearSlope.h"


LinearSlope::LinearSlope()
{
    recentPos = 0;

    startVal  = 0.0;
    stopVal   = 1.1;

    length    = 100;

    // slope
    m = (stopVal-startVal) / (double) length ;

}



/// @brief Set
///
/// @param start the starting value
/// @param stop the final value
/// @param L the length in samples
///
void LinearSlope::set_parameters(double start, double stop, int L)
{


    recentPos = 0;

    startVal  = start;
    stopVal   = stop;

    length    = L;

    // slope
    m = (stopVal-startVal) / (double) L ;

}

void LinearSlope::update_parameters(double stop)
{
    stopVal = stop;

    // slope
    m = (stopVal-startVal) / (double) length ;
}


/// @brief Get the next slope value and step forwar.
///
double LinearSlope::get_next_value()
{

    double  val = m * (double) recentPos + startVal;

    recentPos += 1;

    return val;

}


double LinearSlope::get_value_at(double pos)
{

    double  val = m * (double) pos + startVal;

    return val;

}
