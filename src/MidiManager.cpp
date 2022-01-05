#include"MidiManager.h"

MidiManager::MidiManager(int p, glooo_parameters *gp, GloooConfig *gc)
{
    gloooParam  = gp;

    gloooConf   = gc;

    #if 0 // Choose midi device via user input    
    char choice = 'N';

    cout << "Do you want to use a mide device (Using a midi device will disable the OSC control)?" << "\n" << "[Y] Yes [N] No (Default)" << endl; 
    choice = cin.get();

    if (choice == 'Y')
    {
        try 
        {
            midiIn = new RtMidiIn();
        }
        catch(RtMidiError &error)
        {
            error.printMessage();
            exit(EXIT_FAILURE);
        }

        // Check ports
        unsigned int portCount = midiIn->getPortCount();

        cout << "There are " << portCount << " Midi input sources available." << endl;      

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
            cout << "Input Port #" << i+1 << ": " << portName << endl;
        }

        string portNumber;
        while (port < 0 || port >= portCount && portCount != 0)
        {
            cout << "Choose port: ";
            cin >> portNumber;
            port = stoi(portNumber) - 1;
        }

        cout << "Midi device " << midiIn->getPortName(port) << " is used" << endl;

        // Open port
        try 
        {
          midiIn->openPort (port);
        }
        catch (RtMidiError &error) 
        {
          error.printMessage();
          delete midiIn;
        }

        // Set callback        
        midiIn->setCallback(&callback, this);

        midiIn->ignoreTypes(false, false, false);
    }
    #endif

    #if 1 // Choose port for midi device in settings
    try 
    {
        midiIn = new RtMidiIn();
    }
    catch(RtMidiError &error)
    {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

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
    
    midiIn->setCallback(&callback, this);

    midiIn->ignoreTypes(false, false, false);
    #endif
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
    cout << status << endl;
    cout << (double)message->at(1) << endl;
    cout << (double)message->at(2) << endl;


    note_event_t note;
    
    switch (status)
    {
    case 128: // Note Off event
        note.note       = (int) message->at(1);
        note.velocity   = (int) message->at(2);
        note.type       = glooo::note_off;
        note.time       = deltatime;
        //static_cast<MidiManager*>(userData)->gloooParam->pitch = (double) message->at(1);
        static_cast<MidiManager*>(userData)->gloooParam->intensity = (double) message->at(2);
        break;
    case 144: // Note On event
        note.note       = (int) message->at(1);
        note.velocity   = (int) message->at(2);
        note.type       = glooo::note_on;
        note.time       = deltatime;
        static_cast<MidiManager*>(userData)->gloooParam->pitch = (double) message->at(1);
        static_cast<MidiManager*>(userData)->gloooParam->intensity = (double) message->at(2);
        //static_cast<MidiManager*>(userData)->insert_note_event(note);
        //static_cast<MidiManager*>(userData)->gloooParam->active_note = message->at(1);
        break;
    case 176: // Control Change event 
        if ((int) message->at(1) == static_cast<MidiManager*>(userData)->gloooConf->masterVol_CC) // Master volume on CC48
        {
            static_cast<MidiManager*>(userData)->gloooParam->master_vol = (double) message->at(2) * 0.02362;
        } 
        else if ((int) message->at(1) == static_cast<MidiManager*>(userData)->gloooConf->tonalVol_CC) // Tonal volume on CC49
        {
            static_cast<MidiManager*>(userData)->gloooParam->tonal_vol = (double) message->at(2) * 0.02362;
        }
        else if ((int) message->at(1) == static_cast<MidiManager*>(userData)->gloooConf->noiseVol_CC) // Noise volume on CC50
        {
            static_cast<MidiManager*>(userData)->gloooParam->noise_vol = (double) message->at(2) * 0.02362;
        } else
        {
            break;
        }
    case 224: // Pitch bend
        // static_cast<MidiManager*>(userData)->gloooParam->pitch = (double) message->at(1);
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





