#ifndef LAYER_H
#define LAYER_H

#include <neuron.h>

namespace Neuronets
{

template<class T = Neuron>
class Layer
{
private:
    float inputsize;

protected:
    QVector<float> layerinput; //The layer input
    QVector<T> neurons; //The array of neurons

public:
    Layer() {} //Constructor
    ~Layer() {} //Destructor
    qint32 InputCount() { return inputsize; } //get layerinput size
    qint32 NeuronsCount() { return neurons.size(); } //get neurons size
    T& Neuron(int i) //iterrator
    {
        OUT_OF_RANGE_EX(i >= 0 && i < neurons.size())
        return neurons.at(i);
    }

    //Creates the layer and allocates memory
    void Create(int inputsize, int neuroncount, float randgain = 1)
    {
        WRONG_ARGS_EX(inputsize > 0 && neuroncount > 0 && randgain > 0)

        neurons.resize(neuroncount);
        for (int i = 0; i < neuroncount; i++)
        {
            neurons[i].Create(inputsize, randgain);
        }

        this->inputsize = inputsize;
        layerinput.resize(inputsize);
    }

    //Calculates all neurons performing the network formula
    void Calculate()
    {
        //Apply the formula for each neuron
        for (int i = 0; i < neurons.size(); i++)
        {
            neurons[i].Calculate(layerinput);
        }
    }

    //Fill layerinput with new elements
    void PushInput(const QVector<float> &input)
    {
        VECTOR_SIZE_EX(input.size() == layerinput.size())

        copy(input.begin(), input.end(), layerinput.begin());
    }

    QVector<float> GetOutput()
    {
        QVector<float> output;
        for (int i = 0; i < neurons.size(); i++)
        {
            output.push_back(neurons[i].GetOutput());
        }
        return output;
    }

    //Set new neurons parameters
    void SetParams(float gain = 1, float alpha = 0.2, float momentum = 0.1)
    {
        WRONG_ARGS_EX(gain > 0 && alpha > 0 && alpha < 1 && momentum > 0 && momentum < 1)

        for (int i = 0; i < neurons.size(); i++)
        {
            neurons[i].gain = gain;
            neurons[i].alpha = alpha;
            neurons[i].momentum = momentum;
        }
    }

    friend QDataStream &operator<<(QDataStream &out, const Layer &obj)
    {
        out << obj.neurons << obj.inputsize;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Layer &obj) {
        in >> obj.neurons >> obj.inputsize;
        obj.layerinput.resize(obj.inputsize);
        return in;
    }
};


/* ===== InputNeuron ===== */

template<class T = InputNeuron>
class InputLayer: public Layer<T>
{
public:
    void Train(float errsum)
    {
        for (int i = 0; i < this->neurons.size(); i++)
        {
            this->neurons[i].Train(this->layerinput, errsum);
        }
    }
};


/* ===== HiddenLayer ===== */

template<class T = HiddenNeuron>
class HiddenLayer: public Layer<T>
{
public:
    float Train(float errsum)
    {
        float csum = 0;
        for (int j = 0; j < this->neurons.size(); j++)
        {
            //The general error is the sum of delta values. Where delta is the squared difference
            //of the desired value with the output value
            csum += this->neurons[j].Train(this->layerinput, errsum);
        }
        return csum;
    }
};


/* ===== OutputLayer ===== */

template<class T = OutputNeuron>
class OutputLayer: public Layer<T>
{
public:
    float Train(const QVector<float> &desiredoutput)
    {
        VECTOR_SIZE_EX(desiredoutput.size() == this->neurons.size())

        float errsum = 0;
        for (int i = 0; i < this->neurons.size(); i++)
        {
            errsum += this->neurons[i].Train(this->layerinput, desiredoutput.at(i));
        }

        return errsum;
    }

    float EstimateError(const QVector<float> &desiredoutput)
    {
        VECTOR_SIZE_EX(desiredoutput.size() == this->neurons.size())

        float errorg = 0;
        for (int i = 0; i < this->neurons.size(); i++)
        {
            errorg += pow((desiredoutput.at(i) - this->neurons[i].GetOutput()), 2.f);
        }

        return errorg / 2;
    }
};

}

#endif // LAYER_H
