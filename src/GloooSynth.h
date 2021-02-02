/**
 *  @file GloooSynth.h
 *  @class GloooSynth
 *
 *  @date 2016-03-02
 *  @authors  Henrik von Coler, Benjamin Wiemann
 *
 **/

#ifndef JACKCLIENT_H_
#define JACKCLIENT_H_


#include <jack/jack.h>
#include <jack/midiport.h>

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <csignal>
#include <vector>
#include <memory>
#include <limits>

#include <cmath>


#include "glooo.h"
#include "settings.h"
#include "DeterministicData.h"
#include "StochasticData.h"
#include "VoiceManager.h"
#include "GloooConfig.h"
#include "TextOutputManager.h"
#include "BarkCoefficients.h"




/**
 * @brief Serves as a layer between Jack and the Synthesizer and translates Midi Messages.
 *
 */

class GloooSynth {

public:

    GloooSynth(const char* modelPath, const char* configFile);

    virtual ~GloooSynth();

	/**
	 * @brief The process function, which invokes the main synthesis process
	 * @param nframes Number of samples given by
	 * @return
	 */

	int process(jack_nframes_t samples_per_frame);
	jack_client_t* getClient();

	/**
	 * @brief Function to set the frame size of the system
	 * @param frame_size Current frame size
	 *
	 * Only to be invoked by the callback_set_frame_size function.
	 */

    void set_frame_size();

    /**
     * @brief Function to set the sample rate of the system
     * @param sample_rate Current sample rate
     *
     * Only to be invoked by the callback_set_sample_rate function.
     */
    void set_sample_rate(uint sample_rate);

private:

    GlobalMath *GM;

    jack_client_t   *client;
    jack_status_t   status;
	// for midi in:
	jack_port_t     *inPort;
	// for audio out:
    jack_port_t     **outPort;
	size_t nOutputs;
	char* clientName;
    // std::shared_ptr<Synth> synth;
    // valarray<double> output;


    std::shared_ptr<DeterministicData>  deterministic_data;
    std::shared_ptr<StochasticData>     stochastic_data;

    std::shared_ptr<BarkCoefficients> bark_coefficients;

    jack_default_audio_sample_t **out;

    VoiceManager *voice_manager;

    GloooConfig *gloooconf;

    TextOutputManager *textOutMan;

};


/**
 * @brief The process callback function. Calls Jack::process(samples_per_frame),
 * @param samples_per_frame
 * @param object
 * @return
 */
int callback_process(jack_nframes_t x, void* object);

int callback_set_frame_size(jack_nframes_t frame_size, void *object);

int callback_set_sample_rate(jack_nframes_t sample_rate, void* object);

/**
 * @brief Implements actions to be taken when the application is closed.
 */
void exitCallback();

void jackShutdown (void *arg);

 #endif /* JACKCLIENT_H_ */
