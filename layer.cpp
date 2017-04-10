#include "layer.h"
#include <cstdlib>

Layer::Layer()
{
}

Layer::~Layer()
{
}

void Layer::Create(int inputsize, int neuroncount)
{
    neurons.resize(neuroncount);
    for (int i = 0; i < neuroncount; i++)
    {
        neurons[i].Create(inputsize);
    }

    layerinput.resize(inputsize);
}

//Calculates the neural network result of the layer using the sigmoid function
void Layer::Calculate()
{
    //Apply the formula for each neuron
    for (uint i = 0; i < neurons.size(); i++)
    {
        neurons[i].Calculate(&layerinput);
    }
}

void Layer::Train()
{

}


