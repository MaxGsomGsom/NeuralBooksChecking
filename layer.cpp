#include "layer.h"
#include <cstdlib>
#include <cmath>

float OutputLayer::EstimateError(vector<float>* desiredoutput)
{
    float errorg = 0;
    for (uint i = 0; i < neurons.size(); i++)
    {
        //The general error is the sum of delta values. Where delta is the squared difference
        //of the desired value with the output value
        errorg += pow((desiredoutput->at(i) - neurons[i].GetOutput()), 2.f);
    }

    return errorg / 2;
}

float OutputLayer::Train(vector<float>* desiredoutput)
{
    float errsum = 0;
    for (uint i = 0; i < neurons.size(); i++)
    {
        errsum += neurons[i].Train(desiredoutput->at(i), &layerinput);
    }

    return errsum;
}

float HiddenLayer::Train(float errsum)
{

    float csum = 0;
    for (uint j = 0; j < neurons.size(); j++)
    {
        csum += neurons[j].Train(&layerinput, errsum);
    }
    return csum;
}

void InputLayer::Train(float errsum)
{
    for (uint i = 0; i < neurons.size(); i++)
    {
        neurons[i].Train(&layerinput, errsum);
    }
}

