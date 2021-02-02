/**
  *
 * @file settings.h
 * All constants of Synthesizer are saved in this Document
 *
 * @todo This is a shitty solution – pass config file to synth!
**/

#ifndef SRC_SETTINGS_H_
#define SRC_SETTINGS_H_

#include "glooo.h"

/**
 * @brief Activates the test mode, which uses Synth together with TestClient
 */
const bool TEST_MODE = 0;

/**
 * @brief The number of frames to be synthesized, when main() is called in test mode
 */
const uint NUM_TEST_FRAMES = 200;

/**
 * @brief The default sample rate, which is used when no extern sample rate is given.
 */
const uint DEFAULT_SAMPLE_RATE = 44100;

/**
 * @brief The default frame size, which is used when no extern frame size is given.
 */
const uint DEFAULT_FRAME_SIZE = 256;

/**
 * @brief The synthesis window size, should be equal to the analysis window hop size
 */
const uint WINDOW_SIZE = 256;

/**
 * @brief The number of notes to precalculate in the frequency table
 */
const uint NUM_NOTES = 128;

/**
 * @brief Number of partials to be read from the analysis files
 */
//const uint NUM_PARTIALS = 30;

/**
 * @brief Number of velocity files per
 */
const uint NUM_VELOCITIES = 4;

/**
 * @brief Defines wether to use a sine lookup table or not
 */
const bool SINE_LOOKUP_TABLE = 1;

/**
 * @brief Defines the number of bits to be used for the index of the lookup table
 */
const uint SINE_RESULUTION = 16;

/**
 * @brief If true, starting phases of a voice are selected randomly
 */
const bool RANDOM_START_PHASE = 1;

/**
 * @brief If false, Voice uses stochastic selection of partial tone data for the stationary part of the tone
 */
//const bool STATIC_MODE = 1;

/**
 * @brief The tuning frequency
 */
const double CONCERT_PITCH = 440.0;

/**
 * @brief Frequency interpolation method on velocity axis
 */
const interp_type_e FREQ_INTERP_VEL_AXIS = lin;

/**
 * @brief Frequency extrapolation method on velocity axis
 */
const interp_type_e FREQ_EXTRAP_VEL_AXIS = extrap_const;

/**
 * @brief Frequency interpolation method on pitch axis
 */
const interp_type_e FREQ_INTERP_PITCH_AXIS = logarithmic;

/**
 * @brief Frequency extrapolation method on pitch axis
 */
const interp_type_e FREQ_EXTRAP_PITCH_AXIS = extrap_log;

/**
 * @brief Amplitude interpolation method on pitch axis
 */
const interp_type_e AMP_INTERP_PITCH_AXIS = lin;

/**
 * @brief Amplitude extrapolation method on pitch axis
 */
const interp_type_e AMP_EXTRAP_PITCH_AXIS = extrap_const;

/**
 * @brief Amplitude interpolation method on velocity axis
 */
const interp_type_e AMP_INTERP_VEL_AXIS = logarithmic;

/**
 * @brief Amplitude extrapolation method on velocity axis
 */
const interp_type_e AMP_EXTRAP_VEL_AXIS = extrap_log;

/**
 * @brief Amplitude extrapolation exponent
 */
const double AMP_EXTRAPOLATION_EXP = 1;

/**
 * @brief Frequency extrapolation exponent
 */
const double FREQ_EXTRAPOLATION_EXP = 1/12;

/**
 * @brief Interpolation method for the inverse transform sampling
 */
const interp_type_e CDF_VAL_INTERP = lin;

/**
 * @brief If true, the synthesizer accepts events passed by its wrapper
 */
const bool USE_WRAPPER_EVENTS = 1;


/**
 * @brief If true, the synthesizer accepts OSC events
 */
const bool USE_OSC = 0;


/////////////// Controller settings //////////////////


/**
 * @brief A factor to scale the input pitch
 */
const double pitch_factor = 1;

#endif
