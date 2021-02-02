/**
 *
 * @file OscManager.h
 *
 * @class OscManager
 *
 * @brief Class which parses the incoming OSC messages.
 *
 * This class parses the incoming OSC messages
 * to the glooo_parameters struct.
 *
 * \author Henrik von Coler
 *
 * \version $Revision: 0.5 $
 *
 * \date $Date: 2016-08-18$
 *
 * Contact: von_coler@tu-berlin.de
 *
 *
 */



#ifndef OSCMAN_H
#define OSCMAN_H


// works with 'self built'
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <iostream>
#include <string>
#include <atomic>

#include "glooo.h"
#include "GloooConfig.h"



using namespace std;

class OscManager
{

private:

    glooo_parameters *glooParam;

    /// the port to be opened
    int port;

    lo::ServerThread        *st;

    vector<note_event_t>    note_events;

    vector<double>          master_volume;

    std::atomic<int> received;

    GloooConfig *gloooConf;

    ///
    /// \brief note_event_handler
    /// \param path
    /// \param user_data
    /// \return
    ///
    static int note_event_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);


    static int master_volume_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int tonal_volume_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int noise_volume_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int sms_rate_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);


    static int rand_depth_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);


    static int intensity_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);


    static int pitch_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);


    static int part_amp_mode_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int partfreq_mode_handler(const char *path, const char *types, lo_arg ** argv,
                               int argc, void *data, void *user_data);

    static int  threshold_handler(const char *path, const char *types, lo_arg ** argv,
                                  int argc, void *data, void *user_data);

    static int  shutdown_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  noise_energy_mode_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  articulation_mode_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  glissando_duration_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  glissando_gamma_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  n_partials_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);

    static int  partial_activity_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);



    static int  dispersion_mode_handler(const char *path, const char *types, lo_arg ** argv,
                                 int argc, void *data, void *user_data);



public:

    OscManager(int p, glooo_parameters *gp, GloooConfig *gc);

    int number_of_note_events();

    void insert_note_event(note_event_t ne);

    note_event_t get_note_event(int position);

    vector<note_event_t>* get_note_events();

    void flush_note_messages();

};

#endif
