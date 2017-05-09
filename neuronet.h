#ifndef NEURONET_H
#define NEURONET_H

#include <neurolayer.h>

namespace Neuronets
{

template<class T_in = InputLayer<>, class T_out = OutputLayer<>, class T_hid = HiddenLayer<> >
class Neuronet
{
private:
    T_in m_inputlayer; //Input layer of the network
    T_out m_outputlayer; //Output layer. Contains the result of applying the network
    QVector< T_hid > m_hiddenlayers; //Additional hidden layers
    float gain = 1; //Neuron gain value (default = 1)
    float alpha = 0.2; //Train parameter
    float momentum = 0.1; //Train parameter
    float inputsize; //Network input size
    float outputsize; //Network outinput size
    float lastError = 0;
    bool isTrained = false;

    //Updates the weight values of the network given a desired output
    //and applying the backpropagation algorithm
    float TrainPattern(const QVector<float> &desiredoutput, const QVector<float> &pattern)
    {
        if(!(desiredoutput.size() == outputsize && pattern.size() == inputsize))
            throw WrongInputVectorSize(__FUNCTION__);

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

public:
    Neuronet() {} //Constructor
    ~Neuronet() {} //Destructor
    float GetGain() { return gain; } //Get neurons gain
    float GetAlpha() { return alpha; } //Get alpha
    float GetMomentum() { return momentum; } //Get momentum
    int HiddenLayersCount() { return m_hiddenlayers.size(); } //Get hidden layers count
    float LastError() { return lastError; }
    bool IsTrained() { return isTrained;}

    T_in& InputLayer() {return m_inputlayer; } //iterrator
    T_out& OutputLayer() {return m_outputlayer; } //iterrator

    T_hid& HiddenLayer(int i)
    {
        if(!(i < m_hiddenlayers.size() && i >= 0))
            throw OutOfRange(__FUNCTION__);

        return m_hiddenlayers.at(i);
    }

    //This is useful to get the output values of the network
    QVector<float> GetOutput()
    {
        return m_outputlayer.GetOutput();
    }

    //Creates the network structure on memory
    void Create(int inputcount, int inputneurons, int outputcount, const QVector<int> &hiddenlayers, float randgain = 1)
    {
        if(!(inputcount > 0 && inputneurons > 0 && outputcount > 0 && randgain > 0))
            throw WrongInputArgs(__FUNCTION__);

        lastError = 0;
        isTrained = false;

        this->inputsize = inputcount;
        this->outputsize = outputcount;

        m_inputlayer.Create(inputcount, inputneurons, randgain);
        if (hiddenlayers.size() > 0)
        {
            m_hiddenlayers.resize(hiddenlayers.size());
            for (int i = 0; i < hiddenlayers.size(); i++)
            {
                if (i == 0)
                {
                    //First hidden layer receives the output of the inputlayer so we
                    //set as input the neuroncount of the inputlayer
                    m_hiddenlayers[i].Create(inputneurons, hiddenlayers.at(i), randgain);
                }
                else
                {
                    m_hiddenlayers[i].Create(hiddenlayers.at(i - 1), hiddenlayers.at(i), randgain);
                }
            }
            m_outputlayer.Create(hiddenlayers.at(hiddenlayers.size() - 1), outputcount, randgain);
        }
        else
        {
            m_outputlayer.Create(inputneurons, outputcount, randgain);
        }
    }

    //Calculates the network values given an input pattern
    void Propagate(const QVector<float> &input)
    {
        if(!(input.size() == inputsize))
            throw WrongInputVectorSize(__FUNCTION__);

        //The propagation function should start from the input layer
        //first copy the input vector to the input layer
        m_inputlayer.PushInput(input);
        //Now calculate the inputlayer
        m_inputlayer.Calculate();

        //Propagate the inputlayer out values to the next layer
        QVector<float> output = m_inputlayer.GetOutput();
        m_hiddenlayers[0].PushInput(output);

        if (m_hiddenlayers.size() > 0)
        {
            //Calculating hidden layers if any
            for (int i = 0; i < m_hiddenlayers.size(); i++)
            {
                m_hiddenlayers[i].Calculate();

                output = m_hiddenlayers[i].GetOutput();
                //for all hidden layers, output of last hidden layer puts as input of outputlayer
                if (i + 1 < m_hiddenlayers.size())
                    m_hiddenlayers[i + 1].PushInput(output);
            }
        }

        m_outputlayer.PushInput(output);
        //Calculating the final stage - the output layer
        m_outputlayer.Calculate();
    }

    //Train all given patterns
    float TrainAll(const QVector< QVector<float> > &desiredoutputs, const QVector< QVector<float> > &patterns, int maxiteration = 10000, float stoperror = 0.001)
    {
        if(!(maxiteration > 0 && stoperror >= 0))
            throw WrongInputArgs(__FUNCTION__);

        if(!(desiredoutputs.size() == patterns.size()))
            throw WrongInputVectorSize(__FUNCTION__);

        float error = 0;
        //Start the neural network training
        for (int i = 0; i < maxiteration; i++)
        {
            for (int j = 0; j < patterns.size(); j++)
            {
                error += TrainPattern(desiredoutputs.at(j), patterns.at(j));
            }
            error /= patterns.size();

            lastError = error;

            if (error < stoperror)
            {
                isTrained = true;
                return error;
            }
        }

        return error;
    }

    //Set neurons params
    void SetParams(float gain = 1, float alpha = 0.2, float momentum = 0.1)
    {
        if(!(gain > 0 && alpha > 0 && alpha < 1 && momentum > 0 && momentum < 1))
            throw WrongInputArgs(__FUNCTION__);

        this->gain = gain;
        this->alpha = alpha;
        this->momentum = momentum;

        m_inputlayer.SetParams(gain, alpha, momentum);
        m_outputlayer.SetParams(gain, alpha, momentum);

        for (int i = 0; i < m_hiddenlayers.size(); i++)
        {
            m_hiddenlayers[i].SetParams(gain, alpha, momentum);
        }
    }

    friend QDataStream &operator<<(QDataStream& out, const Neuronet &obj)
    {
        out << obj.gain << obj.alpha << obj.momentum << obj.inputsize << obj.outputsize <<
            obj.lastError << obj.isTrained << obj.m_inputlayer << obj.m_outputlayer << obj.m_hiddenlayers;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Neuronet &obj) {
        in >> obj.gain >> obj.alpha >> obj.momentum >> obj.inputsize >> obj.outputsize >>
            obj.lastError >> obj.isTrained >> obj.m_inputlayer >> obj.m_outputlayer >> obj.m_hiddenlayers;
        return in;
    }
};

}

#endif // NEURONET_H
