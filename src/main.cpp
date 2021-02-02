#include <iostream>
#include <string>
#include "GloooSynth.h"

using namespace std;

int main(int argc, char *argv[])
{

   const char* model_path  = "-1";
   const char* config_file = "-1";


    for (int i = 1; i < argc; i = i + 2)
    {

        if (strcmp((const char *)argv[i], (const char *)"-m") == 0)
        {
            cout << "Using model path: " << argv[i + 1] << endl  << endl;
            model_path = argv[i + 1];
        }

        if (strcmp((const char *)argv[i], (const char *)"-c") == 0)
        {
            cout << "Using config file: " << argv[i + 1] << endl  << endl;
            config_file = argv[i + 1];
        }

    }

    if(strcmp(model_path,"-1")==0 || strcmp(config_file,"-1")==0)
    {
        cout << "Need model path and YAML config file!" << endl;
        cout << "Example: GLOOO_synth -m ~/MODEL -c glooo_config.yml" << endl;
    }
    else
    {
        GloooSynth synth(model_path, config_file);
    }
}
