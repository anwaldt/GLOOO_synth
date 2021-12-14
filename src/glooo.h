/**
 *  @file glooo.h
 *
 *  @date  2016-05-19
 *  @authors Henrik von Coler, Benjamin Wiemann
 */

#ifndef GLOOO_H_
#define GLOOO_H_

#include <algorithm>    // min,max
#include <valarray>
#include <vector>
#include <map>
#include <tuple>        //  std::tie
#include <string>    

using namespace std;



namespace glooo
{


/// @brief the articulation styles of the synth
typedef enum{
    legato,
    glissando,
    detached,
} articulation_style;


/// @brief Enumerator for selecting the mode for grabbing values from CDFs
/// @var random: pick a random value by inverse transform sampling
typedef enum{
    random,
    median,
} stat_mode;

/// @brief Synthesis state of a voice
///
/// - used for the state machine within one voice
/// - also used to select between attack and release data by the stochastic data
///
/// @todo make another enumerator for segment types
typedef enum{
    attack,
    sustain,
    release,
    transition,
    inactive,
} note_state;


/// @brief An enumeration for the different control types
typedef enum {
    note_on,       //!< note_on
    note_off,      //!< note_off
    pitch_mod,
    vel_mod
} event_type_e;


/// @brief Parameter type which is either frequency, amplitude or energy.
typedef enum{
    freq,
    amp,
    band_nrg,
    phase,
} param_type_e;



///  @briefused for index interpolation
typedef struct
{
    int lowerSP;
    int upperSP;
    double frac;
} index_interp;

inline glooo::index_interp interpolate(double pos)
{

    index_interp I;

    I.lowerSP = (int) floor(pos);
    I.upperSP = (int) (I.lowerSP + 1);
    I.frac    = pos - (double) I.lowerSP;

    return I;

}


typedef struct
{
    double a =1;
    double mu;
    double sigma;

} gaus_dist;



}

/// @brief Interpolation axis
typedef enum{
    vel_axis,
    pit_axis
} interp_axis_e;

/**
 * @brief Interpolation type
 */
typedef enum{
    closest,
    lin,
    logarithmic,//!< interp_log
    extrap_log,
    extrap_const
} interp_type_e;


typedef enum{
    uniform,
    bark,
    discrete,
} dispersion_mode;


/**
 * @brief A note message container
 */
typedef struct {
    uint note;              //!< The MIDI note value  (0..127)
    glooo::event_type_e type;      //!< The type of MIDI message
    uint time;              //!< The MIDI time stamp
    uint velocity;          //!< The MIDI velocity value @value (0..127)
    //double pitch;
    //double filter_a;
} note_event_t;


/// @brief Struct for communication between
/// voice manager and OSC manager.
///
/// - glooo_parameters holds all synth parameters
///   which can be changed during runtime
///

typedef struct{

    int    fs               = 0;

    // volumes
    double master_vol       = 1.0;
    double tonal_vol        = 1.0;
    double noise_vol        = 1.0;

    // the control parameters
    double pitch            = 0.0;   //!< the MIDI? pitch value sent from the interface, used


    double intensity        = 0;     //!< intensity is the mean pressure of the excitation pad

    ///
    /// \brief pitch_shift
    /// - shift the synth frequency after
    ///   picking partial data
    /// - 'brute force resampling'
    double pitch_shift      = 1.0;

    ///
    /// \brief sms_rate_glob
    /// The global SMS rate is the ratio between
    /// the sampling rate of the analysis data
    /// and the synthesis framerate.
    double sms_rate     = 4.0;

    double random_depth = 0.1;


    /// the (maximum) number of partials to be
    /// synthesized
    /// - mind: the maximum number of partials
    /// in the data set is: GloooConfig::max_partials
    int n_partials = 40;

    glooo::articulation_style  articulation_mode = glooo::legato;

    double active_note      = 0;    //!< the note value, updated from the controller

    ///
    /// \brief is_playing
    ///  boolean - is true, if there is a note is beeing played on the control interface
    bool is_playing          = false;

    float intensity_thresh  = 2;   //!< threshold for activating and releasing notes
    /// @todo might be two: one for onset and offset

    ///
    /// \brief part_amp_mode
    /// the synthesis mode for partial amplitudes
    /// - "r" = random
    /// - "m" = mean / median mode
    string part_amp_mode           = "r";

    ///
    /// \brief disp_mode
    /// the dispersion mode, used for routing
    /// partial and noise components to the
    /// synth outputs.
    dispersion_mode disp_mode = discrete;

    double glissando_duration = 100;
    double glissando_gamma    = 1.0;

    /// the mode for generating the partial frequencies
    /// - 's':
    ///
    string part_freq_mode   = "s"; // strict
    //    string part_freq_mode   = "m"; // mean
    string noise_energy_mode = "m";

    ///
    /// \brief printControl
    /// Print control data
    bool printControl       = true;
    bool printStochastic    = false;
    bool shutwown           = false;

    ///
    /// \brief partial_activation
    /// a bitmask, detemining whether a partial at
    /// a certain index will be synthesized
    ///
    /// - set by OscManager::partial_activity_handler
    ///
    /// - treated as boolean, yet for simplicity in
    ///   OSC massage handling, integers are used
    vector<int> partial_activation;


} glooo_parameters;


/**
 * @brief A container storing phases, amplitudes
 */
typedef struct {
    valarray<double> frequency;
    valarray<double> amplitude;
} frame_modulator_t;

/**
 * @brief Container type which hold deterministic
 * synthesis data for a defined note and velocity
 *
 */
typedef vector<valarray<double>> voice_matrix_t;

/**
 * @brief Container type which holds stochastic synthesis data.
 *
 * The key corresponds to the index of the current partial tone
 *
 */
typedef vector<map<double, double>> cdf_vector_t;

/**
 * @brief Container with input data to access and interpolate partial vectors
 */
typedef struct{
    uint index;
    double offset;
} mapped_ctrl_t;

/**
 * @brief Container for interpolation square
 */
typedef struct{
    valarray<double> high_vel_high_note;
    valarray<double> high_vel_low_note;
    valarray<double> low_vel_high_note;
    valarray<double> low_vel_low_note;
} interp_square_t;

/**
 * @brief Synthesis result features, used for debugging
 */
typedef struct{
    valarray<double> signal;
    vector<valarray<double>> partial_phase;
    vector<valarray<double>> partial_amp;
} synth_feature_t;



#endif /* SRC_HELPER_H_ */
