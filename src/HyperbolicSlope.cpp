#include "HyperbolicSlope.h"

HyperbolicSlope::HyperbolicSlope()
{

}

void HyperbolicSlope::set_parameters(double y_0, double y_1, int N, double gamma)
{



    startVal = y_0;
    stopVal  = y_1;

    /// Set min and max values for glissando parameters.
    this->gamma     = std::min(3.0, std::max(gamma,0.0));
    this->length    = std::max(N,1);

}

void HyperbolicSlope::update_target_value(double y_1)
{
    stopVal  = y_1;
}


double HyperbolicSlope::get_next_value()
{
    // return (c + d * tanh( steepness * pow(relative_position,skewness) -0.5));
}


double HyperbolicSlope::get_value_at(double pos)
{

    double d = std::abs(startVal-stopVal) / 2.0;
    double c = std::min(startVal,stopVal) + d;

    // use signum
    double sig = 1;
    if(stopVal-startVal < 0)
        sig = -1;


    double y     = c + d * std::tanh( 10.0 * gamma* sig* (pos -  ((double) length/2.0))/ (double) length );
    double fac   = std::pow(gamma,2.0);
    double sl    =  ((stopVal-startVal) / (double) length ) * pos + startVal;
    double out   = fac * y + (1-fac)* sl;

    return out;
}
