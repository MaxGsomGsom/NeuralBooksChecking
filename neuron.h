#ifndef NEURON_H
#define NEURON_H

#include <vector>

using namespace std;

class Neuron
{
public:
    vector<float> weights; //Neuron input weights or synaptic connections
    vector<float> deltavalues; //Neuron delta values
    float output; //Output value
    float gain; //Gain value (default = 1)
    float wgain; //Weight gain value

    Neuron(); //Constructor
    ~Neuron(); //Destructor
    void Create(int inputcount); //Allocates memory and initializates values
    void Calculate(vector<float>* input); //Calculates neuron with formula
};

class InputNeuron: public Neuron {
public:
    void Train(vector<float> *layerinput, float errsum, float alpha, float momentum);
};

class HiddenNeuron: public Neuron {
public:
    float Train(vector<float> *layerinput, float errsum, float alpha, float momentum);
};

class OutputNeuron: public Neuron {
public:
    float Train(float desiredoutput, vector<float>* layerinput, float alpha, float momentum);
};

#endif // NEURON_H
