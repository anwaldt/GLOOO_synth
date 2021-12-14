/**
 * \class VoiceManager
 *
 *
 * \brief Class for managing the voices of the GLOOO synthesizer!
 *
 * This class is responsible for processing the osc messages,
 * creating and deleting voices accordingly.
 * It also combines all active voices' samples for the next
 * synthesis block
 *
 *
 * \author Henrik von Coler
 *
 * \version  0.6
 *
 * \date   2017/10/06
 *
 * von_coler@tu-berlin.de
 *
 *
 */

#ifndef VOICEMANAGER_H
#define VOICEMANAGER_H

#include <vector>

#include <cmath>
#include <complex>


#include <jack/jack.h>
#include <jack/midiport.h>

#include <iostream>

#include "glooo.h"
#include "SingleVoice.h"
#include "StochasticData.h"
#include "DeterministicData.h"
#include "GlobalMath.h"
#include "GloooConfig.h"
#include "OscManager.h"
#include "MidiManager.h"
#include "TextOutputManager.h"
#include "BarkCoefficients.h"

class VoiceManager
{

public:

    bool is_shutdown();

    VoiceManager(jack_default_audio_sample_t **OUT, std::shared_ptr<DeterministicData> DR1, std::shared_ptr<StochasticData> DR , std::shared_ptr<BarkCoefficients> BC, GloooConfig *gc, uint bufferSize, uint sampleRate, TextOutputManager *toM, GlobalMath *GM);

    ~VoiceManager();

    ///
    /// \brief Process all OSC and MIDI messages and update the voices!
    /// \param portBuffer
    ///
    ///         - starts new voices
    ///         - sets voices 'released'
    ///         - kills finished voices

    void update_voices();


    ///
    /// \brief getNextBlock_FD
    /// \param blocksize
    /// \param hopsize
    ///
    void getNextBlock_FD(int blocksize, int hopsize);

    /// @brief Get the next sinusoid values!
    ///
    ///
    /// This method is a LOOP OVER all active voices
    /// to output a new frame using the time domain.
    /// Each voice's sample value is summed up to the
    /// output value 'thisSample'.
    ///
    /// @param **out: The jack out-buffer array
    /// @param n_samples: The jack buffer size
    ///
    void getNextFrame_TD(jack_default_audio_sample_t **out, uint32_t n_samples, int N_out);

    ///
    /// /// @brief This method returns a pointer to the first element of the spectral frame
    /// \return a pointer to the rescent audio sample
    ///
    jack_default_audio_sample_t *recentBlock();

    ///
    /// \brief numberOfVoices
    /// \return returns the number of voices
    ///
    int numberOfVoices();

    //SingleSamplePlayer *getVoice(int number);

    ///
    /// \brief increase_ranks
    /// Incrase the ranks of all voices which are acvtive (rank<num_voices).
    void increase_ranks();

private:

    TextOutputManager *textOutMan;

    glooo_parameters gp;

    GlobalMath *globMat;

    OscManager  *osc_manager;

    MidiManager *midi_manager = nullptr;

    GloooConfig *gloooconf;


    /// This vector holds all voices which are active at that time!
    std::vector<SingleVoice*> voices;


    /// This pointer is used for temporal access of the voices!
    SingleVoice* s = NULL;

    /// This array holds the recent/next frame to synthesize.
    jack_default_audio_sample_t **OUT;

    /// The analysis/synthesis blocksize.
    int blocksize;

    /// The analysis/synthesis hopsize.
    int hopsize;

    std::shared_ptr<StochasticData>     s_data;
    std::shared_ptr<DeterministicData>  d_data;

    std::shared_ptr<BarkCoefficients>   b_coeff;

    GlobalMath *global_math;

    double master_volume;

    double random_depth;


    /// \brief pitch
    /// the recent pitch used for every frame's
    /// synthesis, updated from the interface's input.
    double pitch;

    /// \brief last_pitch
    /// a memory to detect quick pitch changes
    double last_pitch;


    vector<bool> voices_active;

    ///
    /// \brief art_style
    /// when updating voices, the synth checks which
    /// mode to use
    glooo::articulation_style art_style;

   void print_voice_activity();

};

#endif // VOICEMANAGER_H
