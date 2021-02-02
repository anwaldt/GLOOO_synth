/**
*
* @file TextOutputManager.h
* @class TextOutputManager
 *
 * @brief Class for writing sinusoidal and control
 * data to text files.
 *
 * @details All relevant data is stored inside
 * the object during runtime. Data to be recorded
 * is chosen inside the YAML config file. Recorded
 * data is written to separate text files onyl during
 * proper shutdown of the synth.
 *
 * @todo needs to be polyphonic, now
 *
 * @author Henrik von Coler
 *
 * @version  0.31
 *
 * @date   2018/02/05
 *
 */

#ifndef TEXTOUTPUTMANAGER_H
#define TEXTOUTPUTMANAGER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <vector>

#include "GloooConfig.h"

using namespace std;

class TextOutputManager
{

private:


    int ID;

    GloooConfig *gc;

    vector<double> *ampValues;
    vector<double> *freqValues;


    /// @brief Filename for partial amplitudes
    string partial_amplitudes_file;
    /// @brief Output stream for
    /// all partial amplitude trajectories.
    ofstream ampStr;


    /// @brief Filename for partial frequencies
    string partial_frequencies_file;
    /// @brief Output stream for
    /// all partial frequency trajectories.
    ofstream freqStr;

    /// @brief Filename for control data
    string control_inputs_file;
    /// @brief Output stream for
    /// all control data.
    ofstream ctlStr;


public:

    TextOutputManager(GloooConfig *GC, int id);

    /// @todo We REALLY need destructors at some point!!
    ~TextOutputManager();

    /// @brief Write all partial amplitude value to file
    ///
    ///
    void write_amplitude(double amp, bool breakLine);

    void push_amplitude(double amp,  int parIDX);

    void push_frequency(double freq, int parIDX);

    void write_control(double val, bool breakLine);

    void export_to_file(int num_part);

};

#endif // TEXTOUTPUTMANAGER_H
