/**
 *
 * @file StochasticSound.cpp
 * @class StochasticSound
 *
 * @date 2017-03-11
 *
 * @author Henrik von Coler
 */

#include "StochasticSound.h"

StochasticSound::StochasticSound(YAML::Node node, string fileName)
{

    file_name = fileName;

    // get parameters
    // int n_partials = min((int) num_partials, (int) node["param"]["PART"]["nPartials"].as<double>());

    // get the nodes for the three segments
    YAML::Node stochastic_partials           = node["SUS"]["PARTIALS"];
    // YAML::Node trajectories_attack_partials  = node["ATT"]["PARTIALS"];
    YAML::Node release_partials = node["REL"]["PARTIALS"];


    att_start = (int) node["ATT"]["start"].as<double>();
    att_stop  = (int) node["ATT"]["stop"].as<double>();
    rel_start = (int) node["REL"]["start"].as<double>();
    rel_stop  = (int) node["REL"]["stop"].as<double>();


    l_attack  = att_stop-att_start;
    l_release = rel_stop-rel_start;

    nPartials = stochastic_partials.size();
    partial_amplitudes.resize(nPartials);


    for(auto const &partial : stochastic_partials)
    {
        string ind  = partial.first.as<std::string>();
        ind         = ind.replace(0,2,"");
        int p_index = std::stoi(ind, nullptr)-1;

        partial_amplitudes[p_index].init(partial, "AMP");
    }

    partial_frequencies.resize(nPartials);

    for(auto const &partial : stochastic_partials)
    {
        string ind  = partial.first.as<std::string>();
        ind         = ind.replace(0,2,"");
        int p_index = std::stoi(ind, nullptr)-1;

        partial_frequencies[p_index].init(partial, "FRE");

    }


    freq_release.resize(nPartials);

    // loop over all partials and get release parameters
    for(auto const & partial : release_partials)
    {

        string ind  = partial.first.as<std::string>();
        ind         = ind.replace(0,2,"");
        int p_index = std::stoi(ind, nullptr)-1;

        double lamb =  partial.second["lambda"].as<double>();
        double m =  partial.second["mean"].as<double>();
        double s =  partial.second["std"].as<double>();


        freq_release[p_index].set_parameters(l_release, lamb);
        freq_release[p_index].set_statistical_parameters(m,s);

    }

    // Read BARK data
    YAML::Node stochastic_residual      = node["SUS"]["RESIDUAL"];
    nBands = stochastic_residual.size();
    noise_energies.resize(nBands);
    for(auto const &band : stochastic_residual)
    {
        string ind  = band.first.as<std::string>();
        ind         = ind.replace(0,5,"");
        int b_index = std::stoi(ind, nullptr)-1;

        noise_energies[b_index].init(band, "NRG");
    }

}


void StochasticSound::fill_trajectories(std::vector<std::vector<double>> &A, YAML::Node &n, string type)
{

    // get number of trjectories
    int s = n.size();

    A.resize(s);

    for( auto const& traj : n)
    {

        string ind  = traj.first.as<std::string>();

        /// @todo this string comparison is a dirty solution
        if(ind.find("P_")==0)
            ind         = ind.replace(0,2,"");
        if(ind.find("BARK_")==0)
            ind         = ind.replace(0,5,"");

        int IDX = std::stoi(ind, nullptr)-1;

        //    partial_amplitudes_ATT.push_back(std::vector<double>());
        YAML::Node trajectory   = traj.second[type]["trajectory"];

        for(uint i = 0; i < trajectory.size() ; i++)
        {
            double tmpVal = trajectory[i].as<double>();

            A[IDX].push_back(tmpVal);

        }

    }

}


double StochasticSound::get_stochastic_parameter(param_type_e type, int partIDX, double last_value,  double rand)
{

    double out = 0.0;

    switch(type)
    {
    case amp:
        out = partial_amplitudes[partIDX].get_value(last_value, rand);
        break;
    case freq:
        out = partial_frequencies[partIDX].get_value(last_value, rand);
        break;
    case band_nrg:
        out = noise_energies[partIDX].get_value(last_value, rand);
        break;
    }

    return out;
}


double StochasticSound::get_mean_parameter(param_type_e type, int partIDX)
{
    double out = 0.0;

    switch(type)
    {
    case amp:
        out = partial_amplitudes[partIDX].mean();
        break;
    case freq:
        out = partial_frequencies[partIDX].mean();
        break;
    case band_nrg:
        out = noise_energies[partIDX].mean();
        break;
    }

    return out;
}


double StochasticSound::get_amp_att(double pos, int partCNT)
{

    index_interp I = interpolate(pos);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    //tmp_L =  this->partial_amplitudes_ATT[partCNT][I.lowerSP];
    //tmp_U =  this->partial_amplitudes_ATT[partCNT][I.upperSP];

    double out = tmp_L*(1-I.frac) + tmp_U*I.frac;

    return out;

}


double StochasticSound::get_amp_rel(double pos, int partCNT)
{
    // interpolate
    int lowerSP = (int) floor(pos);
    double frac = pos - (double) lowerSP;
    int upperSP = (int) (lowerSP + 1);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    // tmp_L =  this->partial_amplitudes_REL[partCNT][lowerSP];
    // tmp_U =  this->partial_amplitudes_REL[partCNT][upperSP];

    double out = tmp_L*(1-frac) + tmp_U*frac;

    return out;

}


//double StochasticSound::get_lin_amp_att(double pos, int partIDX)
//{

//    double out = this->part_amp_att_lin[partIDX].get_value_at(pos);

//    return out;

//}


//double StochasticSound::get_exp_amp_rel(double pos, int partIDX)
//{

//    double out = this->part_amp_rel_exp[partIDX].get_value_at(pos);

//    return out;

//}

double StochasticSound::get_freq_att(double pos, int partCNT)
{
    // interpolate
    int lowerSP = (int) floor(pos);
    double frac = pos - (double) lowerSP;
    int upperSP = (int) (lowerSP + 1);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    // tmp_L =  this->partial_frequencies_ATT[partCNT][lowerSP];
    // tmp_U =  this->partial_frequencies_ATT[partCNT][upperSP];

    double out = tmp_L*(1-frac) + tmp_U*frac;

    return out;

}


double StochasticSound::get_freq_rel(double pos, int partCNT)
{

    // interpolate
    int lowerSP = (int) floor(pos);
    double frac = pos - (double) lowerSP;
    int upperSP = (int) (lowerSP + 1);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    // tmp_L =  this->partial_frequencies_REL[partCNT][lowerSP];
    // tmp_U =  this->partial_frequencies_REL[partCNT][upperSP];

    double out = tmp_L*(1-frac) + tmp_U*frac;

    return out;

}


double StochasticSound::get_enrg_att(double pos, int partCNT)
{
    // interpolate
    int lowerSP = (int) floor(pos);
    double frac = pos - (double) lowerSP;
    int upperSP = (int) (lowerSP + 1);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    // tmp_L =  this->noise_energies_ATT[partCNT][lowerSP];
    // tmp_U =  this->noise_energies_ATT[partCNT][upperSP];

    double out = tmp_L*(1-frac) + tmp_U*frac;

    return out;

}


double StochasticSound::get_enrg_rel(double pos, int partCNT)
{
    // interpolate
    int lowerSP = (int) floor(pos);
    double frac = pos - (double) lowerSP;
    int upperSP = (int) (lowerSP + 1);

    double tmp_L = 0.0;
    double tmp_U = 0.0;

    // tmp_L =  this->noise_energies_REL[partCNT][lowerSP];
    // tmp_U =  this->noise_energies_REL[partCNT][upperSP];

    double out = tmp_L*(1-frac) + tmp_U*frac;

    return out;

}

int StochasticSound::attack_length()
{
    return l_attack;
}

int StochasticSound::release_length()
{
    return l_release;
}


std::vector<ExponentialDecay>* StochasticSound::get_release_model()
{
    return &freq_release;
}
