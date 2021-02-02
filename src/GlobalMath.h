/**
 *  @class GlobalMath
 *
 * @brief This class offers some computations which are supposed to be executed just once to save computation power.
 *
 *  @date 29.08.2016
 *  @author Benjamin Wiemann
 */

#ifndef SRC_GLOBALMATH_H_
#define SRC_GLOBALMATH_H_

#include <vector>
#include <valarray>
#include <memory>
#include <cmath>
#include <iostream>
#include <map>
#include <csignal>
#include <random>
#include <chrono>

#include "settings.h"

using namespace std;

class GlobalMath {
public:


    GlobalMath(uint frame_size, uint sample_rate, unsigned seed);

    GlobalMath(uint frame_size, uint sample_rate);

    virtual ~GlobalMath();

    /**
     * @brief Sets the current sample rate
     *
     * @param sample_rate
     */
    void set_sample_rate(uint sample_rate);
    void set_frame_size(uint frame_size);

    /**
     * Returns the frequency for a note
     * @param note
     * @return
     */
    double get_note_frequency(int note);

    ///
    /// \brief midi_to_hz
    /// \param midi_pitch
    /// \return
    /// @todo: Don't use static here!
    static double midi_to_hz(double midi_pitch);


    /**
     * @brief Initializes a time vector for one frame. The resulting array is of size frame_size + 1.
     * @return The time vector
     */
    void init_time_vector();
    valarray<double> compute_window();

    /**
     * @brief Looks up a valarray of phases and returns a valarray which is one sample smaller than the input argument.
     *
     * @param arg Phase information
     * @param return_size The number of values selected from arg and size of the return array
     *
     * @return	Sine values
     */
    valarray<double> sin_lookup(valarray<double> arg, uint return_size);


    double single_sin_lookup(double arg);

    /**
     * @brief Returns a valarray of the current frame size containing the time variable.
     * @return
     *
     * Needed for phase interpolation. Distance between 2 time samples is 1/sampling_rate.
     */
    const valarray<double>& get_time() const;

    /**
     * @brief
     *
     * @return a valarray of the current frame size containing the quadratic time variable.
     *
     * Needed for quadratic phase interpolation.
     *
     */
    const valarray<double>& get_quad_time() const;

    int get_sample_rate();
    int get_frame_size();

    /**
     * @brief Returns a random number generator
     *
     * @return
     */
    mt19937& get_generator();




private:

    vector<double> frequency_table;
    valarray<double> time_vector;
    valarray<double> time_vector_quadratic;
    valarray<double> window;
    //	map<int, double> sin_table;
    vector<double> sin_table;


    void init_frequency_table();
    void init_sine_table();

    /**
     * @brief A random number generator
     */
    mt19937 random_generator;


    int sample_rate = -1;
    int frame_size = -1;


};

#endif /* SRC_GLOBALMATH_H_ */
