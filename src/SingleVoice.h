/**
 *
 *  @file SingleVoice.h
 *  @class SingleVoice
 *
 *  @brief Class for a single voice!
 *
 *  @author Henrik von Coler
 *
 *  @version  0.85
 *
 *  @date   2014/06/02
 *
 */

#ifndef SINGLEVOICE_H
#define SINGLEVOICE_H

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <complex>
#include <jack/jack.h>

#include "glooo.h"
#include "settings.h"
#include "Sinusoid.h"
#include "DeterministicData.h"
#include "StochasticData.h"
#include "GlobalMath.h"
#include "LinearSlope.h"
#include"HyperbolicSlope.h"
#include "GloooConfig.h"

#include "TextOutputManager.h"
#include "ResidualSynth.h"
#include "BarkCoefficients.h"

using namespace  std;

class SingleVoice
{

public:

    ///
    /// \brief SingleVoice
    /// \param resamp the resampling factor - unused
    /// \param bs the blocksize, related to the jack-buffer
    /// \param DR1 A deterministic data set
    /// \param DR A stochastic data set
    /// \param BC A bark coefficient object for initializing the noise filters
    /// \param gc a shared pointer to the glooo config object
    /// \param GP a shared pointer to the glooo parameter struct
    /// \param tom  a shared pointer to the text output manager object (for debugging)
    /// \param GM a pointer to the global math object
    ///
    SingleVoice(int id,
                int fs,
                int bs,
                std::shared_ptr<DeterministicData> DR1,
                std::shared_ptr<StochasticData> DR,
                std::shared_ptr<BarkCoefficients> BC,
                GloooConfig *gc,
                glooo_parameters *GP,
                GlobalMath *GM);


    ///
    /// \brief activate is used when new notes are triggered.
    /// All settings for the new sound are made here.
    /// \todo for attack-onsets only, now
    ///
    /// \param note a glooo note event
    ///
    void activate(note_event_t note, articulation_style style);

    /// @brief fully 'deactivate' a note (after release is finished)
    void deactivate();

    /// The destructor trigger printing of the
    /// sinusoidal data to the text file (TextOutputManager).
    ~SingleVoice();


    ///
    /// \brief getNextBlock_FD
    /// reduntant legacy method for frequency domain synthesis
    /// \param blocksize
    /// \param hopsize
    ///
    void getNextBlock_FD(int blocksize, int hopsize);



    /// \brief cycle_start_deterministic
    ///        prepare a frame using the deterministic model
    void cycle_start_deterministic();



    /// \brief cycle_start_stochastic
    ///  prepare a frame using the deterministic model
    void cycle_start_stochastic();



    /// \brief cycle_proceed
    /// called after synthesizing a frame (cycle_start_stochastic)
    ///
    /// - inceremtents counters
    /// - manages states according to segment lengths
    void cycle_proceed();



    /// @brief Get the next sample for all partials and residual!
    ///
    /// - loops over all partials and noise bands of this voice
    /// and sums them up for a single output value.
    ///
    /// @param **out 2d jack buffer array
    /// @param samplePOS The sample position in the output buffer to work on
    ///
    /// @return a vector of next samples (one for each partial)
    ///
    void get_next_block_bark(jack_default_audio_sample_t **out, uint32_t samplePOS);

    /// @brief
    ///
    /// Get next block for the uniform distribution:
    ///
    /// - All outputs receive the complete signal.
    ///
    /// @param **out 2d jack buffer array
    /// @param samplePOS The sample position in the output buffer to work on
    void get_next_block_uniform(jack_default_audio_sample_t **out, uint32_t samplePOS);

    /// @brief
    ///
    /// Get next block for the discrete dispersion.
    ///
    /// - Each partial and noise band is assigned
    ///   an individual output.
    ///
    /// @param **out 2d jack buffer array
    /// @param samplePOS The sample position in the output buffer to work on

    void get_next_block_discrete(jack_default_audio_sample_t **out, uint32_t samplePOS);


    ///
    /// \brief start_transition
    ///        called when a (glissando) transition
    ///        is started
    /// \param as
    /// \param note the 'midi-note' to the new transition
    ///
    /// \details
    ///
    /// sets the parameters for all partial (and noise band)
    /// transition parameters
    ///
    void start_transition(articulation_style as, note_event_t note);

//    int getNoteNumber();

//    int getVelocity();


    /// \brief set_released
    ///
    /// switches note to 'released' state when a
    /// note off message is received.
    ///
    /// \param as articulation style has influence on the release
    ///
    void set_released(articulation_style as);

    std::complex<double> getOUTSample_FD(int pos);

    bool outOfBounds();

    /// @brief Get amplitude value at a position of the attack segment.
    double getAttackEnvelopeAt(int pos);

    /// @brief Get amplitude value at a position of the release segment.
    double getReleaseEnvelopeAt(int pos);

    bool is_active();

    bool is_released();

    void volume(double v);

    void random_depth(double r);

    void sms_rate(double r);


    void set_pitch(double pb);

    void set_intensity(double intens);

    int get_id();

    int get_rank();

    void increase_rank();

    void set_rank(int rank);

    glooo::note_state get_state();




private:


    /// each initialized voice has an individual id,
    /// counting from 0 upwards:
    int ID;

    ///
    /// \brief rank The rank encodes the relative age of the voice.
    /// New voices are initialied with rank=0.
    /// The rank is inrcemented when new voices are activated.
    /// Inactive voices have a rank >= the maximum number of voices.
    int rank;

    GlobalMath *globMat;

    // TextOutputManager *textOutMan;
    TextOutputManager  *partial_printer ;

    glooo_parameters *gp;


    /// The sampling rate.
    int fs;
    int fs_2;   // precalc 1/2 rate for performance

    ///
    /// \brief state
    /// the glooo::note_state of this voice
    glooo::note_state state;

    /// \brief intensity
    /// the recent intensity used for every frame's
    /// synthesis, updated from the interface's input.
    double intensity;

    /// \brief pitch
    /// the recent pitch used for every frame's
    /// synthesis, updated from the interface's input.
    double pitch;

    double randomDepth;

    /// @brief The sampling rate of the sinusoidal sustain-data
    double smsRate;

    /// The postion within the
    /// deterministic sinusoidal
    double smsPOS;

    /// @brief
    /// A pointer to the release model of a stochastic sound.
    std::vector<ExponentialDecay> * current_release_model;

    /// The position within the release segment.
    double releasePOS;

    /// position within the transition segment
    double transitionPOS;

    /// The position within the attack segment
    double attackPOS;

    /// @brief Sets volume for the complete voice.
    /// Directly controlled via the global volume.
    double vol;

    GloooConfig *gloooconf;

    /// File path of the sample used by this instance.
    std::string  *fileName;

    //  The SingleSample object used by this instance.
    //SingleSample *x;

    //  The partial trajectories for this sample.
    //SinusoidalSampleModel *SSM;






    /// Note number of the object.
    /// - should be set when triggered
    ///   to remember the onset note value
    // int nn;

    /// Velociy value of the object
    /// - should be set when triggered
    ///   to remember the onset note value

    // int vel;

    /// @variable The fundamental fequency
    double f0;

    int blocksize;

    int max_partials;

    /// @brief The number of (Bark)bands used for noise synthesis.
    int nBands;

    /// The current sample position within the wave data.
    double currSampPos;

    /// @variable The current position in seconds
    double currTime;

    int lTrans;

    /// Length of the release ///@todo Move all this to the expressive model?
    int lRelease;

    /// length of the attack segment:
    int lAttack;

    /// The next block to transform by FFT.
    double *frame;

    /// @variable The next block after the FFT.
    std::complex<double>* FRAME;

    ///
    /// \brief partials
    /// - Vector of sinusoids used for the acual sound synthesis
    ///
    std::vector<Sinusoid*> partials;

    /// This stream may be used to print out partial (amplitude) trajectories
    std::ofstream debugOutput;

    ///
    /// \brief stochastic_data
    /// - pointer to the stochastic data,
    /// owned by the jackclient
    std::shared_ptr<StochasticData> stochastic_data;

    ///
    /// \brief determinisctic_data
    /// - pointer to the deterministic data,
    /// owned by the jackclient
    std::shared_ptr<DeterministicData> determinisctic_data;

    std::shared_ptr<BarkCoefficients> bark_coefficents;

    ///
    /// \brief res_synth
    /// - manages all noise related stuff.
    ///
    std::shared_ptr<ResidualSynth> res_synth;

    ///
    /// \brief target_amplitudes
    /// - set in stochastic mode for the attack trajectiories as a target.
    ///
    vector<double> target_amplitudes;


    ///
    /// \brief target_energies
    ///
    vector<double> target_energies;


    ///
    /// \brief offset_amplitudes
    /// - set in stochastic mode as departure values of the release trajectories.
    vector<double> offset_amplitudes;

    /// one amplitude for each band
    /// - used in loop over all partials
    ///   for generating an individual gain
    vector<double> band_amps;


    //std::vector<ExponentialDecay> part_amp_rel_exp;
    std::vector<LinearSlope> part_amp_att_lin;

    HyperbolicSlope *pitch_glissando;

    vector<HyperbolicSlope> part_amp_transitions;
    vector<HyperbolicSlope> part_freq_transitions;
    vector<HyperbolicSlope> band_enrg_transitions;


    double generate_random();

};

#endif // SINGLEVOICE_H
