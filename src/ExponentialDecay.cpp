#include "ExponentialDecay.h"

ExponentialDecay::ExponentialDecay()
{
    distribution =  std::normal_distribution<double>(50.0,10.0);
}


void ExponentialDecay::set_parameters(int L, double lambda)
{
    this->lambda = lambda;
    this->length = L;
}


void ExponentialDecay::set_statistical_parameters(double mean, double std)
{
    this->lambda = lambda;

    distribution = std::normal_distribution<double>(mean,std);

}

void ExponentialDecay::generate_lambda()
{

 this->lambda = std::max(0.1, distribution(generator));

}

double ExponentialDecay::get_next_value()
{

}

double ExponentialDecay::get_value_at(double pos)
{
    double val = (-(pos / ((double)length) ) + 1.0) * std::exp(-(lambda * pos) / (double) length);
    return val;
}


