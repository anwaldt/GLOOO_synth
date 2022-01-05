
    #include "GloooConfig.h"

    GloooConfig::GloooConfig(const char *modelPath, const char *configFile) //!< Path to YAML file
    {

        // Read basic file
        YAML::Node config = YAML::LoadFile(configFile);

        // The output node
        YAML::Node outputs;

        outputs = config["OUTPUTS"];




        model_path          = modelPath;

        sms_path            = model_path+"txt/";
        yamlPath            = model_path+"yaml/";
        assignmentFile      = model_path+config["assignment_file"].as<string>();
        barkFile            = model_path+config["bark_file"].as<string>();
        partial_output_path = outputs["partial_output_path"].as<string>();

        printPartials   = (bool) outputs["partial_trajectories"].as<int>();
        printStochastic = (bool) outputs["statistical_data"].as<int>();
        printControl    = (bool) outputs["control_input"].as<int>();

        max_partials    = config["num_partials"].as<int>();
        num_voices      = config["num_voices"].as<int>();
        synth_mode      = config["synth_mode"].as<string>();
        attack_mode     = config["attack_mode"].as<string>();
        release_mode    = config["release_mode"].as<string>();
        sine_mode       = config["sine_mode"].as<string>();

        cout << "Running in "
             << synth_mode << " mode" << endl << endl;

        if (printPartials == true)
        {
            cout << "Writing partial data to: "
                    << partial_output_path << "" << endl << endl;
        }


        cout << "Using "
             << num_voices << " voice(s) polyphony" << endl << endl;

        cout << "Using "
             << max_partials << " partial(s) per voice" << endl << endl;

        cout << "Reading sinusoidal data from: "
             << sms_path <<  endl << endl;

        cout << "Attack mode: "
             << attack_mode <<  endl << endl;


        midiInPort   = config["midi_port_in"].as<int>();
        rec_midi     = (bool) config["receive_midi"].as<int>();
        oscInPort    = config["osc_port_in"].as<int>();
        rec_osc      = (bool) config["receive_osc"].as<int>();

        masterVol_CC = config["CC_master_volume"].as<int>();
        tonalVol_CC = config["CC_tonal_volume"].as<int>();
        noiseVol_CC = config["CC_noise_volume"].as<int>();

    }

    string GloooConfig::partOutPath()
    {
        return partial_output_path;
    }


    int GloooConfig::get_max_partials()
    {
        return max_partials;
    }

    int GloooConfig::N_voices()
    {
        return num_voices;
    }


    string GloooConfig::get_synthesis_mode()
    {
        return synth_mode;
    }



    string GloooConfig::SMS_path()
    {
        return sms_path;
    }



    string GloooConfig::yaml_path()
    {
        return yamlPath;
    }

    string GloooConfig::bark_file()
    {
        return barkFile;
    }


    string GloooConfig::assignment_file()
    {
        return assignmentFile;
    }


    string GloooConfig::ATT_mode()
    {
        return attack_mode;
    }

    string GloooConfig::REL_mode()
    {
        return release_mode;
    }

    string GloooConfig::sineMode()
    {
        return sine_mode;
    }

    int GloooConfig::osc_in_port()
    {
        return oscInPort;
    }

    int GloooConfig::receive_osc()
    {
        return rec_osc;
    }

    void GloooConfig::set_osc(int osc)
    {
        rec_osc = osc;
    }

    int GloooConfig::midi_in_port()
    {
        return midiInPort;
    }

    int GloooConfig::receive_midi()
    {
        return rec_midi;
    }

    void GloooConfig::set_midi(int midi)
    {
        rec_midi = midi;
    }

    bool GloooConfig::print_partials()
    {
        return printPartials;
    }

    bool GloooConfig::print_control()
    {
        return printControl;
    }

    bool GloooConfig::print_stochastic()
    {
        return printStochastic;
    }
