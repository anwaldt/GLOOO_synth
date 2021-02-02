/**
*
*   @class Trajectory
*
*   @brief The class for defining a trajectory.
*
*   Supposed to be a template.
*
*   @author Henrik von Coler
*   @date 2017-10-13
*
*/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H


class Trajectory
{

public:

    Trajectory();

    virtual double get_next_value();

    virtual double get_value_at(double pos);

    virtual double set_parameters();

protected:

    ///
    /// \brief The first value of the trajectory
    ///
    float startVal;


    ///
    /// @brief The last value of the trajectory
    ///
    float stopVal;


    ///
    /// @brief Number of support points
    ///
    int length;

    ///
    /// \brief In a stat-machine mode, this is the recent position.
    ///
    int recentPos;


};

#endif // TRAJECTORY_H
