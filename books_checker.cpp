#include "books_checker.h"
#include <iostream>

using namespace Neuronets;
using namespace std;

#define ITERS 10000
#define STOP_ERR 0.001

#define INPUT_SIZE 2
#define NETWORK_INPUTNEURONS 5
#define NETWORK_OUTPUTNEURONS 1

#define PATTERNS 4

BooksChecker::BooksChecker()
{ 
    vector<int> hidden;
    hidden.push_back(5); //one hidden layer with 5 neurons

    pagesChecker.Create(INPUT_SIZE, NETWORK_INPUTNEURONS, NETWORK_OUTPUTNEURONS, &hidden);
    pagesChecker.SetParams();
}

void BooksChecker::LearnPages()
{
    //load imgs
    //float error = pagesChecker.TrainAll(&desiredouts, &patterns);
}

void BooksChecker::ClearGoodPages()
{
    goodPages.clear();
}

void BooksChecker::AddGoodPage(string file)
{
    goodPages.push_back(file);
}

