/**
 *
 * @file StatisticalParameter.h
 * @class StatisticalParameter
 *
 * @brief Reads and manages a stochastic parameter
 *
 * @date 2018-11-20
 * @version 0.12
 * @author Henrik von Coler
 */

#ifndef STATISTICALPARAMETER_H
#define STATISTICALPARAMETER_H

#include <vector>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort

#include "glooo.h"

class StatisticalParameter
{

public:


    ///
    /// \brief StatisticalParameter Default constructor
    ///
    StatisticalParameter();

    ///
    /// \brief init
    /// \param n  YAML node (iterator)
    /// \param type the type of distribution parameter
    ///
    void init(const YAML::detail::iterator_value n, string type);

    ///
    /// \brief get_value
    /// \param randVal
    /// \return
    ///
    double get_value(double lastVal, double randVal);

    ///
    /// \brief mean
    /// \return
    ///
    double mean();

    ///
    /// \brief median
    /// \return
    ///
    double median();

    ///
    /// \brief std
    /// \return
    ///
    double std();

private:

    ///
    /// \brief type
    ///
    string param_type;

    ///
    /// \brief mean_value
    ///
    double    mean_value;

    ///
    /// \brief median_value
    ///
    double    median_value;

    ///
    /// \brief stddev_value
    ///
    double    stddev_value;

    ///
    /// \brief N_icmf
    /// the number of support points for the ICMF -
    /// avoids the use of the support_points vector.
    int N_icmf;

    ///
    /// \brief support_points
    /// a vector with the support points of the ICMFs
    std::vector<vector<double>> support_points;


    ///
    /// \brief is_valid
    /// boolean vector, set to false (invalid) if the
    /// IFMC at that index is zero only
    std::vector<bool> is_valid;

    ///
    /// \brief dist
    /// a vector of inverted cumulative mass functions (ICMFs)
    std::vector<vector<double>> icfm_collection;

    ///
    /// \brief xval
    /// a vector of x-values, referring to the ICMFs
    std::vector<vector<double>> xval_collection;

    std::vector<double> dist_centers;

    /// @brief stores past values for FIR filtering
    vector<double> out_buff;

    double generate_random()
    {

        double val = (double) rand() / (double)  numeric_limits<int>::max();

        return val;
    }
};

#endif // STATISTICALPARAMETER_H
