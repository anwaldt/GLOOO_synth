/**
 *
 * @file StochasticTester.cpp
 * @class StochasticTester
 *
 * @brief small wrapper for testing the Markovian approach
 *
 * @date 2019-08-11
 *
 * @author Henrik von Coler
 */



#ifndef STOCHASTICTESTER_H
#define STOCHASTICTESTER_H



#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <csignal>
#include <vector>
#include <memory>
#include <limits>

#include <cmath>

#include "glooo.h"
#include "settings.h"
#include "DeterministicData.h"
#include "StochasticData.h"
#include "VoiceManager.h"
#include "GloooConfig.h"
#include "TextOutputManager.h"
#include "BarkCoefficients.h"


class StochasticTester
{
public:
    StochasticTester();

private:

    GloooConfig *gloooconf;

    TextOutputManager *textOutMan;

    std::shared_ptr<DeterministicData>  deterministic_data;
    std::shared_ptr<StochasticData>     stochastic_data;

};

#endif // STOCHASTICTESTER_H
