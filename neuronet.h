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

public:
    Neuronet();//Constructor
    ~Neuronet();//Destructor

    //Creates the network structure on memory
    void Create(int inputcount, int inputneurons, int outputcount, vector<int>* hiddenlayers);

    //Calculates the network values given an input pattern
    void Propagate(vector<float>* input);

    //Updates the weight values of the network given a desired output
    //and applying the backpropagation algorithm
    float Train(vector<float>* desiredoutput, vector<float>* input, float alpha, float momentum);

    //Updates the next layer input values
    void Update(int layerindex);

    //Returns the output layer. This is useful to get the output values of the network
    OutputLayer& GetOutput()
    {
        return m_outputlayer;
    }

};

#endif // NEURONET_H
