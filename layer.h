#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <neuron.h>
#include <cstdlib>

template<class T = Neuron>
class Layer
{
protected:
    vector<float> layerinput; //The layer input
    vector<T> neurons; //The array of neurons

public:
    Layer() {} //Constructor
    ~Layer() {} //Destructor

    void Create(int inputsize, int neuroncount, float randgain = 1) //Creates the layer and allocates memory
    {
        neurons.resize(neuroncount);
        for (int i = 0; i < neuroncount; i++)
        {
            neurons[i].Create(inputsize, randgain);
        }

        layerinput.resize(inputsize);
    }

    void Calculate() //Calculates all neurons performing the network formula
    {
        //Apply the formula for each neuron
        for (uint i = 0; i < neurons.size(); i++)
        {
            neurons[i].Calculate(&layerinput);
        }
    }

    int InputCount() { return layerinput.size(); }
    int NeuronsCount() { return neurons.size(); }
    T& Neuron(int i) {return neurons.at(i); }

    void PushInput(vector<float>* input)
    {
        copy(input->begin(), input->end(), layerinput.begin());
    }

    vector<float> GetOutput()
    {
        vector<float> output;
        for (int i = 0; i < neurons.size(); i++)
        {
            output.push_back(neurons[i].GetOutput());
        }
        return output;
    }

    void SetParams(float gain = 1, float alpha = 0.2, float momentum = 0.1)
    {
        for (int i = 0; i < neurons.size(); i++)
        {
            neurons[i].gain = gain;
            neurons[i].alpha = alpha;
            neurons[i].momentum = momentum;
        }
    }
};


class InputLayer: public Layer<InputNeuron>
{
public:
    void Train(float errsum);
};

class HiddenLayer: public Layer<HiddenNeuron>
{
public:
    float Train(float errsum);
};

class OutputLayer: public Layer<OutputNeuron>
{
public:
    float Train(vector<float>* desiredoutput);
    float EstimateError(vector<float>* desiredoutput);
};

#endif // LAYER_H
