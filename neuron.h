#ifndef NEURON_H
#define NEURON_H

#include <vector>

using namespace std;

class Neuron
{
protected:
    float gain; //Gain value (default = 1)
    float wgain; //Weight gain value
    float output; //Output value
    vector<float> weights; //Neuron input weights or synaptic connections
    vector<float> deltavalues; //Neuron delta values

public:
    Neuron() {} //Constructor
    ~Neuron() {} //Destructor
    void Create(int inputcount); //Allocates memory and initializates values
    void Calculate(vector<float>* input); //Calculates neuron with formula
    float GetOutput() { return output; }
    int InputCount() { return weights.size(); }
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
