#ifndef NEURONET_H
#define NEURONET_H

#include <neurolayer.h>
#include <iostream>
#include "graph_vis.h"

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
    float TrainPattern(const QVector<float>& desiredoutput, const QVector<float>& pattern)
    {
        //pre
        if (!(desiredoutput.size() == outputsize && pattern.size() == inputsize))
            throw WrongInputVectorSize();
        //===

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
        //pre
        if (!(i < m_hiddenlayers.size() && i >= 0))
            throw OutOfRange();
        //===

        return m_hiddenlayers.at(i);
    }

    bool InvariantTest()
    {
        Q_ASSERT(gain > 0);
        Q_ASSERT(alpha > 0 && alpha <= 1);
        Q_ASSERT(momentum > 0 && momentum <= 1);
        Q_ASSERT(inputsize > 0);
        Q_ASSERT(outputsize > 0);
        Q_ASSERT(lastError >= 0);

        //test input layer and neurons
        Q_ASSERT(m_inputlayer.InputCount() == inputsize);
        m_inputlayer.InvariantTest();
        for (int k = 0; k < m_inputlayer.NeuronsCount(); ++k)
        {
            Q_ASSERT(m_inputlayer.Neuron(k).InputCount() == inputsize);
        }

        //test output layer and neurons
        Q_ASSERT(m_outputlayer.NeuronsCount() == outputsize);
        m_outputlayer.InvariantTest();
        for (int k = 0; k < m_outputlayer.NeuronsCount(); ++k)
        {
            Q_ASSERT(m_outputlayer.Neuron(k).InputCount() == m_hiddenlayers.last().NeuronsCount());
        }

        //test hidden layers and neurons
        for (int i = 1; i < m_hiddenlayers.size(); ++i)
        {
            Q_ASSERT(m_hiddenlayers[i - 1].NeuronsCount() == m_hiddenlayers[i].InputCount());

            for (int k = 0; k < m_outputlayer.NeuronsCount(); ++k)
            {
                Q_ASSERT(m_hiddenlayers[i - 1].NeuronsCount() == m_hiddenlayers[i].Neuron(k).InputCount());
            }
        }

        for (int i = 0; i < m_hiddenlayers.size(); ++i)
        {
            m_hiddenlayers[i].InvariantTest();
        }

        return true;
    }

    //This is useful to get the output values of the network
    QVector<float> GetOutput()
    {
        return m_outputlayer.GetOutput();
    }

    //Creates the network structure on memory
    void Create(int inputcount, int inputneurons, int outputcount, const QVector<int>& hiddenlayers, float randgain = 1)
    {
        //pre
        if (!(inputcount > 0 && inputneurons > 0 && outputcount > 0 && randgain > 0))
            throw WrongInputArgs();

        foreach (int hid, hiddenlayers)
        {
            if (hid < 0)
                throw WrongInputArgs();
        }
        //===

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
        std::cerr<<"Create()";
        NeuronNetVis<> nnv;
        nnv.load(m_inputlayer,m_hiddenlayers[0],m_outputlayer);
        nnv.generate();
    }

    //Calculates the network values given an input pattern
    void Propagate(const QVector<float>& input)
    {
        //pre
        if (!(input.size() == inputsize))
            throw WrongInputVectorSize();
        //===

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
    float TrainAll(const QVector< QVector<float> >& desiredoutputs, const QVector< QVector<float> >& patterns, int maxiteration = 10000, float stoperror = 0.001)
    {
        //pre
        if (!(maxiteration > 0 && stoperror >= 0))
            throw WrongInputArgs();

        if (!(desiredoutputs.size() == patterns.size()))
            throw WrongInputVectorSize();

        foreach (QVector<float> vec, desiredoutputs)
        {
            if (vec.size() != outputsize)
                throw WrongInputVectorSize();
        }

        foreach (QVector<float> vec, patterns)
        {
            if (vec.size() != inputsize)
                throw WrongInputVectorSize();
        }
        //===

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
        //pre
        if (!(gain > 0 && alpha > 0 && alpha <= 1 && momentum > 0 && momentum <= 1))
            throw WrongInputArgs();
        //===

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

    friend QDataStream& operator<<(QDataStream& out, const Neuronet& obj)
    {
        out << obj.gain << obj.alpha << obj.momentum << obj.inputsize << obj.outputsize <<
            obj.lastError << obj.isTrained << obj.m_inputlayer << obj.m_outputlayer << obj.m_hiddenlayers;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, Neuronet& obj)
    {
        in >> obj.gain >> obj.alpha >> obj.momentum >> obj.inputsize >> obj.outputsize >>
           obj.lastError >> obj.isTrained >> obj.m_inputlayer >> obj.m_outputlayer >> obj.m_hiddenlayers;
        return in;
    }
};

class NeuronetTest{
public:
    static bool Test01(){
    Neuronet<> net;
    QVector< QVector<float> > outp;
    QVector<float> v = {0.0, 0.1, 0.2, 0.3, 0.4};
    QVector<float> v1 = {0.0, 0.09, 0.12, 0.29, 0.39};
    outp.push_back(v);
    QVector< QVector<float> > patt;
    patt.push_back(v1);
    net.TrainAll(outp, patt, 1000, 0.001);
    return net.IsTrained();
    }
    static bool Test02(){
    Neuronet<> net;
    QVector< QVector<float> > outp;
    QVector<float> v = {0.0, 0.1, 0.2, 0.3, 0.4};
    QVector<float> v1 = {0.0, 0.12, 0.22, 0.32, 0.42};
    outp.push_back(v);
    QVector< QVector<float> > patt;
    patt.push_back(v1);
    net.TrainAll(outp, patt, 1000, 0.001);
    return net.IsTrained();
    }
    static bool Test03(){
    Neuronet<> net;
    QVector< QVector<float> > outp;
    QVector<float> v = {0.0, 0.1, 0.2, 0.3, 0.4};
    QVector<float> v1 = {0.5, 0.15, 0.25, 0.35, 0.45};
    outp.push_back(v);
    QVector< QVector<float> > patt;
    patt.push_back(v1);
    net.TrainAll(outp, patt, 1000, 0.001);
    return !net.IsTrained();
    }

};

}

#endif // NEURONET_H
