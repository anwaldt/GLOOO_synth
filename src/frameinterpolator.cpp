/// @class FrameInterpolator
///
/// @author Henrik von Coler
/// @date   2017-10-23
///


#include "frameinterpolator.h"

FrameInterpolator::FrameInterpolator()
{

}


void FrameInterpolator::set_parameters(double start, double stop, int L)
{
    recentPos = 0;

    startVal = start;
    stopVal  = stop;

    length   = L;

    // slope
    m = (stopVal-startVal) / (float) L ;

}

double FrameInterpolator::get_next_value()
{

    float  val = m * (float) recentPos + (float) startVal;

    recentPos += 1;

    return val;

}
