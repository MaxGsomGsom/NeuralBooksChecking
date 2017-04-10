#include "neuron.h"
#include "cstdlib"
#include <cmath>

Neuron::Neuron(): output(0), gain(0), wgain(0) //constructor
{
}

//Destructor
Neuron::~Neuron()
{
}

//Initializates neuron weights
void Neuron::Create(int inputcount)
{
    weights.resize(inputcount);
    deltavalues.resize(inputcount);
    //important initializate all weights as random unsigned values
    //and delta values as 0
    for (int i = 0; i < inputcount; i++)
    {
        //get a random number between -0.5 and 0.5
        weights[i] = rand() / float(RAND_MAX) * 2 - 1;
        deltavalues[i] = 0;
    }

    gain = 1;
    wgain = rand() / float(RAND_MAX) * 2 - 1;

}

void Neuron::Calculate(vector<float>* input)
{
    float sum = 0; //store the sum of all values here
    for (uint j = 0; j < input->size(); j++)
    {
        //Performing function
        sum += weights[j] * input->at(j); //apply input * weight
    }
    sum += wgain * gain; //apply the gain or theta multiplied by the gain weight.
    //sigmoidal activation function
    output = 1.f / (1.f + exp(-sum)); //calculate the sigmoid function
    //neurons[i]->output=-1 + 2*(1.f + exp(-sum));
}


