/**
 * \class Sinusoid
 *
 *
 * \brief The Sinusoid Class ...
 *
 *
 * \author Henrik von Coler
 *
 * \version  0.6
 *
 * \date   2017/10/06
 *
 * Contact: von_coler@tu-berlin.de
 *
 *
 */

#ifndef SINUSOID_H
#define SINUSOID_H

#define _USE_MATH_DEFINES

#include<vector>
#include<cmath>
#include<valarray>
#include<vector>

#include "settings.h"
#include "StochasticData.h"
#include "frameinterpolator.h"
#include "GlobalMath.h"

using namespace std;


class Sinusoid
{


public:

    Sinusoid(int blocksize , int fS , GlobalMath *GM);

    void set_parameters(double f, double a, double p);

    void proceed(double ms);


    double frequency() const;
    double amplitude() const;
    double phase() const;

    void frequency(double f);
    void amplitude(double a);
    void phase(double p);


    float getNextSample();


    valarray<double>  lookup(valarray<double> arg, uint return_size);

    void set_frame_amplitude_interpolator(double start, double stop, int L);
    void set_frame_frequency_interpolator(double start, double stop, int L);


private:

    GlobalMath *globMat;

    /// position within the frame interpolator
    // PURE SINE STATE
    float freq;
    float amp;
    double phi;

    // SYSTEM RELATED
    int nframes;
    int fs;

    double fs_1;
    double delta_phi;

    double *t;

    vector<double> sin_table;

    FrameInterpolator *frameAmpInt;
    FrameInterpolator *frameFreqInt;



};

#endif // SINUSOID_H
