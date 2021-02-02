/**
  *
 * @file BarkCoefficients.cpp
 * @class BarkCoefficients
 *
 * @brief reads and holds all IIR coefficients for the bark filter bank
 *
 *  @date       2018-11-05
 *  @author     Henrik von Coler
**/

#ifndef BARKCOEFFICIENTS_H
#define BARKCOEFFICIENTS_H

#include "GloooConfig.h"
#include "glooo.h"

class BarkCoefficients
{
public:

    BarkCoefficients(GloooConfig *gc);

    /// @brief Get the a-coefficient for band and idx
    double get_a(int band, int idx);


    /// @brief Get the b-coefficient for band and idx
    double get_b(int band, int idx);

    /// @brief Getter for the number of bands.
    int n_bands();

    /// @brief Getter for the filter order.
    int get_order();

    /// @brief Getter for the filter length.
    int get_N();

    /// @brief Get the amplitude of a band at a frequency
    double get_gaussian_amplitude(int band, double frequency);

private:

    /// @brief The number of frequency bands used
    int nBands;

    /// @brief the filter order
    /// - assuming all filters have the same structures
    int order;

    /// @brief the number of coefficients
    ///  - for both a and b coefficients
    ///  - assuming al filters have the same order
    int nCoeff;

    /// @brief the jack sampling rate
    int fs;

    /// @brief holds the b-coefficients for all bands
    /// - dimensions: [nBands][nCoeff]
    double** B;

    /// @brief holds the a-coefficients for all bands
    /// - dimensions: [nBands][nCoeff]
    double** A;

    /// @brief holds parametersof a gaussian distribution
    /// for each frequency band
    vector <glooo::gaus_dist> gauss;

};

#endif // BARKCOEFFICIENTS_H
