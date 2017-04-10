#ifndef NEURONET_H
#define NEURONET_H

#include <layer.h>
#include <vector>

class Neuronet
{
private:
    Layer m_inputlayer;//input layer of the network
    Layer m_outputlayer;//output layer..contains the result of applying the network
    vector<Layer> m_hiddenlayers;//Additional hidden layers

public:
    //function tu create in memory the network structure
    Neuronet();//Construction..initialzates all values to 0
    ~Neuronet();//Destructor..releases memory
    //Creates the network structure on memory
    void Create(int inputcount,int inputneurons,int outputcount,vector<int> *hiddenlayers);

    void Propagate(vector<float> *input);//Calculates the network values given an input pattern
    //Updates the weight values of the network given a desired output and applying the backpropagation
    //Algorithm
    float Train(vector<float> *desiredoutput,vector<float> *input,float alpha, float momentum);

    //Updates the next layer input values
    void Update(int layerindex);

    //Returns the output layer..this is useful to get the output values of the network
    inline Layer &GetOutput()
    {
        return m_outputlayer;
    }

};

#endif // NEURONET_H
