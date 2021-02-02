/**
 *
 * @file ExponentialDecay.h
 *
 * @class ExponentialDecay
 *
 * @brief used for partialamplitude
 * releases
 *
 * @date  2018-11-13
 *
 * @author Henrik von Coler
 *
**/


#ifndef EXPONENTIALDECAY_H
#define EXPONENTIALDECAY_H

#include <cmath>
#include <random>

#include "Trajectory.h"


class ExponentialDecay : public Trajectory
{

private:

    /// \brief lambda
    ///
    /// - 0.05 is c-ose to zero at x =100
    double lambda;

    /// @brief
    /// Length of the release in samples.
    int length;

    std::default_random_engine generator;

    ///
    std::normal_distribution<double> distribution;

public:

    ExponentialDecay();

    void set_parameters(int L, double lambda);

    /// @brief
    /// Set statistical parameters of normal ditribution.
    void set_statistical_parameters(double mean, double std);

    /// @brief
    /// generate a lambda from the previoulsy set mean and std
    void generate_lambda();

    double get_next_value();

    double get_value_at(double pos);

    double get_length()
    {
        return length;
    }


};

#endif // EXPONENTIALDECAY_H
