/**
  *
 * @file DataSet.h
 *
 * @class DataSet
 * @brief  Basic class for data sets to hold
 * instrument sounds.
 *
 *  @date  2016-06-02
 *
 *  @authors Henrik von Coler
 *
**/

#ifndef DATASET_H
#define DATASET_H

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

#include <sys/types.h>
#include <boost/tokenizer.hpp>
#include <yaml-cpp/yaml.h>

#include "glooo.h"
#include "settings.h"
#include "DeterministicSound.h"

class DataSet
{


public:
    DataSet();

    /// \brief pitch_to_index
    /// \param pitch A MIDI pitch value
    /// \return The corresponding index in the array structures
    ///
    int pitch_to_index(int pitch);

    /// \brief intensity_to_index
    /// \param intensity
    /// \return
    ///
    int intensity_to_index(double intensity);

    /**
     * @brief Returns the lowest stored note
     * @return
     */
    uint get_lowest_note();

    /**
     * @brief Returns the highest stored note
     * @return
     */
    uint get_highest_note();




protected:


    /// @todo We need pointers to the attack and release segments!

    uint num_partials;
    uint min_note;
    uint max_note;

    ///
    /// \brief file_to_note_map
    /// a map assigning each note a file
    std::shared_ptr<map<uint, vector<string>>> file_to_note_map;

    /**
     * @brief Reads the assignment file
     * @return A map with note numbers as keys and a vector with four file names as values.
     *
     * These four files contain the partial data for different velocities
     */
    map<uint, vector<string>> get_file_to_note_assignment(string file_path);
};

#endif // DATASET_H
