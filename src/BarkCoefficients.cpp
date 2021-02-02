#include "BarkCoefficients.h"

BarkCoefficients::BarkCoefficients(GloooConfig *gc)
{

    std::cout << "Reading bark-filter coeficients from: " << gc->bark_file() << std::endl << endl;


    // Read basic file
    YAML::Node bark_filter = YAML::LoadFile(gc->bark_file());

    // The output node
    YAML::Node param;
    param = bark_filter["PARAMETERS"];

    fs     = (int) param["fs"].as<double>();
    order  = (int) param["order"].as<double>();
    nCoeff = order*2+1;

    std::cout << "Using order : " << order << " bandpasses!" << std::endl << endl;

    // The output node
    YAML::Node coefficients;

    coefficients = bark_filter["COEFFICIENTS"];

    nBands = coefficients.size();

    B = new double*[nBands];
    A = new double*[nBands];

    for(int i=0; i<nBands; i++)
    {
        B[i] = new double[nCoeff];
        A[i] = new double[nCoeff];
    }

    int counter = 0;

    // iterate
    for(auto const &band : coefficients)
    {

        // grab id (int) from YAML node name
        string ID   = band.first.as<std::string>();
        string ind  = ID.replace(0,5,"");
        int b_index = std::stoi(ind, nullptr);


        if(b_index > nBands)
        {
            continue;
        }

        YAML::Node b = band.second["b"];
        YAML::Node a = band.second["a"];


        for(uint i= 0; i < b.size(); i++ )
        {
            // cout << b[i].as<double>() << endl;
            B[b_index-1][i] = b[i].as<double>();
        }
        for(uint i= 0; i < a.size(); i++ )
        {
            // cout << a[i].as<double>() << endl;
            A[b_index-1][i] = a[i].as<double>();
        }


        counter ++;
    }





    // The gaussian node
    YAML::Node gaussian_node;

    gaussian_node = bark_filter["GAUSS"];

    for(int i=1; i<nBands; i++)
    {
        gauss.push_back(glooo::gaus_dist());
    }


    //gauss.resize(nBands);


    // iterate
    for(auto const &band : gaussian_node)
    {

        // grab id (int) from YAML node name
        string ID   = band.first.as<std::string>();
        string ind  = ID.replace(0,5,"");
        int b_index = std::stoi(ind, nullptr)-1;


        if(b_index > nBands)
        {
            continue;
        }

        YAML::Node m = band.second["mu"];
        YAML::Node s = band.second["sigma"];

        glooo::gaus_dist tmp_dist;

        tmp_dist.mu    = m.as<double>();
        tmp_dist.sigma = s.as<double>();


        gauss[b_index].mu    = m.as<double>();
        gauss[b_index].sigma = s.as<double>();
    }
}


double BarkCoefficients::get_a(int band, int idx)
{
    return A[band][idx];
}

double BarkCoefficients::get_b(int band, int idx)
{
    return B[band][idx];
}


int BarkCoefficients::n_bands()
{
    return nBands;
}


int BarkCoefficients::get_order()
{
    return order;
}

int BarkCoefficients::get_N()
{
    return nCoeff;
}

double BarkCoefficients::get_gaussian_amplitude(int band, double frequency)
{

  double out = exp(-pow(  ((frequency-gauss[band].mu)  / gauss[band].sigma), 2));

  return out;

}
