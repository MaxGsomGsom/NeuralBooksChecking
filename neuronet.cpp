#include "neuronet.h"
#include <cstdlib>
#include <cmath>

Neuronet::Neuronet()
{
}

Neuronet::~Neuronet()
{
}

void Neuronet::Create(int inputcount,int inputneurons,int outputcount,vector<int> *hiddenlayers)
{
            m_inputlayer.Create(inputcount,inputneurons);
            if(hiddenlayers->size()>0)
            {
                m_hiddenlayers.resize(hiddenlayers->size());
                for(uint i=0;i<hiddenlayers->size();i++)
                {
                    if(i==0)
                    {
                        //first hidden layer receives the output of the inputlayer so we set as input the neuroncount
                        //of the inputlayer
                        m_hiddenlayers[i].Create(inputneurons,hiddenlayers->at(i));
                    }
                    else
                    {
                        m_hiddenlayers[i].Create(hiddenlayers->at(i-1),hiddenlayers->at(i));
                    }
                }
                m_outputlayer.Create(hiddenlayers->at(hiddenlayers->size() - 1),outputcount);
            }
            else
            {
                m_outputlayer.Create(inputneurons,outputcount);
            }
}


void Neuronet::Propagate(vector<float> *input)
{
    //The propagation function should start from the input layer
    //first copy the input vector to the input layer Always make sure the size
    //"array input" has the same size of inputcount
    copy(input->begin(), input->end(), m_inputlayer.layerinput.begin());
    //now calculate the inputlayer
    m_inputlayer.Calculate();

    Update(-1);//propagate the inputlayer out values to the next layer
    if(m_hiddenlayers.size()>0)
    {
        //Calculating hidden layers if any
        for(uint i=0;i<m_hiddenlayers.size();i++)
        {
            m_hiddenlayers[i].Calculate();
            Update(i);
        }
    }

    //calculating the final statge: the output layer
    m_outputlayer.Calculate();
}
//Main training function. Run this function in a loop as many times needed per pattern
float Neuronet::Train(vector<float> *desiredoutput, vector<float> *input, float alpha, float momentum)
{
    //function train, teaches the network to recognize a pattern given a desired output
    float errorg=0; //general quadratic error
    float errorc; //local error;
    float sum=0,csum=0;
    float delta,udelta;
    float output;
    //first we begin by propagating the input
    Propagate(input);

    //the backpropagation algorithm starts from the output layer propagating the error  from the output
    //layer to the input layer
    for(uint i=0;i<m_outputlayer.neurons.size();i++)
    {
        //calculate the error value for the output layer
        output=m_outputlayer.neurons[i].output; //copy this value to facilitate calculations
        //from the algorithm we can take the error value as
        errorc=(desiredoutput->at(i) - output) * output * (1 - output);
        //and the general error as the sum of delta values. Where delta is the squared difference
        //of the desired value with the output value
        //quadratic error
        errorg+=pow((desiredoutput->at(i) - output), 2.f);

        //now we proceed to update the weights of the neuron
        for(uint j=0;j<m_outputlayer.layerinput.size();j++)
        {
            //get the current delta value
            delta=m_outputlayer.neurons[i].deltavalues[j];
            //update the delta value
            udelta=alpha * errorc * m_outputlayer.layerinput[j] + delta * momentum;
            //update the weight values
            m_outputlayer.neurons[i].weights[j]+=udelta;
            m_outputlayer.neurons[i].deltavalues[j]=udelta;

            //we need this to propagate to the next layer
            sum+=m_outputlayer.neurons[i].weights[j] * errorc;
        }

        //calculate the weight gain
        m_outputlayer.neurons[i].wgain+= alpha * errorc * m_outputlayer.neurons[i].gain;

    }

    for(int i=(m_hiddenlayers.size() - 1);i>=0;i--)
    {
        for(uint j=0;j<m_hiddenlayers[i].neurons.size();j++)
        {
            output=m_hiddenlayers[i].neurons[j].output;
            //calculate the error for this layer
            errorc= output * (1-output) * sum;
            //update neuron weights
            for(uint k=0;k<m_hiddenlayers[i].layerinput.size();k++)
            {
                delta=m_hiddenlayers[i].neurons[j].deltavalues[k];
                udelta= alpha * errorc * m_hiddenlayers[i].layerinput[k] + delta * momentum;
                m_hiddenlayers[i].neurons[j].weights[k]+=udelta;
                m_hiddenlayers[i].neurons[j].deltavalues[k]=udelta;
                csum+=m_hiddenlayers[i].neurons[j].weights[k] * errorc;//needed for next layer

            }

            m_hiddenlayers[i].neurons[j].wgain+=alpha * errorc * m_hiddenlayers[i].neurons[j].gain;

        }
        sum=csum;
        csum=0;
    }

    //and finally process the input layer
    for(uint i=0;i<m_inputlayer.neurons.size();i++)
    {
        output=m_inputlayer.neurons[i].output;
        errorc=output * (1 - output) * sum;

        for(uint j=0;j<m_inputlayer.layerinput.size();j++)
        {
            delta=m_inputlayer.neurons[i].deltavalues[j];
            udelta=alpha * errorc * m_inputlayer.layerinput[j] + delta * momentum;
            //update weights
            m_inputlayer.neurons[i].weights[j]+=udelta;
            m_inputlayer.neurons[i].deltavalues[j]=udelta;
        }
        //and update the gain weight
        m_inputlayer.neurons[i].wgain+=alpha * errorc * m_inputlayer.neurons[i].gain;
    }

    //return the general error divided by 2
    return errorg / 2;

}

void Neuronet::Update(int layerindex)
{
    if(layerindex==-1)
    {
        //dealing with the inputlayer here and propagating to the next layer
        for(uint i=0;i<m_inputlayer.neurons.size();i++)
        {
            if(m_hiddenlayers.size()>0)//propagate to the first hidden layer
            {
                m_hiddenlayers[0].layerinput[i]=m_inputlayer.neurons[i].output;
            }
            else //propagate directly to the output layer
            {
                m_outputlayer.layerinput[i]=m_inputlayer.neurons[i].output;
            }
        }

    }
    else
    {
        for(uint i=0;i<m_hiddenlayers[layerindex].neurons.size();i++)
        {
            //not the last hidden layer
            if(layerindex < (int)m_hiddenlayers.size() - 1)
            {
                m_hiddenlayers[layerindex + 1].layerinput[i]=m_hiddenlayers[layerindex].neurons[i].output;
            }
            else
            {
                m_outputlayer.layerinput[i]=m_hiddenlayers[layerindex].neurons[i].output;
            }
        }
    }
}

