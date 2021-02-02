/**
  *
 * @file IirFilter.cpp
 * @class IirFilter
 *
 * @brief actual IIR implementation with coeficients and buffers.
 *
 *  @date  2018-11-11
 *
 *  @author   Henrik von Coler
**/

#ifndef IIRFILTER_H
#define IIRFILTER_H


#include "BarkCoefficients.h"

class IirFilter
{

public:



    IirFilter();


    /// @brief Initialize filter for one band
    ///
    /// @param BC: the bark coefficients
    /// @param band: the band to initialize the filter for
    ///
    void init(std::shared_ptr<BarkCoefficients> BC,  int band);

    /// @brief Push a new value to filter input and get next output
    ///
    /// @param input: the filter input value
    /// @return the next output value from the IIR filter
    ///
    double process_value(double input);

private:

    int order;
    int N;

    double* b;
    double* a;

    double* out_buff;
    double* in_buff;

};

#endif // IIRFILTER_H
