/**
 *
 * @file StochasticSound.cpp
 * @class StochasticSound
 *
 * @brief Reads and holds the (stochastic) data related to one sound
 *
 * @date 2017-03-11
 *
 * @author Henrik von Coler
 */

#ifndef STOCHASTICSOUND_H
#define STOCHASTICSOUND_H

#include <yaml-cpp/yaml.h>
#include <string>

#include "glooo.h"
#include "StatisticalParameter.h"
#include "ExponentialDecay.h"
#include "LinearSlope.h"

using namespace glooo;

class StochasticSound
{

public:

    ///
    /// \brief StochasticSound
    /// \param node A YAML node of one data file for one sound
    /// \param fileName The filename of the sound
    ///
    StochasticSound(YAML::Node node, string fileName);

    ///
    /// \brief get_stochastic_parameter returns a random value from a stochastic parameter
    ///
    /// \param type get either amplitude, frequency of band-energy
    /// \param partIDX the partial index to get the value from
    /// \param rand a single random value from a stochastic paramter
    ///
    /// \return a single random value
    ///
    double get_stochastic_parameter(param_type_e type, int partIDX, double last_value, double rand);

    ///
    /// \brief get_mean_parameter
    ///
    /// \param type s get either amplitude, frequency of band-energy
    /// \param partIDX the partial index to get the value from
    ///
    /// \return the mean value from a distribution
    ///
    double get_mean_parameter(param_type_e type, int partIDX);

    /// @todo these individual setters are probably a bad solutions

    double get_amp_att(double pos, int partCNT);

    double get_amp_rel(double pos, int partCNT);

    /*
    ///
    /// \brief return the value of a linear attack trajectory
    /// for a partials amplitude
    /// \param pos the postition (index) within the attack
    /// \param partIDX the relevant partial number
    /// \return
    ///
    // double get_lin_amp_att(double pos, int partIDX);
    */

    /*
    ///
    /// \brief return the value of a linear release
    /// trajectory for a partials amplitude
    /// \param pos the postition (index) within the attack
    /// \param partIDX the relevant partial number
    /// \return
    ///
     double get_exp_amp_rel(double pos, int partIDX);
    */

    double get_freq_att(double pos, int partCNT);
    double get_freq_rel(double pos, int partCNT);

    double get_enrg_att(double pos, int bandIDX);
    double get_enrg_rel(double pos, int bandIDX);


    int attack_length();

    ///
    /// \brief release_length
    /// getter for the length of the release
    /// \return length of the release in frames
    ///
    int release_length();

    std::vector<ExponentialDecay>* get_release_model();

private:

    void fill_trajectories(std::vector<std::vector<double> > &A, YAML::Node &n, string type);



    glooo_parameters *gp;

    string file_name;

    ///
    /// \brief start index of the attack
    int att_start;

    ///
    /// \brief att_stop
    /// stop index of the attack
    int att_stop;

    ///
    /// \brief rel_start
    /// start index of the release
    int rel_start;

    ///
    /// \brief rel_stop
    /// stop index of the release
    int rel_stop;

    ///
    /// \brief l_attack
    ///  length of the attack [in seconds]
    int l_attack;

    ///
    /// \brief l_release
    /// length of the release [in seconds]
    int l_release;

    /// The number of partials used
    int nPartials;

    /// the number of frequency bins
    int nBands;

    /// \brief partial_amplitudes_STAT
    /// A vector of statistical parameters
    /// for the partial amplitudes.
    std::vector<StatisticalParameter> partial_amplitudes;

    /// \brief partial_frequencies_STAT
    /// A vector of statistical parameters
    /// for the partial frequencies.
    std::vector<StatisticalParameter> partial_frequencies;

    /// \brief noise_energies_STAT
    /// A vector of statistical parameters
    /// for the noise band energies.
    std::vector<StatisticalParameter> noise_energies;

    /// \brief partial_frequencies_REL
    /// A vector of statistical release parameters.
    std::vector<ExponentialDecay> freq_release;





};
#endif // STOCHASTICSOUND_H
