#include "TextOutputManager.h"

TextOutputManager::TextOutputManager(GloooConfig *GC, int id)
{

    ID          = id;

    gc          = GC;

    freqValues  = new vector<double>[gc->get_max_partials()];
    ampValues   = new vector<double>[gc->get_max_partials()];


    partial_amplitudes_file     = gc->partOutPath().append("part_amp_").append(to_string(id).append(".out"));
    partial_frequencies_file    = gc->partOutPath().append("part_freq_").append(to_string(id).append(".out"));
    control_inputs_file         = gc->partOutPath().append("control_").append(to_string(id).append(".out"));


    ctlStr.open(control_inputs_file);

}



void  TextOutputManager::write_amplitude(double amp,bool breakLine)
{

    ampStr << fixed <<  setw( 11 ) <<  setprecision( 6 ) << amp;

    if(breakLine==false)
        ampStr << "\t";
    else
        ampStr << "\n";
}


void  TextOutputManager::push_amplitude(double amp,int parIDX)
{

    ampValues[parIDX].push_back(amp);

}

void  TextOutputManager::push_frequency(double freq,int parIDX)
{

    freqValues[parIDX].push_back(freq);

}

void TextOutputManager::write_control(double val, bool breakLine)
{
    ctlStr << fixed <<  setw( 11 ) <<  setprecision( 8 ) << val;

    if(breakLine==false)
        ctlStr << "\t";
    else
        ctlStr << "\n";
}


void  TextOutputManager::export_to_file(int num_part)
{


    // only open file if output is desired:
    if(gc->print_partials()==true)
    {
        ampStr.open(partial_amplitudes_file);
        freqStr.open(partial_frequencies_file);
    }


    for (unsigned i=0; i < ampValues[0].size(); i++)
    {

        for(int j=0; j< num_part; j++)
        {

            ampStr << ampValues[j][i];
            ampStr << "\t";

            freqStr << freqValues[j][i];
            freqStr << "\t";

        }

        ampStr << endl;
        freqStr << endl;

    }


    ampStr.close();
    freqStr.close();
    ctlStr.close();

    for(int j=0; j< num_part; j++)
    {
        ampValues[j].clear();
        freqValues[j].clear();
    }

}




