#include "VoiceManager.h"



VoiceManager::VoiceManager(jack_default_audio_sample_t **out,
                           std::shared_ptr<DeterministicData> DR1,
                           std::shared_ptr<StochasticData> DR,
                           std::shared_ptr<BarkCoefficients> BC,
                           GloooConfig *gc,
                           uint bufferSize,
                           uint sampleRate,
                           TextOutputManager *toM,
                           GlobalMath *GM)
{

    // grab pointers to relevant objects
    gloooconf   = gc;
    d_data      = DR1;
    s_data      = DR;
    b_coeff     = BC;
    OUT         = out;
    textOutMan  = toM;
    globMat     = GM;

    // No overlap - add is used -
    // blocksize and hopsize identical!
    blocksize = bufferSize;
    hopsize   = bufferSize;

    // set a default master volume
    master_volume = 1.0;

    random_depth  = 0.01;

    for(int voiceCNT=0; voiceCNT<gloooconf->N_voices();voiceCNT++)
    {
        voices.push_back(new SingleVoice(voiceCNT, sampleRate,this->blocksize,d_data, s_data, b_coeff, gloooconf, &gp, globMat));
    }

    // create new OscManager Object
    osc_manager     = new OscManager(gloooconf->osc_in_port(),&gp, gloooconf);
    if (gloooconf->receive_midi() == 1)
        midi_manager    = new MidiManager(gloooconf->midi_in_port(), &gp, gloooconf);

    // Disable OSC control if midi manager is active
    //if (midi_manager != nullptr)
    //{
    //    gloooconf->set_osc(0);
    //} else
    //{
    //    gloooconf->set_midi(0);
    //}
    
    // set some parameters
    gp.printControl = gloooconf->print_control();

    gp.partial_activation.resize(gloooconf->get_max_partials(),1);

    /// @todo Analysis parameters are hard-coded for the sms rate calculation.
    /// Needs to be passed from the analysis data.
    gp.sms_rate =  ((double) bufferSize /  (double) sampleRate)   /   (256.0 /  96000.0) ;

    // defalut articulation stlye
    art_style = glissando;

}

VoiceManager::~VoiceManager()
{

    textOutMan->export_to_file(gp.n_partials);

    for(int voiceCNT = 0; voiceCNT < gloooconf->N_voices(); voiceCNT++)
    {
        voices[voiceCNT]->~SingleVoice();
    }
}

bool VoiceManager::is_shutdown()
{
    return this->gp.shutwown;
}



void VoiceManager::update_voices()
{
    // OSC //////////////////////////////////////////////////////////////////////////////////////
    // use only if OSC receiver is activated

    if (gloooconf->receive_osc() == 1 || gloooconf->receive_midi() == 1)
    {
        // got note events?
        unsigned int N_events;
        if (gloooconf->receive_osc() == 1)
            N_events = osc_manager->number_of_note_events();
            
        if (gloooconf->receive_midi() == 1)
            N_events = midi_manager->number_of_note_events();
        
        // update articulation style
        art_style = gp.articulation_mode;

        //  Get the most recent pitch value from parameter struct.
        pitch = gp.pitch;

        /// Legato and glissando handling.
        if (gp.is_playing == true)
        {

            if(N_events > 0)
            {

                // Get ranks a of voices:
                /// @todo This can be done earlier and global for the update method.
                std::vector<int> ranks;
                for(int i = 0; i< gloooconf->N_voices(); i++)
                    ranks.push_back(voices[i]->get_rank());
                int max_rank_idx = std::max_element(ranks.begin(),ranks.end()) - ranks.begin();
                int max_rank     = *std::max_element(ranks.begin(), ranks.end());

                std::vector<SingleVoice*>::iterator active_voice;
                // get active voice
                for(active_voice = voices.begin(); active_voice != voices.end(); active_voice++)
                {
                    if((*active_voice)->is_active() == true)
                        break;
                }

                if (gloooconf->receive_osc() == 1)
                    osc_manager->flush_note_messages();
    
                if (gloooconf->receive_midi() == 1)                
                    midi_manager->flush_note_messages();

                note_event_t event;

                event.velocity = gp.intensity;
                event.note     = pitch;

                switch (art_style)
                {

                case legato:

                    if(gloooconf->print_control()==true)
                        cout << "Legato transition to " << event.velocity << " - " << event.note  << endl;


                    (*active_voice)->set_released(legato);
                    increase_ranks();

                    // voice stealing
                    if(max_rank<numberOfVoices())
                    {
                        voices[max_rank_idx]->deactivate();
                        voices[max_rank_idx]->activate(event,legato);
                    }
                    else
                    {
                        voices[max_rank_idx]->activate(event,legato);
                    }


                    break;

                case glissando:

                    for(std::vector<SingleVoice*>::iterator it = voices.begin(); it != voices.end(); it++)
                    {
                        if(
                                (*it)->get_state() == sustain ||
                                (*it)->get_state() == attack ||
                                (*it)->get_state() == transition )
                        {

                            if(gloooconf->print_control()==true)
                                cout << "Found active voice: " << (*it)->get_id()  << endl;

                            (*it)->start_transition(glissando, event);

                        }
                    }

                    break;

                case detached:
                    break;


                } // end of: switch (art_style)

                print_voice_activity();

            } // end of: if(N_events > 0)

        } // end of: if (gp.is_playing == true)
        else
        {
            // flush all note messages if
            // synth is not playing
            if (gloooconf->receive_osc() == 1)
                osc_manager->flush_note_messages();

            if (gloooconf->receive_midi() == 1)                
                midi_manager->flush_note_messages();
        }

        // grab the most recent intensity value
        double intensity = this->gp.intensity;

        // if intensity exceeds threshold
        if(intensity >= gp.intensity_thresh)
        {

            // only do something if the synth is NOT playing
            // and if the detected pitch is not 0
            if(gp.is_playing==false && gp.pitch > 0)
            {

                bool found_empty = false;

                // create a note event using the
                // active midi note frequency and
                // the intensity
                note_event_t event;
                event.note     = gp.active_note;
                event.velocity = intensity;

                // loop over all voices
                for(std::vector<SingleVoice*>::iterator it = voices.begin(); it != voices.end(); it++)
                {

                    if((*it)->get_state() == glooo::inactive)
                    {


                        (*it)->activate(event, detached);

                        found_empty = true;

                        if(gloooconf->print_control()==true)
                            cout << "Activated note: " << (*it)->get_id()  << endl;

                        break;
                    }

                }

                // if no inactive voice was found: steal the first one
                /// @todo keep track of voices' age
                if(found_empty == false)
                {
                    voices[0]->deactivate();
                    voices[0]->activate(event, detached);
                }
                // and set playing state to true afterwards
                gp.is_playing=true;

                //if(gloooconf->print_control()==true)
                print_voice_activity();
            }

        }

        // if intensity is below threshold
        if(intensity < gp.intensity_thresh)
        {
            // only do something if
            // the synth IS playing
            if(gp.is_playing==true)
            {

                if(gloooconf->print_control()==true)
                    cout << "Got release!" << endl;

                // Loop for setting voices released, if they receive a note-off!
                for(std::vector<SingleVoice*>::iterator it = voices.begin(); it != voices.end(); it++)
                {

                    // ignore offset when already released
                    // AND if inactive
                    if((*it)->get_state() != release && (*it)->get_state() != inactive)
                    {
                        (*it)->set_released(detached);
                    }

                }

                // and set false when done
                gp.is_playing = false;

                print_voice_activity();

            }

        }

    }//  end of: if(gloooconf->receive_osc() == 1)
}




jack_default_audio_sample_t * VoiceManager::recentBlock()
{
    return OUT[0];
}



void VoiceManager::getNextFrame_TD(
        jack_default_audio_sample_t **out,   //!< A 2d array for all partials and samples in a frame
        uint32_t n_samples,
        int N_out)                  //!< The jack sample rate
{


    /// 1: set all jack output buffers to zero
    for(int i=0; i < N_out; ++i)
    {
        memset (out[i], 0, sizeof(float) * n_samples);
    }

    /// 2: loop over all voices
    for(uint voiCNT=0; voiCNT < voices.size(); ++voiCNT)
    {
        if (!voices[voiCNT]->is_active ()) {
            continue;
        }

        /* Process control values for active voices at the start of each cycle */

        // apply master volume (all voices)
        voices[voiCNT]->volume(gp.master_vol);
        voices[voiCNT]->random_depth(gp.random_depth);
        voices[voiCNT]->sms_rate(gp.sms_rate);
        voices[voiCNT]->set_intensity(gp.intensity);

        // assignpitch value
        if(gp.pitch > 0)
        {
            voices[voiCNT]->set_pitch(gp.pitch);
        }

        if(gloooconf->print_control()==true)
        {
            textOutMan->write_control(gp.intensity,false);
            textOutMan->write_control(gp.pitch,true);
        }

        uint32_t remain = n_samples;

        // perform initialization that needs to happen at every cycle's bufferPOS==0


        while (remain > 0)
        {
            uint32_t n_proc = remain; // std::min(32U, remain); @todo

            if(gloooconf->get_synthesis_mode().compare("deterministic")==0)
            {
                voices[voiCNT]->cycle_start_deterministic();
            }
            else if(gloooconf->get_synthesis_mode().compare("stochastic")==0)
            {
                voices[voiCNT]->cycle_start_stochastic();
            }
            else if(gloooconf->get_synthesis_mode().compare("dual")==0)
            {
                voices[voiCNT]->cycle_start_stochastic();
            }

            // synthesize active voice(s), according to dispersion mode
            switch(gp.disp_mode)
            {
            case uniform:

                voices[voiCNT]->get_next_block_uniform(out, n_proc);
                break;

            case bark:

                voices[voiCNT]->get_next_block_bark(out, n_proc);
                break;

            case discrete:
                voices[voiCNT]->get_next_block_discrete(out, n_proc);
                break;

            }
            //
            voices[voiCNT]->cycle_proceed();

            remain -= n_proc;
        }
    }
}


int VoiceManager::numberOfVoices()
{
    return voices.size();
}

void VoiceManager::print_voice_activity()
{

    // display voice activity in terminal
    cout << "Voices active: ";

    for(int i=0; i<gloooconf->N_voices(); i++)
        cout << " " << voices[i]->is_active() << " ";

    cout  << endl;
}


void VoiceManager::increase_ranks()
{

    for(std::vector<SingleVoice*>::iterator it = voices.begin(); it != voices.end(); it++)
    {
        if((*it)->get_rank() < numberOfVoices())
            (*it)->increase_rank();
    }
}
