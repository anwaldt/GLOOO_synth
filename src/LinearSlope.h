#ifndef LINEARSLOPE_H
#define LINEARSLOPE_H

#include "Trajectory.h"

/// @class LinearSlope
///
/// @brief The LinearSlope class
///
/// @inherits Trajectory
///
/// @author Henrik von Coler
/// @date   2018-11-13
///

class LinearSlope : public Trajectory
{


private:

///  The slope
    double m;



public:

    LinearSlope();

    void set_parameters(double start, double stop, int L);

    ///
    /// \brief update_parameters
    /// update the target value
    /// \param stop
    ///
    void update_parameters(double stop);

    double get_next_value();

    double get_value_at(double pos);

    double get_length()
    {
        return length;
    }


};

#endif // LINEARSLOPE_H
