#ifndef HYPERBOLICSLOPE_H
#define HYPERBOLICSLOPE_H


#include"Trajectory.h"
#include<cmath>
#include <algorithm>

/// @class HyperbolicSlope
///
/// @brief The HyperbolicSlope class
/// intended for use in glissando transitions
///
/// @inherits Trajectory
///
/// @author Henrik von Coler
/// @date   2019-11-18
///


class HyperbolicSlope : public Trajectory
{

public:

    HyperbolicSlope();

    void set_parameters(double y_0, double y_1, int N, double gamma);

    ///
    /// \brief Update the target value
    /// \param y_1
    ///
    void update_target_value( double y_1);

    double get_next_value();

    ///
    /// \brief Get a value from the Trajectory.
    /// \param pos A fractional position.
    /// \return Trajectory value at position within the trajectory.
    ///
    double get_value_at(double pos);

private:

    double sign;
    double gamma;

};

#endif // HYPERBOLICSLOPE_H
