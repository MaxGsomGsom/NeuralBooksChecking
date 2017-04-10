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

    void Create(int inputsize, int neuroncount) //Creates the layer and allocates memory
    {
        neurons.resize(neuroncount);
        for (int i = 0; i < neuroncount; i++)
        {
            neurons[i].Create(inputsize);
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

    void PushInput(vector<float>* input) {
        copy(input->begin(), input->end(), layerinput.begin());
    }

    vector<float> GetOutput() {
        vector<float> output;
        for (int i=0; i<neurons.size(); i++){
            output.push_back(neurons[i].GetOutput());
        }
        return output;
    }

    //void Neurons(int i) = 0;
    //int Count() = 0;
};


class InputLayer: public Layer<InputNeuron>
{
public:
    void Train(float errsum, float alpha, float momentum);
};

class HiddenLayer: public Layer<HiddenNeuron>
{
public:
    float Train(float errsum, float alpha, float momentum);
};

class OutputLayer: public Layer<OutputNeuron>
{
public:
    float Train(vector<float>* desiredoutput, float alpha, float momentum);
    float EstimateError(vector<float>* desiredoutput);
};

#endif // LAYER_H
