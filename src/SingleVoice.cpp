/**
 *
 * @file SingleVoice.cpp
 * @class SingleVoice
 * @brief Class for a single voice!
 *
 *
 * SingleVoice holds the sinusoids and
 * manages the model data for one voice.
 *
 * @author Henrik von Coler
 *
 * @version  0.85
 *
 * @date   2014/06/02
 *
 */


#include "SingleVoice.h"


SingleVoice::SingleVoice(
        int id,
        int fs,
        int bs,
        std::shared_ptr<DeterministicData> DR1,
        std::shared_ptr<StochasticData> DR,
        std::shared_ptr<BarkCoefficients> BC,
        GloooConfig *gc,
        glooo_parameters *GP,
        GlobalMath *GM)

{

    // grab pointer to glooo config object.
    gloooconf   = gc;

    ID   = id;
    rank = gloooconf->N_voices();

    // initial settings of member variables
    gp          = GP;
    globMat     = GM;



    this->fs     = fs;
    this->fs_2   = fs / 2.0;

    /// @todo This needs to be set by param, not by conf
    // the sampling rate of the sms data
    smsRate     = gp->sms_rate;

    smsPOS      = 0.0;

    vol         = 1.0;

    blocksize   = bs;


    /// Initialize individual
    /// segment counters to -1
    attackPOS     = -1;
    releasePOS    = -1;
    transitionPOS = -1;

    state    =  inactive;

    partial_printer = new TextOutputManager(gc, ID);

    //  debugOutput.open("partial_amplitude.txt");

    /// @todo Get this as argument from the analysis
    max_partials               = gloooconf->get_max_partials();

    // set a deafult attack length
    lAttack                 = 100;


    // set pointers to the data objects
    stochastic_data         = DR;
    determinisctic_data     = DR1;
    bark_coefficents        = BC;

    nBands                  = bark_coefficents->n_bands();

    /// Intialize the sinusoids
    for(int partCNT=0; partCNT<max_partials; partCNT++)
    {

        // create the sinusoid(s)
        /// @todo CAREFUL: HARD_CODED sampling rate!!
        partials.push_back(new Sinusoid( this->blocksize ,48000, globMat));

        target_amplitudes.push_back(0.0);

        offset_amplitudes.push_back(0.0);

    }

    for(int bandCNT=0; bandCNT<=nBands; bandCNT++)
    {
        target_energies.push_back(0.0);

        band_amps.push_back(0.0);
    }


    // Initialize the residual synth
    res_synth = std::shared_ptr<ResidualSynth>(new ResidualSynth(gp, bark_coefficents, stochastic_data, determinisctic_data));

    // resize the attack/release trajectories
    //part_amp_rel_exp.resize(max_partials);
    part_amp_att_lin.resize(max_partials);

    // resize all transition trajectories:
    part_amp_transitions.resize(max_partials);
    part_freq_transitions.resize(max_partials);
    band_enrg_transitions.resize(max_partials);

    pitch_glissando = new HyperbolicSlope();

    // point to arbitrary release on start
    current_release_model = stochastic_data->nearest_sound(55.0,100.0).get_release_model();

    // heuristic:
    lRelease    = 100;
    randomDepth = 0.1;




}





void SingleVoice::activate(note_event_t note, articulation_style style)
{



    /// 1: initialize basic parameters

    // A newly activated voice always gets the rank 0.
    rank = 0;

    // assign onset  parameters
    //    this->vel       = note.velocity;
    //    this->nn        = note.note;

    this->pitch     = note.note;

    this->intensity = note.velocity;


    // switch state to attack
    state           = attack;
    attackPOS       = 0;


    // reset released
    releasePOS  = -1;


    // (Re)set the position for the deterministic mode
    smsPOS      = 0;

    // get the fundamental frequency for this note number
    f0 = (440.0) * std::pow(2.0, ((double) note.note - 69.0) / 12.0) ;

    if(gloooconf->print_control()==true)
        std::cout << "Creating a new voice with FO: " << f0 << "\n";

    /// 2: loop over all partials set the sinusoid target values
    for(int partCNT=0; partCNT< gp->n_partials; partCNT++)
    {

        // in the deterministic case
        if(gloooconf->get_synthesis_mode().compare("deterministic")==0)
        {
            // set each partial to 0 amplitude and the f0
            partials[partCNT]->set_parameters( f0*((double)partCNT+1.0), 0 , 0);
        }


        // in the stochastic or dual case
        else if(gloooconf->get_synthesis_mode().compare("stochastic")==0 ||
                gloooconf->get_synthesis_mode().compare("dual")==0)
        {

            partials[partCNT]->set_parameters( f0 * ((double)partCNT+1.0), 0 , generate_random());

            // Get the target partial amplitude values
            target_amplitudes[partCNT] = stochastic_data->get_mean_value(pitch, intensity, partCNT, amp);


            // define the nature of the attack segment,
            // according to the trigger event
            // - only attack length, now!
            switch(style)
            {
            case detached:

                lAttack   = stochastic_data->sound(pitch,this->intensity).attack_length();
                break;

            case legato:

                lAttack = 25;
                break;

            case glissando:
                break;

            }

            part_amp_att_lin[partCNT].set_parameters(0.0, target_amplitudes[partCNT], lAttack);

        }

    }

    /// 3: loop over all noise bands
    for(int bandCNT=0; bandCNT<nBands; bandCNT++)
    {
        target_energies[bandCNT] = 0;//stochastic_data->get_stochastic_value((double) note.note  ,note.velocity,bandCNT, band_nrg, glooo::random);
    }

}


void SingleVoice::cycle_start_deterministic()
{

    for(int partCNT=0;partCNT<gp->n_partials; partCNT++)
    {

        double thisAmp  = 0.0;
        double nextAmp  = 0.0;
        double nextFreq = 0.0;

        switch(state)
        {


        case glooo::release:

            // get recent amplitude value
            thisAmp = partials[partCNT]->amplitude();

            // get next amplitude value
            nextAmp =  thisAmp*0.5;

            break;

        default:
            // get recent amplitude value
            thisAmp = partials[partCNT]->amplitude();

            // get next amplitude value
            nextAmp =  determinisctic_data->get_trajectory_value(partCNT, pitch, intensity, smsPOS,  amp);

            // get frequency value from deterministic matrix
            nextFreq = determinisctic_data->get_trajectory_value(partCNT,pitch,intensity,smsPOS, freq);

            // assign to partial
            partials[partCNT]->frequency(nextFreq);

            break;
        }

        // set amplitude interpolator
        partials[partCNT]->set_frame_amplitude_interpolator(thisAmp, nextAmp , blocksize);

    } // end of partial loop

    res_synth->prepare_frame_deterministic(pitch, intensity,  smsPOS, blocksize);


    if(smsRate!=0)
        smsPOS += (1./smsRate);


    // check length
    //if(smsPOS>=determinisctic_data.)

}

void SingleVoice::cycle_start_stochastic()
{

    /// -----------------------------------------------------------------
    /// 1: Loop over all partials and update them for this synthesis block
    /// -----------------------------------------------------------------

    for(int partCNT=0; partCNT < gp->n_partials; partCNT++)
    {

        // remember recent values and allocate new support points
        double this_amp  = partials[partCNT]->amplitude();
        double next_amp  = 0.0;
        double this_freq = partials[partCNT]->frequency();
        double next_freq = 0.0;

        switch(state)
        {

        case inactive:

            // Nothing to do!
            break;

        case glooo::attack:

            if(gloooconf->ATT_mode().compare("linear")==0)
            {

                // update the target amplitude with recent
                target_amplitudes[partCNT] = stochastic_data->get_stochastic_value(gp->pitch, gp->intensity, partCNT, amp, glooo::random, partials[partCNT]->amplitude());

                // update the slope parameter(s)
                part_amp_att_lin[partCNT].update_parameters(target_amplitudes[partCNT]);

                // get slope value
                next_amp = part_amp_att_lin[partCNT].get_value_at(attackPOS);

                // next_freq = this_freq;
                next_freq = f0 * (double) (partCNT+1) * 1.0;
            }

            else if(gloooconf->ATT_mode().compare("original")==0)
            {

                // get next amplitude trajectory support point
                next_amp  = target_amplitudes[partCNT] * determinisctic_data->get_trajectory_value(partCNT, pitch, intensity, attackPOS,  amp);

                // get frequency value from deterministic matrix
                next_freq = determinisctic_data->get_trajectory_value(partCNT, pitch, intensity, attackPOS, freq);

            }

            break;

            // or get values from release trajectories
        case glooo::release:

            next_amp = offset_amplitudes[partCNT] * current_release_model->at(partCNT).get_value_at(releasePOS);

            // next_amp  = 0.5*this_amp;

            next_freq = this_freq;

            break;


            // or get transition trajectories
        case glooo::transition:


            part_amp_transitions[partCNT].update_target_value( stochastic_data->get_stochastic_value(pitch, intensity, partCNT, amp, glooo::random ,this_amp));
            /// @todo Updating partial frequency causes unexpected behavior in glissando transitions.
            // part_freq_transitions[partCNT].update_target_value(stochastic_data->get_stochastic_value(pitch, intensity, partCNT, freq, glooo::random ,this_freq));

            next_amp =  part_amp_transitions[partCNT].get_value_at(transitionPOS);
            next_freq = part_freq_transitions[partCNT].get_value_at(transitionPOS);



            break;


            // in the sustain
        case glooo::sustain:

            // the next relative frequency
            double next_rel = 0;

            // get next amplitude support point
            if(this->gp->part_amp_mode.compare("m")==0)
            {
                next_amp = stochastic_data->get_mean_value(pitch, intensity, partCNT, amp);
            }
            else if(this->gp->part_amp_mode.compare("r")==0)
            {
                next_amp = stochastic_data->get_stochastic_value(pitch, intensity, partCNT, amp, glooo::random, this_amp);
            }

            if(gp->part_freq_mode.compare("m") == 0)
                next_rel  = stochastic_data->get_mean_value(pitch, intensity,partCNT,freq);

            else if(gp->part_freq_mode.compare("s") == 0)
                next_rel = 1.0;

            else
                next_rel = 1.0;

            next_freq = f0 * (double) (partCNT+1) * next_rel;

            break;

        } // end of: switch state FOR THE SEGMENT

        // set the frequency and amplitude interpolator
        partials[partCNT]->set_frame_amplitude_interpolator(this_amp, next_amp, this->blocksize);


        /// @details Appply global pitch shift to all partials (must not be done for release segments).
        if(state == attack || state == sustain || state == transition)
            next_freq = next_freq * gp->pitch_shift;

        partials[partCNT]->frequency(next_freq);

        ///@todo add frequency interpolator
        //double
        //partials[partCNT]->set_frame_frequency_interpolator(partials[partCNT]->frequency(), next_freq, this->blocksize);



        // set direcly (used for debugging)
        // partials[partCNT]->amplitude(next_amp);

        if(gloooconf->print_partials()==true)
        {
            partial_printer->push_amplitude(next_amp, partCNT);

            partial_printer->push_frequency(partials[partCNT]->frequency(), partCNT);
        }




    } // end of loop over partials

    /// --------------------------------------------------------------------------------
    /// 2: process noise bands
    /// --------------------------------------------------------------------------------

    switch (state)
    {
    case attack:
        res_synth->prepare_frame_attack((int) pitch, (int) intensity,  attackPOS,   blocksize, target_energies);
        break;

    case glooo::sustain:
    case glooo::transition:
        res_synth->prepare_frame_stochastic(this->pitch, intensity, this->blocksize);
        break;
    default:
        break;
    }

}


void SingleVoice::cycle_proceed()
{

    switch(state)
    {

    case inactive:
        // Nothing to do!
        break;

        // if we are within the attack
    case glooo::attack:


        attackPOS += (1.0 * smsRate);

        // if end of attack is reached
        if(attackPOS >= lAttack)
        {
            // change this voice's state to sustain
            state     = glooo::sustain;
        }

        break;

    case glooo::release:

        // increment release counter, only if on first sample of frame
        releasePOS += (1.0 * smsRate);

        // deavtivate and set to 0.0 if length of release is exceeded
        if(releasePOS >= lRelease)
        {
            deactivate();
        }

        break;

    case glooo::transition:

        // increment release counter
        transitionPOS += (1.0 * smsRate);

        // if at end
        if(transitionPOS >= lTrans)
        {
            // change state to sustain
            state = glooo::sustain;
        }

        break;
    }
}


void  SingleVoice::get_next_block_bark(
        jack_default_audio_sample_t **out,
        uint32_t n_samples)
{

    /// 1: loop over partials
    for(int partCNT=0; partCNT < gp->n_partials; partCNT++)
    {


        if(gp->partial_activation[partCNT] != 0)
        {

            if(partials[partCNT]->frequency() >= fs_2)
            {
                continue;
            }


            // vector for storing band indices with contribution
            // greater than a hard-coded threshold
            vector<int> relevant_bands;

            // calculate partial-to-frequency-band contribitions
            for (uint bandCNT = 0; bandCNT < nBands; ++bandCNT)
            {

                band_amps[bandCNT]  = bark_coefficents->get_gaussian_amplitude(bandCNT, partials[partCNT]->frequency());

                if(band_amps[bandCNT] > 10e-4)
                    relevant_bands.push_back(bandCNT);
            }


            for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
            {

                // TODO avoid function call.  prefer Sinusoid::Synth(n_samples)
                double part_val = this->vol *  gp->tonal_vol * partials[partCNT]->getNextSample(/*gloooconf->sineMode()*/);

                for(std::vector<int>::iterator it = relevant_bands.begin(); it != relevant_bands.end(); ++it)
                {
                    /// @todo this is tuned heuristically (0.8 to scale level to that of uniform and descrete)
                    /// - bark mode has more energy due to overlapping bands
                    /// - could be quantified by comparing bark band energy output with discrete for white noise
                    out[*it][bufferPOS] += 0.8 * band_amps[*it] * part_val;
                }
            }

        }

    } // end of loop over partials


    /// 2: NOISE LOOP
    /// @todo merge noise loop with partial loop?
    if(state==glooo::attack || state==glooo::sustain || state==glooo::transition)
    {
        for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
        {
            // let the residual synth create ONE new noise sample for all bands
            res_synth->new_random();


            //   The loop over all noise bands
            for(int bandCNT=0; bandCNT<nBands; bandCNT++)
            {

                // int idx = min(bandCNT,nPartials-1);

                out[bandCNT+24][bufferPOS] += this->vol * gp->noise_vol * res_synth->get_next_value(bandCNT);
            }
        }

    } // end of NOISE LOOP
}


void  SingleVoice::get_next_block_uniform(
        jack_default_audio_sample_t **out,
        uint32_t n_samples)
{


    /// 1: loop over partials
    for(int partCNT=0; partCNT < gp->n_partials; partCNT++)
    {

        if(gp->partial_activation[partCNT] != 0)
        {

            if(partials[partCNT]->frequency() >= fs_2)
            {
                continue;
            }


            for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
            {

                // TODO avoid function call.  prefer Sinusoid::Synth(n_samples)
                double part_val = this->vol *  gp->tonal_vol * partials[partCNT]->getNextSample(/*gloooconf->sineMode()*/);

                for(int bandCNT=0; bandCNT<nBands; bandCNT++)
                {
                    out[bandCNT][bufferPOS] += (1.0/24.0) * part_val;
                }
            }

        }

    } // end of loop over partials


    /// 2: NOISE LOOP
    if(state==glooo::attack || state==glooo::sustain)
    {
        for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
        {
            // let the residual synth create ONE new noise sample for all bands
            res_synth->new_random();


            ///   The loop over all noise bands
            for(int bandCNT=0; bandCNT<nBands; bandCNT++)
            {
                out[bandCNT+24][bufferPOS] += this->vol * gp->noise_vol * res_synth->get_next_value(bandCNT);
            }
        }

    } // end of NOISE LOOP


}

void  SingleVoice::get_next_block_discrete(
        jack_default_audio_sample_t **out,
        uint32_t n_samples)
{

    /// 1: loop over partials
    for(int partCNT=0;partCNT<gloooconf->get_max_partials(); partCNT++)
    {

        if(partials[partCNT]->frequency() >= fs_2)
        {
            continue;
        }
        for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
        {

            /// @attention Hard coded number of partials.
            /// @todo avoid function call.  prefer Sinusoid::Synth(n_samples)l
            out[std::min(partCNT,79)][bufferPOS] += this->vol *  gp->tonal_vol * partials[partCNT]->getNextSample();

        }

    } // end of loop over partials

    /// 2: NOISE LOOP
    if(state==glooo::attack || state==glooo::sustain)
    {
        for (uint bufferPOS = 0; bufferPOS < n_samples; ++bufferPOS)
        {
            // let the residual synth create ONE new noise sample for all bands
            res_synth->new_random();


            ///   The loop over all noise bands
            /// @attention Hard coded number of partials.

            for(int bandCNT=0; bandCNT<nBands; bandCNT++)
            {
                out[bandCNT+80][bufferPOS] += this->vol * gp->noise_vol * res_synth->get_next_value(bandCNT);
            }
        }

    } // end of NOISE LOOP


}


void SingleVoice::start_transition(articulation_style as, note_event_t note)
{



    if(gloooconf->print_control()==true)
        std::cout << "Starting Transition to: " << f0 << "\n";

    transitionPOS = 0;
    state         = glooo::transition;

    lTrans        = (int) gp->glissando_duration;


    for(int partCNT=0; partCNT < gp->n_partials; partCNT++)
    {

        // this partial's recent frequency
        double f1 = partials[partCNT]->frequency();

        // the target value of this partial's frequency:
        double f2 = (partCNT+1) * GlobalMath::midi_to_hz(note.note);

        // this partial's recent amplitude
        double a1 = partials[partCNT]->amplitude();

        // the target value of this partial's amplitude:
        double a2 = stochastic_data->get_mean_value(note.note, intensity, partCNT, amp);



        // set transition parameters:
        part_freq_transitions[partCNT].set_parameters(f1, f2,  lTrans, gp->glissando_gamma);
        part_amp_transitions[partCNT].set_parameters(a1,  a2,  lTrans, gp->glissando_gamma);

    }
}

void SingleVoice::deactivate()
{

    // user feedback
    if(gloooconf->print_control()==true)
        cout << "Deactivated note with ID: " << this->ID << endl;

    state = inactive;
    rank  = gloooconf->N_voices();

}


double SingleVoice::getAttackEnvelopeAt(int pos)
{
    double out =   (double) pos / (double) lAttack ;
    return out;
}

double SingleVoice::getReleaseEnvelopeAt(int pos)
{
    //Get amplitude value at a position of the release segment.

    double out = (((double) lRelease- (double) pos)/(double) lRelease);

    return out;
}



void SingleVoice::set_released(articulation_style as)
{

    state       = release;
    releasePOS  = 0;

    // get length of release
    if( gloooconf->get_synthesis_mode().compare("stochastic")==0 ||gloooconf->get_synthesis_mode().compare("dual") ==0 )
    {

        lRelease    = 0.2*stochastic_data->sound(gp->active_note, this->intensity).release_length();


        // shorten release in legato
        if(as==legato)
        {
            /// @remark pure heuristic:
            /// @todo get this value from the model
            lRelease =  30;
        }
    }
    else if(gloooconf->get_synthesis_mode().compare("deterministic")==0)
    {
        lRelease    = stochastic_data->sound(pitch,intensity).release_length();
    }


    // initialize all amplitude release trajectories
    for(int partCNT=0; partCNT<gp->n_partials; partCNT++)
    {

        offset_amplitudes[partCNT] = partials[partCNT]->amplitude();

        current_release_model = stochastic_data->nearest_sound(gp->active_note, this->intensity).get_release_model();

        if(gloooconf->REL_mode().compare("statistic")==0)
        {
            current_release_model->at(partCNT).generate_lambda();
        }

        //part_amp_rel_exp[partCNT].set_parameters(offset_amplitudes[partCNT], 0.0, lRelease, 3.0 + (partCNT + 2.0));

        // part_amp_rel_exp[partCNT].set_statistical_parameters();

    }

}


void SingleVoice::set_pitch(double pb)
{
    this->pitch =  pb;
    f0 = (440.0 ) * std::pow(2.0, (pitch -69) / 12.0);
}


void SingleVoice::volume(double v)
{
    vol = v;
}

void SingleVoice::set_intensity(double intens)
{
    this->intensity = intens;
}


void SingleVoice::random_depth(double r)
{if(gloooconf->get_synthesis_mode().compare("deterministic")==0)
        this->randomDepth = r;
}


void SingleVoice::sms_rate(double r)
{
    this->smsRate = r;
}

int SingleVoice::get_id()
{
    return ID;
}

//int SingleVoice::getNoteNumber()
//{
//    return nn;
//}

//int SingleVoice::getVelocity()
//{
//    return vel;
//}

bool SingleVoice::is_active()
{
    bool out;

    if(state == glooo::inactive)
        out = false;
    else
        out = true;

    return out;
}

glooo::note_state SingleVoice::get_state()
{
    return state;
}


SingleVoice::~SingleVoice()
{

    partial_printer->export_to_file(gp->n_partials);
    debugOutput.close();

}


double SingleVoice::generate_random()
{
    double val = (double) rand() / (double)  numeric_limits<int>::max();
    return val;
}


int SingleVoice::get_rank()
{
    return rank;
}

void SingleVoice::increase_rank()
{
    rank+=1;
}

void SingleVoice::set_rank(int rank)
{
    this->rank = rank;
}



