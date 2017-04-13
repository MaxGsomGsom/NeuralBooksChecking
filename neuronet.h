#ifndef NEURONET_H
#define NEURONET_H

#include <layer.h>
#include <vector>
#include <cstdlib>
#include <exceptions.h>

namespace Neuronets
{

template<class T_in = InputLayer<>, class T_out = OutputLayer<>, class T_hid = HiddenLayer<> >
class Neuronet
{
private:
    T_in m_inputlayer; //Input layer of the network
    T_out m_outputlayer; //Output layer. Contains the result of applying the network
    vector< T_hid > m_hiddenlayers; //Additional hidden layers
    float gain = 1; //Neuron gain value (default = 1)
    float alpha = 0.2; //Train parameter
    float momentum = 0.1; //Train parameter
    float inputsize; //Network input size
    float outputsize; //Network outinput size

public:
    Neuronet() {} //Constructor
    ~Neuronet() {} //Destructor
    float GetGain() { return gain; } //Get neurons gain
    float GetAlpha() { return alpha; } //Get alpha
    float GetMomentum() { return momentum; } //Get momentum
    int HiddenLayersCount() { return m_hiddenlayers.size(); } //Get hidden layers count

    T_in& InputLayer() {return m_inputlayer; } //iterrator
    T_out& OutputLayer() {return m_outputlayer; } //iterrator

    //iterrator
    T_hid& HiddenLayer(int i)
    {
        OUT_OF_RANGE_EX(i < m_hiddenlayers.size() && i >= 0)
        return m_hiddenlayers.at(i);
    }

    //This is useful to get the output values of the network
    vector<float> GetOutput()
    {
        return m_outputlayer.GetOutput();
    }

    //Creates the network structure on memory
    void Create(int inputcount, int inputneurons, int outputcount, vector<int>* hiddenlayers, float randgain = 1)
    {
        WRONG_ARGS_EX(inputcount > 0 && inputneurons > 0 && outputcount > 0 && randgain > 0 && hiddenlayers)

        this->inputsize = inputcount;
        this->outputsize = outputcount;

        m_inputlayer.Create(inputcount, inputneurons, randgain);
        if (hiddenlayers->size() > 0)
        {
            m_hiddenlayers.resize(hiddenlayers->size());
            for (uint i = 0; i < hiddenlayers->size(); i++)
            {
                if (i == 0)
                {
                    //First hidden layer receives the output of the inputlayer so we
                    //set as input the neuroncount of the inputlayer
                    m_hiddenlayers[i].Create(inputneurons, hiddenlayers->at(i), randgain);
                }
                else
                {
                    m_hiddenlayers[i].Create(hiddenlayers->at(i - 1), hiddenlayers->at(i), randgain);
                }
            }
            m_outputlayer.Create(hiddenlayers->at(hiddenlayers->size() - 1), outputcount, randgain);
        }
        else
        {
            m_outputlayer.Create(inputneurons, outputcount, randgain);
        }
    }

    //Calculates the network values given an input pattern
    void Propagate(vector<float>* input)
    {
        VECTOR_SIZE_EX(input && input->size() == inputsize)

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
                if (i + 1 < m_hiddenlayers.size())
                    m_hiddenlayers[i + 1].PushInput(&output);
            }
        }

        m_outputlayer.PushInput(&output);
        //Calculating the final stage - the output layer
        m_outputlayer.Calculate();
    }

    //Updates the weight values of the network given a desired output
    //and applying the backpropagation algorithm
    float TrainPattern(vector<float>* desiredoutput, vector<float>* pattern)
    {
        VECTOR_SIZE_EX(desiredoutput && desiredoutput->size() == outputsize && pattern && pattern->size() == inputsize)

        //First we begin by propagating the input
        Propagate(pattern);

        float errorg = m_outputlayer.EstimateError(desiredoutput);
        float sum = m_outputlayer.Train(desiredoutput);

        for (int i = (m_hiddenlayers.size() - 1); i >= 0; i--)
        {
            sum = m_hiddenlayers[i].Train(sum);
        }

        //Finally process the input layer
        m_inputlayer.Train(sum);

        //Return the general error
        return errorg;
    }

    //Train all given patterns
    float TrainAll(vector< vector<float> >* desiredoutputs, vector< vector<float> >* patterns, int maxiteration = 10000, float stoperror = 0.001)
    {
        WRONG_ARGS_EX(maxiteration > 0 && stoperror >= 0)
        VECTOR_SIZE_EX(desiredoutputs && desiredoutputs->size() == patterns->size())

        float error = 0;
        //Start the neural network training
        for (int i = 0; i < maxiteration; i++)
        {
            for (uint j = 0; j < patterns->size(); j++)
            {
                error += TrainPattern(&(desiredoutputs->at(j)), &(patterns->at(j)));
            }
            error /= patterns->size();

            if (error < stoperror)
                return error;
        }

        return error;
    }

    //Set neurons params
    void SetParams(float gain = 1, float alpha = 0.2, float momentum = 0.1)
    {

        WRONG_ARGS_EX(gain > 0 && alpha > 0 && alpha < 1 && momentum > 0 && momentum < 1)

        this->gain = gain;
        this->alpha = alpha;
        this->momentum = momentum;

        m_inputlayer.SetParams(gain, alpha, momentum);
        m_outputlayer.SetParams(gain, alpha, momentum);

        for (uint i = 0; i < m_hiddenlayers.size(); i++)
        {
            m_hiddenlayers[i].SetParams(gain, alpha, momentum);
        }
    }
};

}

#endif // NEURONET_H
