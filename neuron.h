#ifndef NEURON_H
#define NEURON_H

#include <vector>

using namespace std;

class Neuron
{
public:
    vector<float> weights; // neuron input weights or synaptic connections
    vector<float> deltavalues; //neuron delta values
    float output; //output value
    float gain;//Gain value
    float wgain;//Weight gain value

    Neuron();//Constructor
    ~Neuron();//Destructor
    void Create(int inputcount);//Allocates memory and initializates values
    void Calculate(vector<float>* input);
};

#endif // NEURON_H
