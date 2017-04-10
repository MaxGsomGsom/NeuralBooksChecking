#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <neuron.h>

class Layer
{
public:
    vector<Neuron> neurons;//The array of neurons
    vector<float> layerinput;//The layer input

    Layer();//Object constructor. Initializates all values as 0
    ~Layer();//Destructor. Frees the memory used by the layer

    void Create(int inputsize, int _neuroncount);//Creates the layer and allocates memory
    void Calculate();//Calculates all neurons performing the network formula
    void Layer::Train();
};

#endif // LAYER_H
