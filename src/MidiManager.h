/**
 *
 * @file MidiManager.h
 *
 * @class MidiManager
 *
 * @brief Class which parses the incoming MIDI messages.
 *
 * This class parses the incoming MIDI messages
 * to the glooo_parameters struct.
 *
 * \author Hilko Tondock 
 *
 * \version $Revision: 0.5 $
 *
 * \date $Date: 2021-12-09$
 *
 * Contact: h.tondock@campus.tu-berlin.de
 *
 *
 */

#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H


#include<rtmidi/RtMidi.h>
#include<iostream>
#include<vector> 

#include "glooo.h"
#include "GloooConfig.h"

using namespace std;
class MidiManager 
{

private:

    // data structure of a midi message
    typedef struct 
    {
        unsigned int byte0;
        unsigned int byte1;
        unsigned int byte2;
    } midiMessage;
    
    RtMidiIn *midiIn = nullptr;

    glooo_parameters *gloooParam;

    GloooConfig *gloooConf;
    
    static void callback(double deltatime, vector<unsigned char> *message, void *userData);

    bool isVerbose = true;

    vector<note_event_t> note_events;

public:

    MidiManager(int p, glooo_parameters *gp, GloooConfig *gc);

    ~MidiManager();

    int number_of_note_events();

    void insert_note_event(note_event_t ne);

    note_event_t get_note_event(int position);

    void flush_note_messages();
    
};

#endif // MIDIMANAGER_H