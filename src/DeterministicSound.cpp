
#include "DeterministicSound.h"



using namespace std;
using namespace glooo;

DeterministicSound::DeterministicSound(string directory, string fileName, uint nPart)
{

    nBands = 25;

    file_name = fileName;
    string path;

    path = directory    + fileName + ".AMPL";
    fill_trajectories(partial_amplitudes, path, nPart, "AMPL");

    path = directory    + fileName + ".FREQ";
    fill_trajectories(partial_frequencies, path, nPart, "FREQ");

    path = directory    + fileName + ".BBE";
    fill_trajectories(noise_energies, path, nBands, "BBE");

    L = noise_energies.size();

}



void DeterministicSound::fill_trajectories(std::vector<std::vector<double>> &A, string fileName, uint num_columns, string type)
{

    typedef tokenizer<escaped_list_separator<char> > Tokenizer;
    boost::escaped_list_separator<char> separator("", " ", "");
    double value;

    ifstream in_stream(fileName.c_str(), ifstream::in);

    if (!in_stream.is_open())
    {
        cout << "Could not open file " << fileName.c_str() << endl;
        throw new string("File not found exception: " + fileName + "\n");
    }

    string line;


    in_stream.close();
    ifstream in_stream2(fileName.c_str(), ifstream::in);

    vector<string> vec;
    list<valarray<double>> partial_vec_list;

    for (uint i = 0; getline(in_stream2, line); i++)
    {

        Tokenizer tok(line, separator);
        vec.assign(tok.begin(), tok.end());

        A.push_back(std::vector<double>());

        valarray<double> data_vec(num_columns);
        for (uint j = 0; j < num_columns; j++)
        {
            value       = strtof(vec.at(j).c_str(), NULL);
            A[i].push_back(value);
        }

        partial_vec_list.push_back(data_vec);

    }

    // voice_matrix_t partial_matrix(partial_vec_list.begin(), partial_vec_list.end());

    in_stream.close();

}

double DeterministicSound::get_trajectory_value(double pos, int IDX,  param_type_e type)
{

    double out = 0.0;

    if(pos>=0 && pos <=L)
    {
        // interpolate
        int lowerSP = (int) floor(pos);
        double frac = pos - (double) lowerSP;
        int upperSP = (int) (lowerSP + 1);

        double tmp_L = 0.0;
        double tmp_U = 0.0;


        switch (type) {
        case amp:
            tmp_L =  this->partial_amplitudes[lowerSP][IDX];
            tmp_U =  this->partial_amplitudes[upperSP][IDX];
            break;

        case freq:
            tmp_L =  this->partial_frequencies[lowerSP][IDX];
            tmp_U =  this->partial_frequencies[upperSP][IDX];
            break;

        case band_nrg:
            tmp_L =  this->noise_energies[lowerSP][IDX];
            tmp_U =  this->noise_energies[upperSP][IDX];
            break;

        default:
            break;
        }

        out = tmp_L*(1-frac) + tmp_U*frac;

    }
    else
    {
        out = 0.0;
    }

    return out;

}

int DeterministicSound::length()
{
    return L;
}
