#include "StatisticalParameter.h"

StatisticalParameter::StatisticalParameter()
{
    /// @todo The FIR is now just a sliding window with hard-coded parameters
    out_buff.resize(20);

    std::fill(out_buff.begin(),out_buff.end(),0.0);

}

void StatisticalParameter::init(const YAML::detail::iterator_value n, std::string type)
{

    param_type = type;
    YAML::Node sub_node = n.second[type]["ICMF"];

    // read two global vectors
    YAML::Node tmpNode = sub_node["valid"];
    for(uint i= 0; i < tmpNode.size(); i++ )
    {
        is_valid.push_back((bool) tmpNode[i].as<double>());
    }

    tmpNode = sub_node["dist_centers"];
    for(uint i= 0; i < tmpNode.size(); i++ )
    {
        dist_centers.push_back(tmpNode[i].as<double>());
    }

    N_icmf = dist_centers.size();

    icfm_collection.resize(N_icmf);
    support_points.resize(N_icmf);


    // loop over all distributions
    for(uint i= 0; i < N_icmf; i++ )
    {

        string tmp = "icmf_" + std::to_string((i+1));

        YAML::Node subsub_node  = sub_node[tmp];
        YAML::Node icmf_node    = subsub_node["icmf"];
        YAML::Node support_node = subsub_node["support"];

        // loop over all values
        for(uint j= 0; j < icmf_node.size(); j++ )
        {
            icfm_collection[i].push_back(icmf_node[j].as<double>());
            support_points[i].push_back(support_node[j].as<double>());
        }

    }

    mean_value   = n.second[type]["mean"].as<double>();
    median_value = n.second[type]["med"].as<double>();
    stddev_value = n.second[type]["std"].as<double>();
}


double StatisticalParameter::get_value(double lastVal, double randVal)
{

    /// @todo do not let the residual approach zero energies
    /// for very low notes: as long as there is excitation,
    /// there is noise!

    // get first ICMF idx BELOW lastVal
    vector<double>::iterator it = lower_bound(dist_centers.begin(), dist_centers.end(),lastVal);
    int icmfIDX                 = distance(dist_centers.begin(),it);

    // find true index by distance comparison
    if(icmfIDX>0)
    {
        double lower_dist = std::abs(dist_centers[icmfIDX-1]  - lastVal);
        double upper_dist = std::abs(dist_centers[icmfIDX]    - lastVal);

        if(lower_dist<upper_dist)
            icmfIDX--;
    }

    icmfIDX = max(icmfIDX,0);

    //    /// @todo do we still need this?
    //    if(icmfIDX>=dist_centers.size())
    //    {
    //        it =   dist_centers.end()-1;
    //        icmfIDX = distance(dist_centers.begin(),it);
    //    }


    double outVal = 0.0;

    if(icfm_collection.size()>0)
    {
        // all ICMFs have individual sizes - get this ICMFs size:
        int n_support   = icfm_collection[icmfIDX].size();

        // get the index related to the random value
        int tmpIDX      = round((randVal * (double) (n_support - 1)));
        tmpIDX          = min(max(tmpIDX,0), N_icmf-1);

        // debug output:
        //if(gc->print_stochastic()==true)
        //{
        //   cout << "Using ICMF: " << icmfIDX << " at positition: " << tmpIDX << endl;
        //   cout << "Picked index " << tmpIDX  << " of " << n_support << endl;
        //   cout << (double) tmpIDX/n_support << endl;
        //}

        outVal = icfm_collection[icmfIDX][tmpIDX];

    }


    return outVal;

}

double StatisticalParameter::mean()
{
    return mean_value;
}

double StatisticalParameter::median()
{
    return median_value;
}

double StatisticalParameter::std()
{
    return stddev_value;
}
