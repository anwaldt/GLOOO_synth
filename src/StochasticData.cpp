///
/// @file StochasticData.cpp
/// @date 2017-03-11
/// @authors Benjamin Wiemann, Henrik von Coler
///

#include "StochasticData.h"

StochasticData::StochasticData(GloooConfig *GC)
{

    srand(time(NULL));

    this->gc = GC;

    A = 0;
    B = 0;
    C = 0;
    D = 0;
}

StochasticData::~StochasticData() {
    // TODO Auto-generated destructor stub
}

void StochasticData::load_stochastic(string assigment_file, uint num_partials)
{

    // USER Feedback
    cout << "Reading stochastic data! \n \n";

    this->num_partials  = num_partials;

    // Get note to sample assignment:
    map<uint, vector<string>> note_to_sample =
            this->get_file_to_note_assignment(assigment_file);

    //map<uint, vector<string>>::const_iterator it = note_to_sample.begin();

    this->min_note      = note_to_sample.begin()->first;
    this->max_note      = note_to_sample.rbegin()->first;

    // iterate notes
    int noteCNT = 0;
    for(auto const &note : note_to_sample)
    {

        sound_matrix.push_back(std::vector<StochasticSound>());

        //iterate velocities
        uint intCNT = 0;
        for( const string file : note.second){

            // user info:
            string XXX = gc->yaml_path()  + file + ".yml";

            cout << "Loading stochastic data of: " << file << endl;

            // load YAML file
            YAML::Node node = YAML::LoadFile(gc->yaml_path()  + file + ".yml");

            sound_matrix[noteCNT].push_back(StochasticSound(node,file));

            intCNT++;

        }
        noteCNT++;
    }
}


double StochasticData::get_stochastic_value(double pitch, double intensity, uint IDX, param_type_e type, glooo::stat_mode mode, double last_value)
{

    // allocate output
    double out = 0.0;

    pitch = std::min(pitch, (double) max_note);

    // Get the velocity index
    if(intensity>=127)
        intensity=127.0;

    double velRel = velocity_to_index(intensity);

    // interpolate for velocity
    int lowerVel   = floor(velRel);
    double fracVel = velRel - (double) lowerVel;
    int upperVel;
    if(fracVel==0.0)
        upperVel = lowerVel;
    else
        upperVel   = (int) (lowerVel + 1);

    // interpolate for note
    int lowerNote   = floor(pitch);
    double fracNote = pitch - (double) lowerNote;
    int upperNote   = (int) (lowerNote+1);

    // check for note boundaries
    upperNote = std::min((uint) upperNote,max_note);
    lowerNote = std::max((uint) lowerNote,min_note);

    double upperIDX = pitch_to_index(upperNote);
    double lowerIDX = pitch_to_index(lowerNote);

    if(gc->print_stochastic()==true)
    {
        cout << "Interpolating - " << endl;
        cout << "between pitches:" << "\t\t" << lowerNote << " - " << upperNote  << endl;
        cout << "betwen intensities (1...4):" <<"\t" << lowerVel << " - " << upperVel  << endl;
    }


    // we need one random value to pick from each distribution
    double randVal;
    switch(mode)
    {
    case glooo::median:
        randVal = 0.5;
        break;
    case glooo::random:
        randVal = generate_random();
        break;
    }



    double tmpVAL = 0.0;

    // get last values from relevant buffers

    switch (type) {
    case freq:
        A = part_freq_buffer[0];
        B = part_freq_buffer[1];
        C = part_freq_buffer[2];
        D = part_freq_buffer[3];
        break;

    case amp:
        A = part_amp_buffer[0];
        B = part_amp_buffer[1];
        C = part_amp_buffer[2];
        D = part_amp_buffer[3];
        break;

    case band_nrg:
        A = bark_energy_buffer[0];
        B = bark_energy_buffer[1];
        C = bark_energy_buffer[2];
        D = bark_energy_buffer[3];
        break;

    default:
        break;
    }

    // low note and low velocity
    A = sound_matrix[lowerIDX][lowerVel].get_stochastic_parameter(type, IDX, A, randVal);
    double a = (1-fracNote) * (1-fracVel) * A;

    // low note and high velocity
    B = sound_matrix[lowerIDX][upperVel].get_stochastic_parameter(type, IDX, B, randVal);
    double b = (1-fracNote) * fracVel * B;

    // high note and low velocity
    C = sound_matrix[upperIDX][lowerVel].get_stochastic_parameter(type, IDX, C, randVal);
    double c = fracNote * (1-fracVel) * C;

    // high note and high velocity
    D = sound_matrix[upperIDX][upperVel].get_stochastic_parameter(type, IDX, D, randVal);
    double d = fracNote * fracVel * D;



    // write output values into relevant buffers
    switch (type) {
    case freq:
        part_freq_buffer[0] = A;
        part_freq_buffer[1] = B;
        part_freq_buffer[2] = C;
        part_freq_buffer[3] = D;
        break;

    case amp:
        part_amp_buffer[0] = A;
        part_amp_buffer[1] = B;
        part_amp_buffer[2] = C;
        part_amp_buffer[3] = D;
        break;

    case band_nrg:
        bark_energy_buffer[0] = A;
        bark_energy_buffer[1] = B;
        bark_energy_buffer[2] = C;
        bark_energy_buffer[3] = D;
        break;

    default:
        break;
    }

    out = a+b+c+d;

    return out;
}


double StochasticData::get_mean_value(double pitch, double intensity, int IDX, param_type_e type)
{


    /// @todo this is almost a copy of  get_stochastic_value()

    if(intensity>=127)
        intensity=127.0;

    double out = 0.0;

    // Get the velocity index
    double velRel = velocity_to_index(intensity);

    // interpolate for velocity
    int lowerVel   = floor(velRel);
    double fracVel = velRel - (double) lowerVel;
    int upperVel   = (int) (lowerVel + 1);

    // interpolate for note
    int lowerNote   = floor(pitch);
    double fracNote = pitch - (double) lowerNote;
    int upperNote   = (int) (lowerNote+1);


    // check for note boundaries
    upperNote = std::min((uint) upperNote,max_note);
    lowerNote = std::max((uint) lowerNote,min_note);

    double upperIDX = pitch_to_index(upperNote);
    double lowerIDX = pitch_to_index(lowerNote);

    double tmpVAL;

    // interpolate, if level inside bounds
    if(lowerNote>=0 && lowerVel>=0 && upperVel<=3)
    {

        // low note and low velocity
        tmpVAL = sound_matrix[lowerIDX][lowerVel].get_mean_parameter(type, IDX);
        double a = (1-fracNote) * (1-fracVel) * tmpVAL;

        // low note and high velocity
        tmpVAL = sound_matrix[lowerIDX][upperVel].get_mean_parameter(type, IDX);
        double b = (1-fracNote) * fracVel * tmpVAL;

        // high note and low velocity
        tmpVAL = sound_matrix[upperIDX][lowerVel].get_mean_parameter(type, IDX);
        double c = fracNote * (1-fracVel) * tmpVAL;

        // high note and high velocity
        tmpVAL = sound_matrix[upperIDX][upperVel].get_mean_parameter(type, IDX);
        double d = fracNote * fracVel * tmpVAL;

        //
        out = a + b + c + d;

    }
    return out;
}



double StochasticData::get_trajectory_value(double pitch , double velocity, int partCNT, double pos, param_type_e type,  note_state mode)
{

    double out = 0.0;

    int pitchIDX_restricted = max(min((int) pitch, (int) max_note), (int)  min_note);

    // shift pitch to index
    int pitchIDX = pitch_to_index(pitchIDX_restricted);

    // Get the velocity index
    int velIDX = velocity_to_index(velocity);

    switch(type)
    {
    case freq:
        switch(mode)
        {
        case glooo::attack:
            out = sound_matrix[pitchIDX][velIDX].get_freq_att(pos, partCNT);
            break;
        case glooo::release:
            out = sound_matrix[pitchIDX][velIDX].get_freq_rel(pos, partCNT);
            break;
        }
        break;
    case amp:
        switch(mode)
        {
        case glooo::attack:
            out = sound_matrix[pitchIDX][velIDX].get_amp_att(pos, partCNT);
            break;
        case glooo::release:
            out = sound_matrix[pitchIDX][velIDX].get_amp_rel(pos, partCNT);
            break;
        case glooo::sustain:
            break;
        case glooo::inactive:
            break;
        }
        break;
    case band_nrg:
        switch(mode)
        {
        case glooo::attack:
            out = sound_matrix[pitchIDX][velIDX].get_enrg_att(pos, partCNT);
            break;
        case glooo::release:
            out = sound_matrix[pitchIDX][velIDX].get_enrg_rel(pos, partCNT);
            break;
        case glooo::sustain:
            break;
        case glooo::inactive:
            break;
        }
        break;
    }

    return out;
}

StochasticSound &StochasticData::sound(int nn,int intensity)
{

    int noteIDX = pitch_to_index(nn);
    int velIDX  = velocity_to_index(intensity);

    return sound_matrix[noteIDX][velIDX];

}

StochasticSound &StochasticData::nearest_sound(double nn,double intensity)
{

    int NN  = round(nn);
    int INT = round(intensity);


    int noteIDX = pitch_to_index(NN);
    int velIDX  = velocity_to_index(INT);

    return sound_matrix[noteIDX][velIDX];

}

double StochasticData::velocity_to_index(double velocity)
{
    double velIDX = ( velocity / 127.0) * 3.0;

    return velIDX;
}




