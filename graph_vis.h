#include <vector>
#include <fstream>
#include "neurolayer.h"

namespace Neuronets
{
template < class T_in = InputLayer<>, class T_out = OutputLayer<>, class T_hid = HiddenLayer<> >
class NeuronNetVis
{
private:

    T_in inputLayer;
    T_hid hiddenLayer;
    T_out outputLayer;

    std::string beforeFirstLayer;
    std::string beforeSecondLayer;
    std::string beforeThirdLayer;
    std::string afterThirdLayer;
    std::string afterAll;


public:

    NeuronNetVis()
    {
        beforeFirstLayer =  "digraph G {"
                            "nodesep=0;"
                            "splines=false;"
                            "clusterrank=local;"
                            "node[width=1, shape=circle];"
                            "edge[style=invis];"
                            "subgraph cluster_0 {"
                            "label = \"Neural network\";"
                            "subgraph cluster_3 {"
                            "label=\"Output layer\";";
        beforeSecondLayer = "}"
                            "subgraph cluster_2 {"
                            "label=\"Hidden layer\";";
        beforeThirdLayer =  "}"
                            "subgraph cluster_1 {"
                            "label=\"Input layer\";";
        afterThirdLayer = "}"
                          "}"
                          "edge[style=solid, penwidth=1, constraint=false];";
        afterAll = "} ";
    }

    void load(T_in inp,
              T_hid hid,
              T_out outp)
    {
        inputLayer = inp;
        hiddenLayer = hid;
        outputLayer = outp;
    }

    std::string printNode(std::string id, std::string val)
    {
        return "{" + id + "[label=\"" + val + "\"]}";
    }

    std::string printEdge(std::string id1, std::string id2)
    {
        return id1 + " -> " + id2 + ";";
    }

    std::string generateAllPairs(int inp, int hid, int outp)
    {
        std::string s = "";
        for (int i = 0; i < inp; i++)
            for (int j = 0; j < hid; j++)
                s += printEdge("i" + std::to_string(i), "h" + std::to_string(j));
        for (int i = 0; i < hid; i++)
            for (int j = 0; j < outp; j++)
                s += printEdge("h" + std::to_string(i), "o" + std::to_string(j));
        return s;
    }

    void generate()
    {
        std::ofstream file;
        file.open("out.gv");
        file << beforeFirstLayer; //OUTPUT
        for (int i = 0; i < outputLayer.NeuronsCount(); i++)
        {
            file << printNode("o" + std::to_string(i), std::to_string(outputLayer.Neuron(i).GetWGain()));
            if (i != outputLayer.NeuronsCount() - 1)
                file << "->";
            else
                file << ";";
        }

        file << beforeSecondLayer; //HIDDEN
        for (int i = 0; i < hiddenLayer.NeuronsCount(); i++)
        {
            file << printNode("h" + std::to_string(i), std::to_string(hiddenLayer.Neuron(i).GetWGain()));
            if (i != hiddenLayer.NeuronsCount() - 1)
                file << "->";
            else
                file << ";";
        }
        file << beforeThirdLayer; //INPUT
        for (int i = 0; i < inputLayer.NeuronsCount(); i++)
        {
            file << printNode("i" + std::to_string(i), std::to_string(inputLayer.Neuron(i).GetWGain()));
            if (i != inputLayer.NeuronsCount() - 1)
                file << "->";
            else
                file << ";";
        }
        file << afterThirdLayer;
        file << generateAllPairs(inputLayer.NeuronsCount(), hiddenLayer.NeuronsCount(), outputLayer.NeuronsCount());
        file << afterAll;
        file.close();
        system("dot -Tps out.gv -o graph.ps");
    }
};
}
