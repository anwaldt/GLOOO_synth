#include "StochasticTester.h"

StochasticTester* tester;


StochasticTester::StochasticTester()
{

    tester = this;

    // read stuff from the YAML config and parse it to the glooconf object
    /// @todo This has not been adopted for the new file structure!
    gloooconf = new GloooConfig("xxx", "../setup/stochastic_test_settings.yml");

    // Prepare output files /////////////////////////////////////////////////////////////////
    textOutMan = new TextOutputManager(gloooconf, 0);

    this->stochastic_data = std::shared_ptr<StochasticData>(new StochasticData(gloooconf));
    this->stochastic_data->load_stochastic( gloooconf->assignment_file(), gloooconf->get_max_partials());
    //this->deterministic_data = std::shared_ptr<DeterministicData>(new DeterministicData());
    //this->deterministic_data->load_deterministic(gloooconf->SMS_path(), gloooconf->assignment_file(), gloooconf->N_partials());


    ofstream output;
    output.open("random_sequence.txt");


    double out;
    //double last_value = stochastic_data->get_stochastic_value(55, 10, 0, amp, glooo::random,  0.0200);

    for(int CNT=0;CNT<1000;CNT++)
    {
        out =  stochastic_data->get_stochastic_value(55.5, 20, 0, band_nrg, glooo::random, NAN);
        //last_value = out;
        output << out << endl;
    }

    output.close();

}
