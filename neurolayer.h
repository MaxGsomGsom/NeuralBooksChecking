#ifndef LAYER_H
#define LAYER_H

#include <neuron.h>

namespace Neuronets
{

template<class T = Neuron> class Layer;
template<class T = Neuron> class LayerIterator;
template <class T = Layer<>> struct LayerAllocator;


template<class T>
class Layer
{
private:
    float inputsize;
    friend class LayerIterator<T>;
    friend class LayerAllocator<Layer>;

protected:
    QVector<float> layerinput; //The layer input
    QVector<T> neurons; //The array of neurons

public:
    Layer() {} //Constructor
    ~Layer() {} //Destructor
    qint32 InputCount() { return inputsize; } //get layerinput size
    qint32 NeuronsCount() { return neurons.size(); } //get neurons size

    //iterator
    typedef LayerIterator<T> iterator;
    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, neurons.size() - 1); }

    //allocator
    typedef LayerAllocator<Layer> allocator;

    bool InvariantTest()
    {
        Q_ASSERT(inputsize == layerinput.size());

        for (int i = 0; i < neurons.size(); ++i)
        {
            neurons[i].InvariantTest();
        }

        return true;
    }

    T& Neuron(int i) //iterrator
    {
        //pre
        if (!(i >= 0 && i < neurons.size()))
            throw OutOfRange();
        //===

        return neurons[i];
    }

    //Creates the layer and allocates memory
    void Create(int inputsize, int neuroncount, float randgain = 1)
    {
        //pre
        if (!(inputsize > 0 && neuroncount > 0 && randgain > 0))
            throw WrongInputArgs();
        //===

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
    void PushInput(const QVector<float>& input)
    {
        //pre
        if (!(input.size() == layerinput.size()))
            throw WrongInputVectorSize();
        //===

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
        //pre
        if (!(gain > 0 && alpha > 0 && alpha < 1 && momentum > 0 && momentum < 1))
            throw WrongInputArgs();
        //===

        for (int i = 0; i < neurons.size(); i++)
        {
            neurons[i].gain = gain;
            neurons[i].alpha = alpha;
            neurons[i].momentum = momentum;
        }
    }

    friend QDataStream& operator<<(QDataStream& out, const Layer& obj)
    {
        out << obj.neurons << obj.inputsize;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, Layer& obj)
    {
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
    float Train(const QVector<float>& desiredoutput)
    {
        //pre
        if (!(desiredoutput.size() == this->neurons.size()))
            throw WrongInputVectorSize();
        //===

        float errsum = 0;
        for (int i = 0; i < this->neurons.size(); i++)
        {
            errsum += this->neurons[i].Train(this->layerinput, desiredoutput.at(i));
        }

        return errsum;
    }

    float EstimateError(const QVector<float>& desiredoutput)
    {
        //pre
        if (!(desiredoutput.size() == this->neurons.size()))
            throw WrongInputVectorSize();
        //===

        float errorg = 0;
        for (int i = 0; i < this->neurons.size(); i++)
        {
            errorg += pow((desiredoutput.at(i) - this->neurons[i].GetOutput()), 2.f);
        }

        return errorg / 2;
    }
};

/* ===== iterator for layer ===== */

template<class T> //must be neuron type
class LayerIterator
{
private:
    Layer<T>* container;
    uint position;
public:
    LayerIterator(Layer<T>& container, uint position)
    {
        this->container = &container;
        this->position = position;
    }
    bool operator==(LayerIterator& other)
    {
        return (container == (other.container) && position == other.position);
    }
    bool operator!=(LayerIterator& other)
    {
        return !(*this == other);
    }

    T& operator*()
    {
        if (position < 0 || (position > container->neurons.size() - 1))
            throw OutOfRange();
        return container->neurons.data()[position];
    }

    LayerIterator& operator++()
    {
        ++position;
        return *this;
    }

    LayerIterator& operator--()
    {
        --position;
        return *this;
    }
};

/* ===== allocator for layer ===== */

template <class T> //must be layer type
struct LayerAllocator
{
    typedef T value_type;
    LayerAllocator() = default;
    template <class U> LayerAllocator(const LayerAllocator<U>&) {}

    T* allocate(std::size_t n)
    {
        T* obj = new T();
        obj->neurons.resize(n);
        return static_cast<T*>(obj);
    }

    void deallocate(T* p, size_t n)
    {
        if (p->neurons.size() > 0)
            p->neurons.clear();
        delete p;
    }
};

template <class T, class U>
bool operator==(const LayerAllocator<T>&, const LayerAllocator<U>&)
{
    return true;
}

template <class T, class U>
bool operator!=(const LayerAllocator<T>&, const LayerAllocator<U>&)
{
    return false;
}

}

#endif // LAYER_H
