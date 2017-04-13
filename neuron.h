#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <cstdlib>
#include <cmath>

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
    float GetOutput() { return output; } //Get neuron output
    int InputCount() { return weights.size(); } //Get input and weights count
    float gain = 1; //Gain value (default = 1)
    float alpha = 0.2; //Train parameter
    float momentum = 0.1; //Train parameter

    //Allocates memory and initializates values
    void Create(int inputcount, float randgain = 1)
    {
        weights.resize(inputcount);
        deltavalues.resize(inputcount);

        //Important initializate all weights as random unsigned values
        //and delta values as 0
        for (int i = 0; i < inputcount; i++)
        {
            //Get a random number between -1 and 1
            weights[i] = rand() / float(RAND_MAX) * 2 - 1;
            deltavalues[i] = 0;
        }

        wgain = rand() / float(RAND_MAX) * 2 - 1;
        wgain *= randgain;
    }

    //Calculates neuron with formula
    void Calculate(vector<float>* input)
    {
        float sum = 0; //Store the sum of all values here
        for (uint j = 0; j < input->size(); j++)
        {
            sum += weights[j] * input->at(j); //apply input * weight
        }
        sum += wgain * gain; //Apply the gain or theta multiplied by the gain weight
        output = 1.f / (1.f + exp(-sum)); //Sigmoidal activation function
        //neurons[i]->output=-1 + 2*(1.f + exp(-sum));
    }
};

/* ===== InputNeuron ===== */

class InputNeuron: public Neuron
{
public:
    void Train(vector<float>* layerinput, float errsum)
    {
        float errorc = output * (1 - output) * errsum;

        for (uint j = 0; j < layerinput->size(); j++)
        {
            float udelta = alpha * errorc * layerinput->at(j) + deltavalues[j] * momentum;
            //Update weights
            weights[j] += udelta;
            deltavalues[j] = udelta;
        }
        //Update the gain weight
        wgain += alpha * errorc * gain;
    }
};

/* ===== HiddenNeuron ===== */

class HiddenNeuron: public Neuron
{
public:
    float Train(vector<float>* layerinput, float errsum)
    {

        float cerrsum = 0; //Neded for next layer

        //Calculate the error for this layer
        float errorc = output * (1 - output) * errsum;

        //Update neuron weights
        for (uint k = 0; k < layerinput->size(); k++)
        {
            float udelta = alpha * errorc * layerinput->at(k) + deltavalues[k] * momentum;
            weights[k] += udelta;
            deltavalues[k] = udelta;
            cerrsum += weights[k] * errorc;
        }

        wgain += alpha * errorc * gain;

        return cerrsum;
    }
};

/* ===== OutputNeuron ===== */

class OutputNeuron: public Neuron
{
public:
    float Train(vector<float>* layerinput, float desiredoutput)
    {
        float errsum = 0;

        //Calculate the error value for the output layer
        float errorc = (desiredoutput - output) * output * (1 - output);

        //Now we proceed to update the weights of the neuron
        for (uint j = 0; j < layerinput->size(); j++)
        {
            //Update the delta value
            float udelta = alpha * errorc * layerinput->at(j) + deltavalues[j] * momentum;
            //Update the weight values
            weights[j] += udelta;
            deltavalues[j] = udelta;

            //We need this to propagate to the next layer
            errsum += weights[j] * errorc;
        }

        //Calculate the weight gain
        wgain += alpha * errorc * gain;

        return errsum;
    }
};

#endif // NEURON_H
