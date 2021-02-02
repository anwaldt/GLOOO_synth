/**
  *
 * @file ResidualSynth.cpp
 * @class ResidualSynth
 *
 *
 *  @brief ResidualSynth is the filterbank for noise synthesis.
 *
 * - One ResidualSynth per voice
 * - ResidualSynth holds all IIR filters
 *
 *  @date  2018-11-11
 *  @author   Henrik von Coler
**/

#ifndef RESIDUALSYNTH_H
#define RESIDUALSYNTH_H


#include <vector>

#include "GloooConfig.h"
#include "StochasticData.h"
#include "BarkCoefficients.h"
#include "IirFilter.h"
#include "frameinterpolator.h"

using namespace std;

class ResidualSynth
{

public:



    ResidualSynth(glooo_parameters *GP, std::shared_ptr<BarkCoefficients> BC,std::shared_ptr<StochasticData> SD, std::shared_ptr<DeterministicData> DD);


    /// @brief: Prepare the next noise frame
    ///
    /// - get a new support value for the energies of all bands
    ///   by random generation
    /// - set the frame amplitude interpolatorss
    ///
    /// @param pitch: The pitch read from the OSC input
    /// @param intensity: The intensity read from the OSC input
    ///
    void prepare_frame_stochastic(double pitch, double intensity, int nBuff);


    ///
    /// \brief prepare_frame_determinisitc
    ///        is called for each buffer,
    ///        initializes the frame interpolator.
    ///
    /// \param pitch
    /// \param intensity
    /// \param position
    /// \param nBuff
    ///
    void prepare_frame_deterministic(double pitch, double intensity, double position, int nBuff);

    ///
    /// \brief prepare_frame_attack
    /// get the next support point for trajectories
    /// of thenoise band energies, based on the original
    /// trajectories.
    ///
    /// \param pitch
    /// \param intensity
    /// \param position
    /// \param nBuff
    ///
    void prepare_frame_attack(double pitch, double intensity, double position, int nBuff, vector<double> target);

    /// @brief generate one sample of noise betwen -1 and 1
    // and store it in this object
    void new_random();


    /// @brief: get next noise value for one band
    ///
    /// - get a new support point for the energies of all bands
    /// - set the frame amplitude interpolatorss
    ///
    /// @param IDX: the index of the band to process
    ///
    /// @return the output value
    double get_next_value(int IDX);

    double band_energy(int IDX);

private:

    std::shared_ptr<StochasticData>     stochastic_data;
    std::shared_ptr<DeterministicData>  deterministic_data;
    std::shared_ptr<BarkCoefficients>   bc;

    double randBuff;

    double* out_values;

    IirFilter *filters;

    /// @variable An array of frame interpolators (one for each band)
    FrameInterpolator *frameEnrgInt;


    vector <double> lastEnergies;

    glooo_parameters *gp;

};

#endif // RESIDUALSYNTH_H
