/// @class FrameInterpolator
///
/// @brief manages the interpolation within a frame
///
/// @author Henrik von Coler
/// @date   2017-10-23
///

#ifndef FRAMEINTERPOLATOR_H
#define FRAMEINTERPOLATOR_H


#include "Trajectory.h"

class FrameInterpolator : public Trajectory

{

private:

    ///  The slope
    double m;


public:

    FrameInterpolator();

    void set_parameters(double start, double stop, int L);

    ///
    /// \brief get_next_value
    /// \return a value of the interpolated trajectory
    ///
    double get_next_value();

};

#endif // FRAMEINTERPOLATOR_H
