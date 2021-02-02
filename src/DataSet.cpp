#include "DataSet.h"

DataSet::DataSet()
{

}



int DataSet::pitch_to_index(int pitch)
{

    pitch = max(min(pitch, (int) max_note), (int)  min_note);

    int idx = pitch - min_note;

    idx = max(idx,0);

    return idx;

}

int DataSet::intensity_to_index(double velocity)
{

    int velPos =0;

    if(velocity<40)
    {
        velPos = 0;
    }
    else if(velocity>= 40 && velocity<70)
    {
        velPos = 1;
    }
    else if(velocity>= 70 && velocity<100)
    {
        velPos = 2;
    }
    else if(velocity>= 100)
    {
        velPos = 3;
    }

    return velPos;
}

map<uint, vector<string>> DataSet::get_file_to_note_assignment(string file_path)
{

    if (this->file_to_note_map == NULL)
    {
        map<uint, vector<string>> assignment;
        boost::escaped_list_separator<char> col_separator("", "\t", "");
        boost::escaped_list_separator<char> name_separator("", "_", "");

        ifstream in_stream(file_path,
                           ifstream::in);
        if (!in_stream.is_open()) {
            cout << "Could not open file " << file_path << endl;
            exit(0);
        }
        string line;

        vector<string> vec;

        // skip first line
        getline(in_stream, line);

        for (uint i = 0; getline(in_stream, line); i++)
        {
            if (line.empty()) {
                break;
            }
            tokenizer<escaped_list_separator<char> > tok(line, col_separator);
            vec.assign(tok.begin(), tok.end());
            //map first element of vec as key to a vector to the rest of the elements
            vector<string> velocities(NUM_VELOCITIES);
            for (uint j = 0; j < velocities.size(); j++) {
                if (vec.at(j + 1).empty()) {
                    continue;
                } else {
                    velocities[j] = vec.at(j + 1);
                }
            }
            assignment[stoi(vec.at(0))] = velocities;
        }
        this->file_to_note_map = make_shared<map<uint, vector<string>>>(assignment);
        in_stream.close();
    }
    return *(this->file_to_note_map);

}

uint DataSet::get_lowest_note(){
    return this->min_note;
}

uint DataSet::get_highest_note(){
    return this->max_note;
}
