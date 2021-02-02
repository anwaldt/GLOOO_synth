#include "Sinusoid.h"




/// The constructor does allmost nothing at this point.
/// just setting the permanent values
Sinusoid::Sinusoid( int blocksize , int fS, GlobalMath *GM)
{
    this->nframes = blocksize;
    this->fs      = fS;
    this->fs_1    = 1.0 / (double)fS;

    frameAmpInt   = new FrameInterpolator();
    frameFreqInt  = new FrameInterpolator();

    globMat = GM;

}

void Sinusoid::set_parameters(double f, //!< The instantaneous frequency
                              double a, //!< The instantaneous amplitude
                              double p //!< The instantaneous phase
                              )
{
    frequency (f);
    amplitude (a);
    phase (p);
}

/**
 * @brief Get the next sinusoid value
 *
 This method gets the next sample of the sinusoid.
 The amplitude is applied and the angle of the
 sinusoid is increased, according to the sample rate
 and frequency.
*
* @return thisVal float value -> the sinusoid output
 */

float Sinusoid::getNextSample()
{
    amp = frameAmpInt->get_next_value();

    const float thisVal = sinf (phi) * amp;

    phi = fmodf (phi + delta_phi, 2 * M_PI);

    return thisVal;
}


/**

 @brief sets the interpolator for one frame

 @param start first value
  @param stop last value
 @param L length of the interpolator (usually buffer size)

*/

void Sinusoid::set_frame_amplitude_interpolator(double start, double stop, int L)
{

    frameAmpInt->set_parameters(start, stop, L);
    //amp = start;

}

void Sinusoid::set_frame_frequency_interpolator(double start, double stop, int L)
{
    frameFreqInt->set_parameters(start, stop, L);

}


/// @brief Getter for the sinusoid's frequency
///
double Sinusoid::frequency() const
{
    return this->freq;
}

/// @brief Getter for the sinusoid's amplitude
///
double Sinusoid::amplitude() const
{
    return amp;
}

/// @brief Getter for the sinusoid's phase
///
double Sinusoid::phase() const
{
    return phi;
}

/// @brief Setter for the sinusoid's frequency
///
void Sinusoid::frequency(double f)
{
    this->freq = f;
    delta_phi = 2.0 * M_PI * f * fs_1;
}



/// @brief Setter for the sinusoid's amplitude
///
void Sinusoid::amplitude(double a)
{
    amp = a;
}

/// @brief Setter for the sinusoid's phase
///
void Sinusoid::phase(double p)
{
    phi = p;
}


/// @brief Sine lookup table
///
///
///
/// @author Wiemann

valarray<double> Sinusoid::lookup(valarray<double> arg, uint return_size) {

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
