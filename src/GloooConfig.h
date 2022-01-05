/**
*
* @file GloooConfig.h
* @class GloooConfig
 *
 * @brief Class for a reading configs from YAML
 *        and holding them.
 *         @todo GloooConfig is intended ONLY for the
 *               parameters which do not change after
 *               startup - dynamic parameters are handeled
 *               in the glooo_parameters struct!
 *
 * @author Henrik von Coler
 *
 * @version  0.51
 *
 * @date   2017-11-02
 *
 */

#ifndef GLOOOCONFIG_H
#define GLOOOCONFIG_H

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>

using namespace std;

class GloooConfig
{
private:


    ///
    /// \brief max_partials
    /// the maximum number of partials
    /// in the data set - not to be changed
    /// during runtime
    int max_partials;

    int num_voices;

    //! The synth mode
    /*!
      can be either
        - "deterministic"
        - "stochastic"
    */
    string synth_mode;

    string model_path;

    string sms_path;

    string yamlPath;

    string assignmentFile;

    string barkFile;

    string attack_mode;

    string release_mode;

    string sine_mode;

    int rec_osc;

    int rec_midi;

    int oscInPort;

    int midiInPort;

    bool printPartials;
    bool printControl;
    bool printStochastic;

    string partial_output_path;

public:

    GloooConfig(const char *modelPath, const char *configFile);  

    /// @brief Getter for the number of partials
    int get_max_partials();

    /// @brief Getter for the number of voices (polyphony)
    int N_voices();

    string get_synthesis_mode();

    string SMS_path();


    string yaml_path();

    string assignment_file();

    string bark_file();


    string ATT_mode();


    string REL_mode();

    string sineMode();

    /// @brief Getter for the osc input port
    int osc_in_port();

    int receive_osc();

    void set_osc(int);

    int midi_in_port();

    int receive_midi();
    
    void set_midi(int);

    bool print_partials();
    bool print_control();
    bool print_stochastic();


    string partOutPath();
    
    // Should be private and accessed by getter method

    int masterVol_CC;

    int tonalVol_CC;

    int noiseVol_CC;   


};

#endif // GLOOOCONFIG_H
