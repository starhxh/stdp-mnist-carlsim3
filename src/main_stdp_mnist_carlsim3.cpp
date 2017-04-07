#include "mnist.h"
#include "myconnection.h"

#include <carlsim.h>

#include <iostream>
#include <vector>

using std::cout;
using std::vector;
typedef unsigned char uint8_t;


int main() {

    // keep track of execution time
    Stopwatch watch;


    // Set parameters
    bool test_mode = false;
    int num_examples;
    if (test_mode)
        num_examples = 10000;
    else
        num_examples = 60000;

    int single_example_time = 350;
    int resting_time = 150;
    int runtime = num_examples * (single_example_time + resting_time);

    float input_intensity = 2.0f;
    float start_input_intensity = input_intensity;

    int n_in = 784;
    int n_exc = 400;
    int n_inh = n_exc;


    // Read dataset
    vector<vector<uint8_t> > images;
    vector<uint8_t> labels;
    if (test_mode) {
        read_mnist_images("mnist/t10k-images-idx3-ubyte", images, 10000);
        read_mnist_labels("mnist/t10k-labels-idx1-ubyte", labels, 10000);
    } else {
        read_mnist_images("mnist/train-images-idx3-ubyte", images, 60000);
        read_mnist_labels("mnist/train-labels-idx1-ubyte", labels, 60000);
    }


    // ---------------- CONFIG STATE -------------------
    CARLsim sim("stdp_mnist_carlsim3", GPU_MODE, USER, /*ithGPU=*/0, /*randSeed=*/42);

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

    sim.setESTDP(group_exc, true, STANDARD, ExpCurve(0.001f, 20.0f, 0.0015f, 20.0f));

    sim.setConductances(true);


    // ---------------- SETUP STATE -------------------
    watch.lap("setupNetwork");
    sim.setupNetwork();

    SpikeMonitor *spkMon = sim.setSpikeMonitor(group_exc, "DEFAULT");
    ConnectionMonitor *connMon = sim.setConnectionMonitor(group_in, group_exc, "DEFAULT");

    PoissonRate poissRate(n_in, /*onGPU=*/true);
    PoissonRate poissRateSilent(n_in, /*onGPU=*/true);
    poissRateSilent.setRates(0);


    // ---------------- RUN STATE -------------------
    watch.lap("runNetwork");

    int i = 0;
    while (i < 10) {
        if (test_mode == false) {
            // normalization
        }

        // Set rate for next image
        for (int j = 0; j < n_in; j++) {
            poissRate.setRate(j, (float)images[i % num_examples][j] / 8.0f * input_intensity);
        }
        sim.setSpikeRate(group_in, &poissRate);

        spkMon->startRecording();
        sim.runNetwork(0, single_example_time);
        spkMon->stopRecording();

        if (spkMon->getPopNumSpikes() < 5) {
            input_intensity += 1;
            sim.setSpikeRate(group_in, &poissRateSilent);
            sim.runNetwork(0, resting_time);
        } else {
            if (test_mode) {
                //save
            }

            if (i % 100 == 0)
                cout << "runs done: " << i << " of " << num_examples << endl;

            sim.setSpikeRate(group_in, &poissRateSilent);
            sim.runNetwork(0, resting_time);
            input_intensity = start_input_intensity;
            i++;
        }
    }

    watch.stop(); // Print stopwatch summary

    return 0;
}
