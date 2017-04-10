#include "neuronet.h"
#include <cstdlib>

Neuronet::Neuronet()
{
}

Neuronet::~Neuronet()
{
}

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
    copy(input->begin(), input->end(), m_inputlayer.layerinput.begin());
    //Now calculate the inputlayer
    m_inputlayer.Calculate();

    Update(-1);//Propagate the inputlayer out values to the next layer
    if (m_hiddenlayers.size() > 0)
    {
        //Calculating hidden layers if any
        for (uint i = 0; i < m_hiddenlayers.size(); i++)
        {
            m_hiddenlayers[i].Calculate();
            Update(i);
        }
    }

    //Calculating the final statge: the output layer
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

void Neuronet::Update(int layerindex)
{
    if (layerindex == -1)
    {
        //Dealing with the inputlayer here and propagating to the next layer
        for (uint i = 0; i < m_inputlayer.neurons.size(); i++)
        {
            if (m_hiddenlayers.size() > 0) //Propagate to the first hidden layer
            {
                m_hiddenlayers[0].layerinput[i] = m_inputlayer.neurons[i].output;
            }
            else //Propagate directly to the output layer
            {
                m_outputlayer.layerinput[i] = m_inputlayer.neurons[i].output;
            }
        }

    }
    else
    {
        for (uint i = 0; i < m_hiddenlayers[layerindex].neurons.size(); i++)
        {
            //Not the last hidden layer
            if (layerindex < (int)m_hiddenlayers.size() - 1)
            {
                m_hiddenlayers[layerindex + 1].layerinput[i] = m_hiddenlayers[layerindex].neurons[i].output;
            }
            else
            {
                m_outputlayer.layerinput[i] = m_hiddenlayers[layerindex].neurons[i].output;
            }
        }
    }
}

