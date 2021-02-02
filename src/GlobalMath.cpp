/*!
 * @file GlobalMath.cpp
 *
 *  @date  2016-08-29
 *  @author Benjamin Wiemann
 */

#include "GlobalMath.h"

GlobalMath::GlobalMath(uint frame_size, uint sample_rate, unsigned seed){
    this->set_sample_rate(sample_rate);
    this->set_frame_size(frame_size);
    this->init_frequency_table();
    this->init_sine_table();
    this->random_generator = mt19937(seed);
}

GlobalMath::GlobalMath(uint frame_size, uint sample_rate) :
    GlobalMath(frame_size, sample_rate, std::chrono::system_clock::now().time_since_epoch().count()){

}

GlobalMath::~GlobalMath(){
}

void GlobalMath::init_frequency_table() {
    this->frequency_table = vector<double>(NUM_NOTES);
    for (uint i = 0; i < NUM_NOTES; i++) {
        double freq = (2.0 * 440.0 / 32.0)
                * pow(2, (((double) i - 9.0) / 12.0));
        this->frequency_table[i] = freq;
    }
}

void GlobalMath::init_time_vector() {

    this->time_vector = valarray<double>(frame_size + 1);
    this->time_vector_quadratic = valarray<double>(frame_size + 1);
    for (uint i = 0; i < time_vector.size(); i++) {
        this->time_vector[i] = (double) i / this->sample_rate;
        this->time_vector_quadratic[i] = this->time_vector[i] * this->time_vector[i];
    }
}

void GlobalMath::init_sine_table() {
    uint n_entries = (uint) pow(2, SINE_RESULUTION);
    this->sin_table = vector<double>(n_entries+1);
    for (uint i = 0; i <= n_entries; i++) {
        this->sin_table[i] = sin((M_PI / 2) * i / n_entries);
    }
}

double GlobalMath::get_note_frequency(int note) {
    return this->frequency_table[note];
}

void GlobalMath::set_frame_size(uint frame_size) {
    this->frame_size = frame_size;
    if (this->sample_rate > 0) {
        this->init_time_vector();
    }
}

void GlobalMath::set_sample_rate(uint sample_rate) {

    this->sample_rate = sample_rate > 0 ? sample_rate : DEFAULT_SAMPLE_RATE;
    if (this->frame_size > 0) {
        this->init_time_vector();
    }
}

const valarray<double>& GlobalMath::get_time() const {
    return this->time_vector;
}

const valarray<double>& GlobalMath::get_quad_time() const {
    return this->time_vector_quadratic;
}

int GlobalMath::get_sample_rate() {
    return this->sample_rate;
}

int GlobalMath::get_frame_size(){
    return this->frame_size;
}




valarray<double> GlobalMath::sin_lookup(valarray<double> arg, uint return_size) {

    int key;
    double t_arg;
    int fac = (int) pow(2, (double) SINE_RESULUTION);
    valarray<double> res(0.0, return_size);
    // For each value, convert it to an integer between 0 and a given upper border.
    // Use this value as the index for the lookup table.
    for (uint i = 0; i < return_size; i++) {
        // reduce values to interval [0, 2*PI)
        if(isnan(arg[i])){
            res[i] = 0;
            continue;
        }
        t_arg = fmod(arg[i], 2 * M_PI);
        if(t_arg < M_PI){
            if(t_arg < M_PI/2){
                key = (int) round(t_arg/(M_PI/2) * fac);
            }else{
                key = (int) round((M_PI - t_arg)/(M_PI/2) * fac);
            }
            res[i] = this->sin_table.at(key);
        }else{
            t_arg = t_arg - M_PI;
            if(t_arg < M_PI/2){
                key = (int) round(t_arg/(M_PI/2) * fac);
            }else{
                key = (int) round((M_PI - t_arg)/(M_PI/2) * fac);
            }
            res[i] = - this->sin_table.at(key);
        }
    }
    return res;
}

double GlobalMath::single_sin_lookup(double arg)
{

    double res;

    int key;

    int fac = (int) pow(2, (double) SINE_RESULUTION);

    double t_arg = fmod(arg, 2 * M_PI);

    if(t_arg < M_PI)
    {
        if(t_arg < M_PI/2)
        {
            key = (int) round(t_arg/(M_PI/2) * fac);
        }
        else
        {
            key = (int) round((M_PI - t_arg)/(M_PI/2) * fac);
        }

        if(key<sin_table.size())
        res = this->sin_table.at(key);
    }
    else
    {
        t_arg = t_arg - M_PI;
        if(t_arg < M_PI/2)
        {
            key = (int) round(t_arg/(M_PI/2) * fac);
        }
        else
        {
            key = (int) round((M_PI - t_arg)/(M_PI/2) * fac);
        }

        if(key<sin_table.size())
        res = - this->sin_table.at(key);

    }

    return res;
}

mt19937& GlobalMath::get_generator(){
    return this->random_generator;
}



double GlobalMath::midi_to_hz(double midi_pitch)
{
    double f0 = (440.0 ) * std::pow(2.0, (midi_pitch -69) / 12.0);
    return f0;
}
