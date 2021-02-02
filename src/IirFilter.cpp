#include "IirFilter.h"

IirFilter::IirFilter()
{
}

void IirFilter::init(std::shared_ptr<BarkCoefficients> BC, int band)
{

    order       = BC->get_order();
    N           = BC->get_N();

    b           = new double[N];
    a           = new double[N];
    out_buff    = new double[N];
    in_buff     = new double[N];

    // fill coefficients
    /// @todo This could be more efficient
    for(int i=0; i<N; i++)
    {

        double  tmpA = BC->get_a(band,i);
        double  tmpB = BC->get_b(band,i);

        a[i] = tmpA;
        b[i] = tmpB;

        out_buff[i] = 0.0;
        in_buff[i]  = 0.0;
    }
}

double IirFilter::process_value(double input)
{


    // shift input buffer
    for(int i=N-1;i>0;i--)
        in_buff[i] = in_buff[i-1];

    // write input value
    in_buff[0] = input;

    // allocate output value
    double output = 0.0;

    // apply filter
    for(int i=0;i<N;i++)
    {
        output += in_buff[i]*b[i];
    }

    for(int i=1;i<N;i++)
    {
        output -= out_buff[i-1]*a[i];
    }

    // shift output buffer
     for(int i=N-1;i>0;i--)
        out_buff[i] = out_buff[i-1];

    // write input value
    out_buff[0] = output;

    return output;

}
