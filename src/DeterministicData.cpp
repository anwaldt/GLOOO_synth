/**
 *
 * @file DeterministicData.cpp
 * @class DeterministicData
 *
 *  @date 02.06.2016
 *
 *      @authors  Henrik von Coler, Benjamin Wiemann
 */

#include "DeterministicData.h"

DeterministicData::DeterministicData(){

    // will be overwritten by set_frame_size()
    this->frame_size = DEFAULT_FRAME_SIZE;

}

DeterministicData::~DeterministicData()
{

}


void DeterministicData::load_deterministic(string file_directory, string assignment_file, uint num_partials){

    // Get note to sample assignment:
    map<uint, vector<string>> note_to_sample =
            this->get_file_to_note_assignment(assignment_file);

    //map<uint, vector<string>>::const_iterator it = note_to_sample.begin();

    this->min_note      = note_to_sample.begin()->first;
    this->max_note      = note_to_sample.rbegin()->first;


    cout << "Reading deterministic data" << endl << endl;


    this->num_partials  = num_partials;


    /// get note to sample assignment
    note_to_sample = this->get_file_to_note_assignment(assignment_file);

    /// read  data
    int noteCNT = 0;
    for (auto const &entry : note_to_sample)
    {

        if((entry.first > max_note + 1 && max_note > 0) || entry.first < min_note)
        {
            continue;
        }

        sound_matrix.push_back(std::vector<DeterministicSound>());

//        list<voice_matrix_t> velocity_list(0);
//        list<voice_matrix_t> freq_list(0);
//        list<voice_matrix_t> bark_list(0);

        // iterate notes

        for (string file : entry.second)
        {

            std::cout << "Loading deterministic data of: " << file  << endl;

            sound_matrix[noteCNT].push_back(DeterministicSound(file_directory, file, num_partials));

//             velocity_list.push_back(this->read_matrix(file_directory    + file + ".AMPL", num_partials));
//             freq_list.push_back(this->read_matrix(file_directory        + file + ".FREQ", num_partials));
//             bark_list.push_back(this->read_matrix(file_directory        + file + ".BBE", 24));

        }

//        this->partial_amp_collection[entry.first]   = vector<voice_matrix_t>(velocity_list.begin(), velocity_list.end());
//        this->partial_freq_collection[entry.first]  = vector<voice_matrix_t>(freq_list.begin(), freq_list.end());
//        this->bark_energy_collection[entry.first]   = vector<voice_matrix_t>(bark_list.begin(), bark_list.end());

        noteCNT++;
    }

}

voice_matrix_t DeterministicData::read_matrix(string file_path,
                                              uint num_colums)
{

    cout << "Reading: " << file_path  << endl;

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
    boost::escaped_list_separator<char> separator("", " ", "");
    double value;

    ifstream in_stream(file_path.c_str(), ifstream::in);

    if (!in_stream.is_open())
    {
        cout << "Could not open file " << file_path.c_str() << endl;
        throw new string("File not found exception: " + file_path + "\n");
    }

    string line;


    in_stream.close();
    ifstream in_stream2(file_path.c_str(), ifstream::in);

    vector<string> vec;
    list<valarray<double>> partial_vec_list;

    for (uint i = 0; getline(in_stream2, line); i++)
    {

        Tokenizer tok(line, separator);
        vec.assign(tok.begin(), tok.end());

        valarray<double> data_vec(num_colums);
        for (uint j = 0; j < num_colums; j++)
        {
            value       = strtof(vec.at(j).c_str(), NULL);
            data_vec[j] = value;
        }

        partial_vec_list.push_back(data_vec);

    }

    voice_matrix_t partial_matrix(partial_vec_list.begin(), partial_vec_list.end());

    in_stream.close();
    return partial_matrix;

}


uint DeterministicData::get_num_partials(){
    return this->num_partials;
}

void DeterministicData::set_frame_size(uint frame_size) {
    this->frame_size = frame_size;
}


double DeterministicData::get_trajectory_value(int partialID, int noteNumber, int velocity, double idx, param_type_e TYPE)
{


    // get indices for control values
    int velPos  = intensity_to_index(velocity);
    int noteIDX = pitch_to_index(noteNumber);


    double out = sound_matrix[noteIDX][velPos].get_trajectory_value(idx, partialID,  TYPE);

    return out;

}





