#ifndef NEURON_H
#define NEURON_H

#include <vector>

using namespace std;

class Neuron
{
protected:
    float wgain; //Weight gain value
    float output; //Output value
    vector<float> weights; //Neuron input weights or synaptic connections
    vector<float> deltavalues; //Neuron delta values

public:
    Neuron() {} //Constructor
    ~Neuron() {} //Destructor
    void Create(int inputcount, float randgain = 1); //Allocates memory and initializates values
    void Calculate(vector<float>* input); //Calculates neuron with formula
    float GetOutput() { return output; }
    int InputCount() { return weights.size(); }

    float gain = 1; //Gain value (default = 1)
    float alpha = 0.2;
    float momentum = 0.1;
};

class InputNeuron: public Neuron
{
public:
    void Train(vector<float>* layerinput, float errsum);
};

class HiddenNeuron: public Neuron
{
public:
    float Train(vector<float>* layerinput, float errsum);
};

class OutputNeuron: public Neuron
{
public:
    float Train(float desiredoutput, vector<float>* layerinput);
};

#endif // NEURON_H
