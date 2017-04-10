#include "neuronet.h"
#include <cstdlib>

void Neuronet::Create(int inputcount, int inputneurons, int outputcount, vector<int>* hiddenlayers)
{
    m_inputlayer.Create(inputcount, inputneurons);
    if (hiddenlayers->size() > 0)
    {
        m_hiddenlayers.resize(hiddenlayers->size());
        for (uint i = 0; i < hiddenlayers->size(); i++)
        {
            if (i == 0)
            {
                //First hidden layer receives the output of the inputlayer so we
                //set as input the neuroncount of the inputlayer
                m_hiddenlayers[i].Create(inputneurons, hiddenlayers->at(i));
            }
            else
            {
                m_hiddenlayers[i].Create(hiddenlayers->at(i - 1), hiddenlayers->at(i));
            }
        }
        m_outputlayer.Create(hiddenlayers->at(hiddenlayers->size() - 1), outputcount);
    }
    else
    {
        m_outputlayer.Create(inputneurons, outputcount);
    }
}


void Neuronet::Propagate(vector<float>* input)
{
    //The propagation function should start from the input layer
    //first copy the input vector to the input layer
    m_inputlayer.PushInput(input);
    //Now calculate the inputlayer
    m_inputlayer.Calculate();

    //Propagate the inputlayer out values to the next layer
    vector<float> output = m_inputlayer.GetOutput();
    m_hiddenlayers[0].PushInput(&output);

    if (m_hiddenlayers.size() > 0)
    {
        //Calculating hidden layers if any
        for (uint i = 0; i < m_hiddenlayers.size(); i++)
        {
            m_hiddenlayers[i].Calculate();

            output = m_hiddenlayers[i].GetOutput();
            //for all hidden layers, output of last hidden layer puts as input of outputlayer
            if (i+1<m_hiddenlayers.size())
                m_hiddenlayers[i+1].PushInput(&output);
        }
    }

    m_outputlayer.PushInput(&output);
    //Calculating the final stage - the output layer
    m_outputlayer.Calculate();
}

//Main training function. Run this function in a loop as many times needed per pattern
float Neuronet::Train(vector<float>* desiredoutput, vector<float>* input, float alpha, float momentum)
{
    //First we begin by propagating the input
    Propagate(input);

    float errorg = m_outputlayer.EstimateError(desiredoutput);
    float sum = m_outputlayer.Train(desiredoutput, alpha, momentum);

    for (int i = (m_hiddenlayers.size() - 1); i >= 0; i--)
    {
        sum = m_hiddenlayers[i].Train(sum, alpha, momentum);
    }

    //Finally process the input layer
    m_inputlayer.Train(sum, alpha, momentum);

    //Return the general error
    return errorg;
}
