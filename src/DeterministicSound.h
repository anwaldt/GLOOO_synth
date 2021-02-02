/**
 *
 * @file DeterministicSound.h
 *
 * @class DeterministicSound
 *
 * @brief  holds a single deterministic sound
 *
 * - makes the partial trajectories from analysis
 *   available through methods
 *
 *  @date  2016
 *
 *  @authors   Benjamin Wiemann, Henrik von Coler
 *
**/


#ifndef DETERMINISTICSOUND_H
#define DETERMINISTICSOUND_H

#include <string>
#include <list>
#include <map>
#include <iterator>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <regex>
#include <memory>
#include <random>


#include <boost/tokenizer.hpp>
#include <boost/token_iterator.hpp>


#include <sys/types.h>

#include "glooo.h"

using namespace std;
using namespace boost;
using namespace glooo;

class DeterministicSound
{
public:

    ///
    /// \brief DeterministicSound
    ///
    DeterministicSound(string directory, string fileName, uint nPart);

    void  fill_trajectories(std::vector<std::vector<double>> &A, string fileName, uint num_columns, string type);

    ///
    /// \brief get_trajectory_value
    /// get the value of a parameter at a certain position
    /// (time-index) and partial/noise index
    ///
    /// \param pos
    /// \param partCNT
    /// \param type
    /// \return
    ///
    double get_trajectory_value(double pos, int partCNT, param_type_e type);

    int length();

private:

    void fill_trajectories();

    glooo_parameters *gp;

    string file_name;


    int nPartials;
    int nBands;

    int L;

    /// the trajectory matrices for the partial amplitudes
    std::vector<std::vector<double>> partial_amplitudes;


    /// the trajectory matrices for the partial frequencies
    std::vector<std::vector<double>> partial_frequencies;

    /// the trajectory matrices for the residual
    std::vector<std::vector<double>> noise_energies;


};

#endif // DETERMINISTICSOUND_H
