#include "OscManager.h"

///////////////////////////////////////////////////////////////////////
/// \brief Constructor
///
/////////////////////////////////////////////////////////////////////////

OscManager::OscManager(int p, glooo_parameters *gp, GloooConfig *gc) //!< the UDP port for the osc server
{

    glooParam = gp;

    gloooConf = gc;

    port = p;
    received = 0;

    /// @todo Run a loop to find next free port if the chosen one is in use
    bool success = false;
    while (success == false)
    {
        try
        {
            st = new lo::ServerThread(port);
            success = true;
        }
        catch (int e)
        {
            cout << "Increasing port!" << endl
                 << endl;
            port += 1;
        }
    }

    cout << "Changed OSC input port to: " << port << endl
         << endl;

    // performance parameters
    st->add_method("/pitch", "f", pitch_handler, this);
    st->add_method("/intensity", "f", intensity_handler, this);
    st->add_method("/note", "ii", note_event_handler, this);
    st->add_method("/gliss_duration", "f", glissando_duration_handler, this);
    st->add_method("/gliss_gamma",    "f", glissando_gamma_handler, this);


    // adjustment
    st->add_method("/master_volume", "f", master_volume_handler, this);
    st->add_method("/tonal_volume", "f", tonal_volume_handler, this);
    st->add_method("/noise_volume", "f", noise_volume_handler, this);
    st->add_method("/pitch_shift", "f", pitch_shift_handler, this);
    st->add_method("/sms_rate", "f", sms_rate_handler, this);
    st->add_method("/random_depth", "f", rand_depth_handler, this);
    st->add_method("/threshold", "f", threshold_handler, this);

    // synthesis modes
    st->add_method("/part_amp_mode", "s", part_amp_mode_handler, this);
    st->add_method("/part_freq_mode", "s", partfreq_mode_handler, this);
    st->add_method("/noise_energy_mode", "s", noise_energy_mode_handler, this);
    st->add_method("/disp_mode", "s", dispersion_mode_handler, this);
    st->add_method("/art_mode",       "s", articulation_mode_handler, this);

    // partial activity
    st->add_method("/n_partials", "i", n_partials_handler, this);
    st->add_method("/partial_active", "ii", partial_activity_handler, this);

    /// @todo Add subscribe mode!
    //st->add_method( "/subscribe", "ii", note_event_handler, this);

    st->add_method("/shutdown", "i", shutdown_handler, this);

    st->start();

    std::cout << "Started OSC server on port:" << port << endl;
}

int OscManager::note_event_handler(const char *path, const char *types, lo_arg **argv,
                                   int argc, void *data, void *user_data)
{

    // static cast to this object
    //OscManager* statCast = static_cast<OscManager*>(user_data);

    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Got note on: " << argv[0]->i << " - " << argv[1]->i << endl;

    // create a note event
    note_event_t ne;
    ne.note = argv[0]->i;
    ne.velocity = argv[1]->i;

    //statCast->insert_note_event(ne);
    static_cast<OscManager *>(user_data)->insert_note_event(ne);

    // write only
    static_cast<OscManager *>(user_data)->glooParam->active_note = argv[0]->i;

    return 0;
}

int OscManager::master_volume_handler(const char *path, const char *types, lo_arg **argv,
                                      int argc, void *data, void *user_data)
{

    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed master volume to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->master_vol = argv[0]->f;
    return 0;
}

int OscManager::tonal_volume_handler(const char *path, const char *types, lo_arg **argv,
                                     int argc, void *data, void *user_data)
{

    //if(static_cast<OscManager*>(user_data)->glooParam->printControl == true)
    //    cout << "Changed master volume to: " << argv[0]->f  << endl;

    static_cast<OscManager *>(user_data)->glooParam->tonal_vol = argv[0]->f;
    return 0;
}

int OscManager::noise_volume_handler(const char *path, const char *types, lo_arg **argv,
                                     int argc, void *data, void *user_data)
{

    //if(static_cast<OscManager*>(user_data)->glooParam->printControl == true)
    //    cout << "Changed master volume to: " << argv[0]->f  << endl;

    static_cast<OscManager *>(user_data)->glooParam->noise_vol = argv[0]->f;
    return 0;
}

int OscManager::sms_rate_handler(const char *path, const char *types, lo_arg **argv,
                                 int argc, void *data, void *user_data)
{

    // static cast to this object
    //OscManager* statCast = static_cast<OscManager*>(user_data);
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed SMS rate to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->sms_rate = argv[0]->f;

    return 0;
}

int OscManager::rand_depth_handler(const char *path, const char *types, lo_arg **argv,
                                   int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed random depth to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->random_depth = argv[0]->f;

    return 0;
}

int OscManager::intensity_handler(const char *path, const char *types, lo_arg **argv,
                                  int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)

        cout << "Changed intensity to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->intensity = argv[0]->f;

    return 0;
}

int OscManager::pitch_handler(const char *path, const char *types, lo_arg **argv,
                              int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed pitch to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->pitch = argv[0]->f;
    return 0;
}

int OscManager::pitch_shift_handler(const char *path, const char *types, lo_arg **argv,
                                    int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed pitch to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->pitch_shift = argv[0]->f;
    return 0;
}

///  @todo Add
int OscManager::part_amp_mode_handler(const char *path, const char *types, lo_arg **argv,
                                      int argc, void *data, void *user_data)
{

    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed sms-AM mode to: " << argv[0]->s << endl;

    static_cast<OscManager *>(user_data)->glooParam->part_amp_mode = argv[0]->s;
    return 0;
}

int OscManager::partfreq_mode_handler(const char *path, const char *types, lo_arg **argv,
                                      int argc, void *data, void *user_data)
{

    //    if(static_cast<OscManager*>(user_data)->glooParam->printControl == true)
    //        cout << "Changed sms-AM mode to: " << argv[0]->s  << endl;

    static_cast<OscManager *>(user_data)->glooParam->part_freq_mode = argv[0]->s;
    return 0;
}

int OscManager::noise_energy_mode_handler(const char *path, const char *types, lo_arg **argv,
                                          int argc, void *data, void *user_data)
{

    //    if(static_cast<OscManager*>(user_data)->glooParam->printControl == true)
    cout << "Changed noise energy mode to: " << argv[0]->s << endl;

    static_cast<OscManager *>(user_data)->glooParam->noise_energy_mode = argv[0]->s;
    return 0;
}

///////////////////////////////////////////////////////////////////////
/// \brief The default callback function:
///  On each incoming message, we push it into the vectors!
/// \param void
/// \return void
/////////////////////////////////////////////////////////////////////////

int OscManager::number_of_note_events()
{
    // return 1;
    return note_events.size();
}

void OscManager::insert_note_event(note_event_t ne)
{

    note_events.push_back(ne);
}

note_event_t OscManager::get_note_event(int position)
{

    note_event_t n = note_events[position];

    return n;
}

void OscManager::flush_note_messages()
{
    note_events.clear();
}

int OscManager::threshold_handler(const char *path, const char *types, lo_arg **argv,
                                  int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)

        cout << "Changed intensity threshold to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->intensity_thresh = argv[0]->f;

    return 0;
}

int OscManager::shutdown_handler(const char *path, const char *types, lo_arg **argv,
                                 int argc, void *data, void *user_data)
{

    cout << "Got remote SSSHHHUUUT down" << endl;

    static_cast<OscManager *>(user_data)->glooParam->shutwown = true;

    return 0;
}

int OscManager::articulation_mode_handler(const char *path, const char *types, lo_arg **argv,
                                          int argc, void *data, void *user_data)
{

    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed articulation mode to: " << argv[0]->s << endl;

    char tmp = argv[0]->s;

    if (tmp == 'g')
        static_cast<OscManager *>(user_data)->glooParam->articulation_mode = glooo::glissando;

    if (tmp == 'l')
        static_cast<OscManager *>(user_data)->glooParam->articulation_mode = glooo::legato;

    return 0;
}


int OscManager::glissando_duration_handler(const char *path, const char *types, lo_arg **argv,
                                  int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed glissando duration to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->glissando_duration = argv[0]->f;

    return 0;
}


int OscManager::glissando_gamma_handler(const char *path, const char *types, lo_arg **argv,
                                  int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed glissando gamma to: " << argv[0]->f << endl;

    static_cast<OscManager *>(user_data)->glooParam->glissando_gamma = argv[0]->f;

    return 0;
}



int OscManager::n_partials_handler(const char *path, const char *types, lo_arg **argv,
                                   int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed number of partials to: " << argv[0]->i << endl;

    // make sure that number is within allowed range
    /// @todo move to SingleVoice
    if (argv[0]->i < (static_cast<OscManager *>(user_data)->gloooConf->get_max_partials()) && argv[0]->i >= 0)
        static_cast<OscManager *>(user_data)->glooParam->n_partials = argv[0]->i;

}

int OscManager::partial_activity_handler(const char *path, const char *types, lo_arg **argv,
                                         int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed partial activity: " << argv[0]->i << endl;

    static_cast<OscManager *>(user_data)->glooParam->partial_activation[argv[0]->i] = argv[1]->i;
}

int OscManager::dispersion_mode_handler(const char *path, const char *types, lo_arg **argv,
                                        int argc, void *data, void *user_data)
{
    if (static_cast<OscManager *>(user_data)->glooParam->printControl == true)
        cout << "Changed dispersion mode: " << argv[0]->s << endl;

    string c(1,argv[0]->s);

    if(c == "u")
        static_cast<OscManager *>(user_data)->glooParam->disp_mode = uniform;

    if(c == "b")
        static_cast<OscManager *>(user_data)->glooParam->disp_mode = bark;

    if(c == "d")
        static_cast<OscManager *>(user_data)->glooParam->disp_mode = discrete;

}
