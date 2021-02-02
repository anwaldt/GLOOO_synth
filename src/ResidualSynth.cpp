#include "ResidualSynth.h"

ResidualSynth::ResidualSynth(glooo_parameters *GP, std::shared_ptr<BarkCoefficients> BC, std::shared_ptr<StochasticData> SD , std::shared_ptr<DeterministicData> DD)
{

    // shared pointers
    stochastic_data     = SD;
    deterministic_data  = DD;
    bc                  = BC;
    gp                  = GP;

    randBuff = 0.0;

    // allocation
    out_values      = new double[bc->n_bands()];
    filters         = new IirFilter[bc->n_bands()];
    frameEnrgInt    = new FrameInterpolator[bc->n_bands()];

    lastEnergies.resize(bc->n_bands());

    for(int i=0; i<bc->n_bands(); i++)
    {
        filters[i].init(bc,i);

        out_values[i]       = 0.0;
        lastEnergies[i]     = 0.0;
    }

}

void ResidualSynth::prepare_frame_stochastic(double pitch, double intensity, int nBuff)
{

    for(int bandCNT= 0; bandCNT<bc->n_bands(); bandCNT++)
    {

        double nextEnrg = 0;

        if(gp->noise_energy_mode.compare("s")==0)
        {

            // get new support point from statistical data
            nextEnrg     = stochastic_data->get_stochastic_value(pitch,intensity, bandCNT, band_nrg, glooo::random, lastEnergies[bandCNT]);
        }
        else if(gp->noise_energy_mode.compare("m")==0)
        {
            nextEnrg     = stochastic_data->get_mean_value(pitch,intensity, bandCNT, band_nrg);

        }


        //

        // set the frame interpolator for this band
        frameEnrgInt[bandCNT].set_parameters(lastEnergies[bandCNT],nextEnrg,nBuff);

        // store this support point for the next run
        lastEnergies[bandCNT] = nextEnrg;
    }

}


void ResidualSynth::prepare_frame_deterministic(double pitch, double intensity, double position, int nBuff)
{
    for(int bandCNT= 0; bandCNT<bc->n_bands(); bandCNT++)
    {
        double nextEnrg = deterministic_data->get_trajectory_value(bandCNT, pitch, intensity, position, glooo::band_nrg);

        // set the frame interpolator for this band
        frameEnrgInt[bandCNT].set_parameters(lastEnergies[bandCNT],nextEnrg,nBuff);

        // store this support point for the next run
        lastEnergies[bandCNT] = nextEnrg;
    }
}

void ResidualSynth::prepare_frame_attack(double pitch, double intensity, double position, int nBuff, vector<double> target)
{

    double miniramp = 1.0;

    if(position<=50)
        miniramp = (position+0.0001) / 50.0;


    for(int bandCNT= 0; bandCNT<bc->n_bands(); bandCNT++)
    {
        double nextEnrg =  miniramp * //stochastic_data->get_trajectory_value(  pitch , intensity,   bandCNT, position, band_nrg,  attack);
                deterministic_data->get_trajectory_value(bandCNT,pitch, intensity,  position, band_nrg);

        // set the frame interpolator for this band
        frameEnrgInt[bandCNT].set_parameters(lastEnergies[bandCNT], nextEnrg, nBuff);

        // store this support point for the next run
        lastEnergies[bandCNT] = nextEnrg;
    }

}

void ResidualSynth::new_random()
{
    randBuff  =  (((double) rand() / (double)  numeric_limits<int>::max())-0.5)*2.0;
}

double ResidualSynth::get_next_value(int IDX)
{

    double tmpEnrg  = frameEnrgInt[IDX].get_next_value();

    double tmp      = 50.0 * tmpEnrg * filters[IDX].process_value(randBuff);

    return tmp;

}


