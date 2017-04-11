#ifndef NEURONET_H
#define NEURONET_H

#include <layer.h>
#include <vector>

class Neuronet
{
private:
    InputLayer m_inputlayer; //Input layer of the network
    OutputLayer m_outputlayer; //Output layer. Contains the result of applying the network
    vector<HiddenLayer> m_hiddenlayers; //Additional hidden layers

    float gain = 1;
    float alpha = 0.2;
    float momentum = 0.1;
    float inputsize;
    float outputsize;

public:
    Neuronet() {} //Constructor
    ~Neuronet() {} //Destructor

    //Creates the network structure on memory
    void Create(int inputcount, int inputneurons, int outputcount, vector<int>* hiddenlayers, float randgain = 1);

    //Calculates the network values given an input pattern
    void Propagate(vector<float>* input);

    //Updates the weight values of the network given a desired output
    //and applying the backpropagation algorithm
    float TrainPattern(vector<float>* desiredoutput, vector<float>* pattern);
    float TrainAll(vector< vector<float> >* desiredoutputs, vector< vector<float> >* patterns, int maxiteration = 10000, float stoperror = 0.001);

    //Returns the output layer. This is useful to get the output values of the network
    vector<float> GetOutput()
    {
        return m_outputlayer.GetOutput();
    }

    void SetParams(float gain = 1, float alpha = 0.2, float momentum = 0.1);

    float GetGain() { return gain; }
    float GetAlpha() { return alpha; }
    float GetMomentum() { return momentum; }

};

#endif // NEURONET_H
