/**
 *
 * @file DeterministicData.h
 *
 * @class DeterministicData
 *
 * The container class for all deterministic sounds.
 *
 *  @date  2016-06-02
 *
 *  @authors   Benjamin Wiemann, Henrik von Coler
 *
**/

#ifndef SRC_DATAREADER_H_
#define SRC_DATAREADER_H_

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
#include "DataSet.h"

using namespace std;
using namespace boost;
using namespace glooo;


/**
 * @brief Loads and holds all partial tone data
 */
class DeterministicData: protected DataSet{
public:

    DeterministicData();
    virtual ~DeterministicData();

	/**
     * @brief Loads csv-files with matrices used for deterministic synthesis
	 * @param file_dir		The directory in which the csv-folder is located
	 * @param num_partials 	Number of partials to be read
	 * @param min_note		Minimum note to be read. If equal zero, it will be ignored
	 * @param max_note		Minimum note to be read. If equal zero, it will be ignored
	 */
    void load_deterministic(string file_dir, string assignment_file, uint num_partials);


	uint get_num_partials();

	void set_frame_size(uint frame_size);



    ///
    /// \brief Returns a single trajectory value from the sound
    ///        realted to pitch, intensity and ...
    ///
    /// \param partialID    The id of the partial to get the value for!
    /// \param noteNumber   The  midi note number to get the value for!
    /// \param velocity     The  midi velocity to get the value for!
    /// \param idx          The temporalidx (frame) to get the value for!
    /// \return             A double amplitude value!

    double get_trajectory_value(int partialID, int noteNumber, int velocity, double idx, param_type_e TYPE);


private:

    /// @brief Replaces the map structures
    vector<vector <DeterministicSound>> sound_matrix;



	/**
	 * @brief Reads values of a specific file and stores it into a voice matrix
     * @param file_path the path for the data
	 * @param num_partials The number of partials to be read.
     *
	 * @return A matrix containing partial vectors
	 */
    voice_matrix_t read_matrix(string file_path, uint num_colums);

	string file_directory;
	uint frame_size;



};

#endif /* SRC_DATAREADER_H_ */
