#include"MidiManager.h"

MidiManager::MidiManager(int p, glooo_parameters *gp, GloooConfig *gc)
{
    gloooParam  = gp;

    gloooConf   = gc;

    try 
    {
        midiIn = new RtMidiIn();
    }
    catch(RtMidiError &error)
    {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    // Display available ports for the user
    unsigned int portCount = midiIn->getPortCount();
    string portName;
    for (int i = 0; i < portCount; i++) 
    {
        try 
        {
            portName = midiIn->getPortName(i);
        }
        catch(RtMidiError &error) 
        {
            error.printMessage();
            delete midiIn;
        }
        cout << "MIDI input port #" << i+1 << ": " << portName << endl;
    }
    
    try 
    {
        midiIn->openPort(p-1);
        cout << "Midi device " << midiIn->getPortName(p-1) << " is used" << endl;
    }
    catch (RtMidiError &error) 
    {
        error.printMessage();
        delete midiIn;
    }
    
    midiIn->ignoreTypes(false, false, false);

    midiIn->setCallback(&callback, this);
}

MidiManager::~MidiManager()
{
    delete midiIn;
}

void MidiManager::callback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    midiMessage midiMessage;
    midiMessage.byte0 = (unsigned int) message->at(0);
    unsigned int channel = midiMessage.byte0 & 0xF;
    unsigned int status  = midiMessage.byte0 & 0xF0;

    note_event_t note;
    
    switch (status)
    {
    case 128: // Note Off event
        note.note       = (int) message->at(1);
        note.velocity   = (int) message->at(2);
        note.type       = glooo::note_off;
        note.time       = deltatime;
        static_cast<MidiManager*>(userData)->gloooParam->intensity = (double) message->at(2);
        break;
    case 144: // Note On event
        note.note       = (int) message->at(1);
        note.velocity   = (int) message->at(2);
        note.type       = glooo::note_on;
        note.time       = deltatime;
        static_cast<MidiManager*>(userData)->gloooParam->pitch = (double) message->at(1);
        static_cast<MidiManager*>(userData)->gloooParam->intensity = (double) message->at(2);
        break;
    case 176: // Control Change event 
        if ((int) message->at(1) == 48) // Master volume on CC48
        {
            static_cast<MidiManager*>(userData)->gloooParam->master_vol = (double) message->at(2) * 0.02362;
        } 
        else if ((int) message->at(1) == 49) // Tonal volume on CC49
        {
            static_cast<MidiManager*>(userData)->gloooParam->tonal_vol = (double) message->at(2) * 0.02362;
        }
        else if ((int) message->at(1) == 50) // Noise volume on CC50
        {
            static_cast<MidiManager*>(userData)->gloooParam->noise_vol = (double) message->at(2) * 0.02362;
        } else
        {
            break;
        }
    case 224: // Pitch bend
        break;
    default:
        break;
    } 
}

int MidiManager::number_of_note_events()
{
    return note_events.size();
}

void MidiManager::insert_note_event(note_event_t ne)
{
    note_events.push_back(ne);
}

note_event_t MidiManager::get_note_event(int position)
{
    note_event_t n = note_events[position];

    return n;
}

void MidiManager::flush_note_messages()
{
    note_events.clear();
}





