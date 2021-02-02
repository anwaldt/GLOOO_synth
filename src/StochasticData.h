/**
 *
 * @file StochasticData.cpp
 * @class DeterministicData
 *
 * @brief Reads and manages the stochastic data sets
 *
 * - holds CDFs for partial parameters and noise band-energies
 * - holds trajectories for attack and decay of sinusoids and noise
 * - provides methods for picking samples from the data
 *
 * @date 2017-03-11
 *
 * @authors Benjamin Wiemann, Henrik von Coler
 */




#ifndef STOCHASTICDATAREADER_H_
#define STOCHASTICDATAREADER_H_

#include <pthread.h>

#include "DataSet.h"
#include "DeterministicData.h"
#include "GloooConfig.h"
#include "StochasticSound.h"

/**
 * @brief This class extends DataReader and reads and provides stochastic interpolation data
 */
class StochasticData: public DataSet{

public:

    StochasticData(GloooConfig *GC);
    virtual ~StochasticData();

    /**
     * @brief Loads yaml-files with hierachical organized data used for stochastic synthesis
     */
    void load_stochastic(string assigment_file, uint num_partials);






//    /// @brief Return a value from a set of partial trajectories
//    ///
//    /// used for the original attack and decay segments
//    ///
//    double get_partial_val(glooo::note_state state, param_type_e type, int partialID, int noteNumber, int velocity, double idx);

//    /// @brief Get the length of a partial trajectory set
//    ///
//    int get_trajectory_length(glooo::note_state state, int noteNumber, int velocity);

    /// @brief return a single random value, according to a note number and velocity
    ///
    /// - maps MIDI values to array indices
    /// - interpolates between 4 values in the 'timbre space'
    ///
    /// @param note: The MIDI note value - can be fractional
    /// @param velocity: The MIDI intensity value - can be fractional
    /// @param IDX: The index of the partial or noise band to get
    /// @param type: Get value from either amplitude, frequncy or energy distribution
    /// @param mode: Chose between actual random sampling or simply picking the median
    ///
    double get_stochastic_value(double note, double velocity, uint IDX, param_type_e type, glooo::stat_mode mode, double last_value);

    ///
    /// \brief get_mean_value
    ///
    /// Manages the generation of
    ///
    /// \param note
    /// \param velocity
    /// \param IDX
    /// \param type
    /// \return
    ///
    double get_mean_value(double note, double velocity, int IDX, param_type_e type);


    ///
    /// \brief get_trajectory_value
    ///        get the value from a trajectory defined by the parameter set
    ///
    /// \param pitch
    /// \param velocity
    /// \param partCNT
    /// \param pos
    /// \param type
    /// \param mode
    /// \return
    ///
    double get_trajectory_value(double pitch , double velocity, int partCNT, double pos, param_type_e type, glooo::note_state mode);

    /// \brief sound returns the pointer to a stochastic sound,
    ///         defined by note number and intensity
    ///
    /// \param nn
    /// \param vel
    /// \return
    ///
    StochasticSound &sound(int nn, int intensity);

    /// \brief Returns the nearest sound in the timbre plane,
    ///        given fractional values for pitch and intensity.
    /// \param double nn
    /// \param double intensity
    /// \return
    ///
    StochasticSound &nearest_sound(double nn, double intensity);

    /// \brief velocity_to_index
    /// \param velocity A double representing a fractional  MIDI value
    /// \return A fractional index ranging from 0 to 3 for accessing the 4 dynamic levels
    ///
    double velocity_to_index(double velocity);




private:

    GloooConfig *gc;

    /// @brief Replaces the map structures of the previous version
    vector<vector <StochasticSound>> sound_matrix;

    double generate_random()
    {

        double val = (double) rand() / (double)  numeric_limits<int>::max();

        return val;
    }

    ///
    /// \brief part_amp_buffer
    /// Used for moving average filter (deprecated).
    double part_amp_buffer[4];

    ///
    /// \brief part_freq_buffer
    /// Used for moving average filter.
    double part_freq_buffer[4];

    double bark_energy_buffer[4] = {0, 0, 0, 0};

    double A, B, C, D;




};

#endif /* STOCHASTICDATAREADER_H_ */
