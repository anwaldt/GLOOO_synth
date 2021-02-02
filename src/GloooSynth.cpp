/**
 *  @file GloooSynth.cpp
 *  @class GloooSynth
 *
 *  \brief  Central synth class, acting as JACK client!
 *
 *  @date 2016-03-02
 *  @authors  Henrik von Coler, Benjamin Wiemann
 *
 **/

#include "GloooSynth.h"


GloooSynth* synth;

/**
 * @brief The central class serving as a Jack Client.
 *
 * - owns most data-related objects
 *
 */
GloooSynth::GloooSynth(const char *modelPath, const char *configFile) {

    synth = this;

    // read stuff from the YAML config and parse it to the glooconf object
    gloooconf = new GloooConfig(modelPath,  configFile);

    // Prepare output files /////////////////////////////////////////////////////////////////
    textOutMan = new TextOutputManager(gloooconf, -1);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// READ DATA and initialize objects
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @note This is just for making the startup
    /// faster - in the end, both models should be loaded
    /// and a parameter during runtime determines the mode!

    // READ filter coefficients and initialize object
    this->bark_coefficients = std::shared_ptr<BarkCoefficients>(new BarkCoefficients(gloooconf));


    if( gloooconf->get_synthesis_mode().compare("deterministic")==0)
    {
        this->deterministic_data = std::shared_ptr<DeterministicData>(new DeterministicData());
        this->deterministic_data->load_deterministic(gloooconf->SMS_path(), gloooconf->assignment_file(), gloooconf->get_max_partials());
    }
    else if(gloooconf->get_synthesis_mode().compare("stochastic")==0)
    {
        this->stochastic_data = std::shared_ptr<StochasticData>(new StochasticData(gloooconf));
        this->stochastic_data->load_stochastic( gloooconf->assignment_file(), gloooconf->get_max_partials());
    }

    else if( gloooconf->get_synthesis_mode().compare("dual")==0)
    {
        this->stochastic_data = std::shared_ptr<StochasticData>(new StochasticData(gloooconf));
        this->stochastic_data->load_stochastic( gloooconf->assignment_file(), gloooconf->get_max_partials());
        this->deterministic_data = std::shared_ptr<DeterministicData>(new DeterministicData());
        this->deterministic_data->load_deterministic(gloooconf->SMS_path(), gloooconf->assignment_file(), gloooconf->get_max_partials());
    }


    // Assign as many jack-outputs as we have partials and noise bands
    /// @todo Get GloooSynth->nOutputs as parameter from GloooConfig!
    this->nOutputs = 104; // num partials + bark_coefficients->n_bands() * 2;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Open and configure the jack client
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << "Starting Jack Server with " << this->nOutputs << "outputs!" << endl;

    this->client = jack_client_open("GLOOO_V3", JackNullOption, &status, NULL);

    if (this->client == NULL) {
        fprintf(stderr, "jack_client_open() failed, "
                        "status = 0x%2.0x\n", status);
        if (this->status & JackServerFailed) {
            fprintf(stderr, "Unable to connect to JACK server\n");
        }
        exit(1);
    }

    // status check
    if (this->status & JackServerStarted)
    {
        fprintf(stderr, "JACK server started\n");
    }
    if (this->status & JackNameNotUnique)
    {
        this->clientName = jack_get_client_name(this->client);
        fprintf(stderr, "unique name `%s' assigned\n", this->clientName);
    }

    /// Set jack callbacks
    jack_set_process_callback(this->client, callback_process, this);
    jack_on_shutdown(this->client, jackShutdown, 0);

    // a callback for a frame size change
    //jack_set_buffer_size_callback(this->client, callback_set_frame_size, this);
    jack_set_sample_rate_callback(this->client, callback_set_sample_rate, this);

    // register portss
    // MIDI input
    this->inPort = jack_port_register(client, "MIDI in", JACK_DEFAULT_MIDI_TYPE,
                                      JackPortIsInput, 0);
    printf("Registered port '%s' with id %d for MIDI input \n",
           jack_port_name(this->inPort), (uint) jack_port_uuid(this->inPort));


    // set audio outputs
    /// @todo: implement switch between partial- and bark band spatialization
    this->outPort = new jack_port_t*[nOutputs];

    for (int i=0; i < nOutputs; i++)
    {
          std::string tmp_str = "-";

        /// @todo Get this value from number of partials.
        if(i<79)
             tmp_str = "Partial_" + std::to_string(i+1);
        else
             tmp_str = "Residual_" + std::to_string(i+1 - 79);


        this->outPort[i] = jack_port_register(client, tmp_str.c_str(),
                                              JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

        printf("Registered port '%s' with id %d for audio output \n",
               jack_port_name(this->outPort[i]),
               (uint) jack_port_uuid(this->outPort[i]));


    }


    // allocate output buffers
    out = new jack_default_audio_sample_t*[nOutputs];

    for ( int i=0 ; i< nOutputs; i++)
    {
        out[i] = (jack_default_audio_sample_t *) jack_port_get_buffer(this->outPort[i], jack_get_buffer_size(client));
    }


    // Initialize the global math object
    GM = new GlobalMath(jack_get_sample_rate(client),jack_get_buffer_size(client));



    // Allocate the voice manager
    voice_manager = new VoiceManager(out, deterministic_data, stochastic_data,  bark_coefficients, gloooconf ,jack_get_buffer_size(client), jack_get_sample_rate(client), textOutMan, GM);

    cout << "Jack sample rate: " << jack_get_sample_rate(client) << endl << endl;


    if ((this->inPort == NULL) || (this->outPort == NULL)) {
        fprintf(stderr, "no more JACK ports available\n");
        exit(1);
    }

    // Activate jack
    if (jack_activate(this->client)) {
        fprintf(stderr, "Cannot activate client");
        exit(1);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Connect all midi and partials (audio outputs) /////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (int i=0; i <= 79; i++)
    {

        std::string tmp_str = "Partial_" + std::to_string(i+1);

        // connect
        std::string s22 = "GLOOO_V3:";

        string tmp1 =   s22+tmp_str;

        string tmp2;
        //if(i % 2 == 0)
        tmp2 = "system:playback_1";
        jack_connect(client,tmp1.c_str(),tmp2.c_str());


        //else
        tmp2 = "system:playback_2";


        cout << "Trying to connect:" << tmp1 << " to " << tmp2 << endl;

        jack_connect(client,tmp1.c_str(),tmp2.c_str());


    }


    for (int i=0; i < bark_coefficients->n_bands(); i++)
    {

        std::string tmp_str = "Residual_" + std::to_string(i+1);

        // connect
        std::string s22 = "GLOOO_V3:";

        string tmp1 =   s22+tmp_str;

        string tmp2;
        //if(i % 2 == 0)
        tmp2 = "system:playback_1";
        jack_connect(client,tmp1.c_str(),tmp2.c_str());


        //else
        tmp2 = "system:playback_2";


        cout << "Trying to connect:" << tmp1 << " to " << tmp2 << endl;

        jack_connect(client,tmp1.c_str(),tmp2.c_str());


    }


//         for (  2*(bark_coefficients->n_bands(); gloooconf->N_partials() ;i++)
//         {

//    }

    // jack_connect(client,"a2j_bridge:capture","GLOOO_V3:MIDI in");



    // close client at exit
    atexit(exitCallback);

    while(1)
    {
        sleep(1);
    };

//    cout << "Enter 'quit' to exit" << endl;
//    string s;
//    cin >> s;

//    voice_manager->~VoiceManager();

}




/// @brief The jack-callback function
///
/// @param samples_per_frame: the number of samples in the jack output buffer
///
int GloooSynth::process(jack_nframes_t samples_per_frame) {

    /// 1: Process the OSC and midi massages und update the voices
    voice_manager->update_voices();

    /// 2: Get the complete next frame and write it to the jack buffers
    voice_manager->getNextFrame_TD(out, jack_get_buffer_size(client), nOutputs);

    return 0;
}

///
/// \brief GloooSynth::getClient
/// return a pointer to the JACK client
/// \return pointer to the JACK client
///
jack_client_t* GloooSynth::getClient() {
    return this->client;
}

void GloooSynth::set_frame_size()
{

}

void jackShutdown(void *arg) {
    exit(1);
}

int callback_process(jack_nframes_t x, void* object) {
    return static_cast<GloooSynth*>(object)->process(x);
}

//int callback_set_frame_size(jack_nframes_t frame_size, void *object) {
//    static_cast<JackClient*>(object)->set_frame_size((uint) frame_size);
//    return 0;
//}

int callback_set_sample_rate(jack_nframes_t sample_rate, void* object) {
    static_cast<GloooSynth*>(object)->set_sample_rate(sample_rate);
    return 0;
}

void exitCallback() {

    cout << "Entered JackClient::exitCallback !" << endl;


    std::raise(SIGINT);

    jack_client_close(synth->getClient());

}

void GloooSynth::set_sample_rate(uint sample_rate) {

}

GloooSynth::~GloooSynth()
{
    // TODO Auto-generated destructor stub

}
