#include "books_checker.h"
#include <iostream>
#include <QImage>

using namespace Neuronets;
using namespace std;

#define ITERS 10000
#define STOP_ERR 0.1

#define NETWORK_INPUTNEURONS 100
#define NETWORK_OUTPUTNEURONS 1
#define NETWORK_HIDDENEURONS 10

#define IMG_HEIGHT 210
#define IMG_WIDTH 148

BooksChecker::BooksChecker()
{
    vector<int> hidden;
    hidden.push_back(NETWORK_HIDDENEURONS); //one hidden layer

    pagesChecker.Create(IMG_HEIGHT * IMG_WIDTH, NETWORK_INPUTNEURONS, NETWORK_OUTPUTNEURONS, &hidden);
    pagesChecker.SetParams();
}

float BooksChecker::LearnPages()
{
    int patternsNum = goodPages.size() + badPages.size();
    vector< vector<float> > patterns;
    patterns.resize(patternsNum);

    vector< vector<float> > outputs;
    outputs.resize(patternsNum);

    for (uint i = 0; i < goodPages.size(); i++)
    {
        QImage img(goodPages[i]);
        img = img.scaled(IMG_WIDTH, IMG_HEIGHT);

        outputs[i].push_back(1.f);

        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                patterns[i].push_back(img.pixel(x, y) & 0x00FFFFFF);
            }
        }

    }

    for (uint i = 0; i < badPages.size(); i++)
    {
        QImage img(badPages[i]);
        img = img.scaled(IMG_WIDTH, IMG_HEIGHT);

        outputs[i + goodPages.size()].push_back(0.f);

        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                patterns[i + goodPages.size()].push_back(img.pixel(x, y) & 0x00FFFFFF);
            }
        }

    }

    float error = pagesChecker.TrainAll(&outputs, &patterns, ITERS, STOP_ERR);
    return error;
}

void BooksChecker::ClearGoodPages()
{
    goodPages.clear();
}

void BooksChecker::AddGoodPage(QString file)
{
    goodPages.push_back(file);
}

void BooksChecker::ClearBadPages()
{
    badPages.clear();
}

void BooksChecker::AddBadPage(QString file)
{
    badPages.push_back(file);
}
