#include "myconnection.h"

#include <carlsim.h>


int main() {

    // keep track of execution time
    Stopwatch watch;


    // ---------------- CONFIG STATE -------------------
    int ithGPU = 0;
    int randSeed = 42;
    CARLsim sim("stdp_mnist_carlsim3", CPU_MODE, USER, ithGPU, randSeed);

    // Configure the network.
    bool test_mode = false;
    int num_examples;
    if (test_mode)
        num_examples = 10000;
    else
        num_examples = 60000;

    int single_example_time = 350;
    int resting_time = 150;
    int runtime = num_examples * (single_example_time + resting_time);

    float input_intensity = 2.;
    float start_input_intensity = input_intensity;

    int n_in = 784;
    int n_exc = 400;
    int n_inh = n_exc;

    int group_in = sim.createSpikeGeneratorGroup("input", n_in, EXCITATORY_NEURON);
    int group_exc = sim.createGroup("excitatory", n_exc, EXCITATORY_NEURON);
    int group_inh = sim.createGroup("inhibitory", n_inh, INHIBITORY_NEURON);
    sim.setNeuronParameters(group_exc, 0.02f, 0.2f, -65.0f, 8.0f);
    sim.setNeuronParameters(group_inh, 0.02f, 0.2f, -65.0f, 8.0f);

    MyFullConnection myFullConn;
    sim.connect(group_in, group_exc, &myFullConn, SYN_PLASTIC);
    MyOneToOneConnection myOneToOneConn;
    sim.connect(group_exc, group_inh, &myOneToOneConn);
    sim.connect(group_inh, group_exc, "full-no-direct", RangeWeight(17.0), 1.0f);

    sim.setConductances(true);


    // ---------------- SETUP STATE -------------------
    // build the network
    watch.lap("setupNetwork");
    sim.setupNetwork();

    sim.setSpikeMonitor(group_in, "DEFAULT");
    sim.setSpikeMonitor(group_exc, "DEFAULT");
    sim.setConnectionMonitor(group_in, group_exc, "DEFAULT");

    PoissonRate in(n_in);
    in.setRates(30.0f);
    sim.setSpikeRate(group_in, &in);


    // ---------------- RUN STATE -------------------
    watch.lap("runNetwork");

    // Run for a total of 10 seconds.
    // At the end of each `runNetwork` call, SpikeMonitor stats will be printed (thus, every second).
    for (int i = 0; i < 10; i++) {
        sim.runNetwork(1, 0);
    }

    // Print stopwatch summary
    watch.stop();

    return 0;
}
