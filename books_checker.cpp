#include "books_checker.h"
#include <QImage>
#include <QStringList>
#include <QFile>

using namespace Neuronets;
using namespace std;

#define ITERS 10000

#define INPUTNEURONS_PAGES 100
#define OUTPUTNEURONS_PAGES 1
#define HIDDENEURONS_PAGES 10
#define IMG_HEIGHT 210
#define IMG_WIDTH 148
#define BORDER_PAGES 0.5
#define STOP_ERR_PAGES 0.1

#define INPUT_PARAMS 5
#define INPUTNEURONS_PARAMS 10
#define OUTPUTNEURONS_PARAMS 1
#define HIDDENEURONS_PARAMS 5
#define BORDER_PARAMS 0.9
#define STOP_ERR_PARAMS 0.001
#define DIFF 200

BooksChecker::BooksChecker()
{
    ResetPagesNet();
    ResetParamsNet();
}

void BooksChecker::ResetPagesNet()
{
    QVector<int> hidden;
    hidden.push_back(HIDDENEURONS_PAGES); //one hidden layer
    pagesChecker.Create(IMG_HEIGHT * IMG_WIDTH, INPUTNEURONS_PAGES, OUTPUTNEURONS_PAGES, hidden);
    pagesChecker.SetParams();
}

void BooksChecker::ResetParamsNet()
{
    QVector<int> hidden2;
    hidden2.push_back(HIDDENEURONS_PARAMS); //one hidden layer
    paramsChecker.Create(INPUT_PARAMS, INPUTNEURONS_PARAMS, OUTPUTNEURONS_PARAMS, hidden2);
    paramsChecker.SetParams();
}

float BooksChecker::LearnPages()
{
    int patternsNum = goodPages.size() + badPages.size();
    QVector< QVector<float> > patterns;
    patterns.resize(patternsNum);

    QVector< QVector<float> > outputs;
    outputs.resize(patternsNum);

    for (int i = 0; i < goodPages.size(); i++)
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

    for (int i = 0; i < badPages.size(); i++)
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

    float error = pagesChecker.TrainAll(outputs, patterns, ITERS, STOP_ERR_PAGES);
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


//====================================


void BooksChecker::ClearParamsToLearn()
{
    paramsToLearn.clear();
}

void BooksChecker::AddParamsToLearn(QString params)
{

    QVector<float> dataset;
    QStringList list = params.split(";");
    foreach (QString param, list)
    {
        dataset.push_back(param.toFloat());
    }
    paramsToLearn.push_back(dataset);
}

float BooksChecker::LearnParams()
{
    QVector< QVector<float> > outputs;
    QVector< QVector<float> > patterns;

    QVector<float> output1;
    output1.push_back(1.f);

    QVector<float> output0;
    output0.push_back(0.f);

    for (int i = 0; i < paramsToLearn.size(); i++)
    {

        //add good pattern
        outputs.push_back(output1);
        patterns.push_back(paramsToLearn[i]);

        //add two bad patterns
        outputs.push_back(output0);
        outputs.push_back(output0);
        QVector<float> max;
        QVector<float> min;

        for (int k = 0; k < INPUT_PARAMS; k++)
        {
            max.push_back(paramsToLearn[i][k] + DIFF);
            min.push_back(paramsToLearn[i][k] - DIFF);
        }
        patterns.push_back(max);
        patterns.push_back(min);
    }


    float error = paramsChecker.TrainAll(outputs, patterns, ITERS, STOP_ERR_PARAMS);
    return error;
}


//======================================


//verify page of book
bool BooksChecker::VerifyPage(QString file)
{
    QImage img(file);
    img = img.scaled(IMG_WIDTH, IMG_HEIGHT);
    QVector<float> input;
    for (int y = 0; y < img.height(); y++)
    {
        for (int x = 0; x < img.width(); x++)
        {
            input.push_back(img.pixel(x, y) & 0x00FFFFFF);
        }
    }
    pagesChecker.Propagate(input);
    if (pagesChecker.GetOutput()[0] > BORDER_PAGES)
        return true;
    else
        return false;
}

//verify params of book
bool BooksChecker::VerifyParams(QString params)
{
    QVector<float> input;
    QStringList list = params.split(";");
    foreach (QString param, list)
    {
        input.push_back(param.toFloat());
    }

    paramsChecker.Propagate(input);
    if (paramsChecker.GetOutput()[0] > BORDER_PARAMS)
        return true;
    else
        return false;
}

//adding pages and params to verify
void BooksChecker::AddBooksToVerify(QString params)
{
    QVector<float> dataset;
    QStringList list = params.split(";");
    for (int i = 0; i < INPUT_PARAMS; i++)
    {
        dataset.push_back(list[i].toFloat());
    }
    booksToVerify.push_back(dataset);
    pagesToVerify.push_back(list.last());

}

//clear books list
void BooksChecker::ClearBooksToVerify()
{
    booksToVerify.clear();
    pagesToVerify.clear();
}



QVector<int> BooksChecker::VerifyBooks()
{

    QVector<int> result;

    for (int i = 0; i < booksToVerify.size(); i++)
    {
        if (QFile::exists(pagesToVerify[i].trimmed()))
        {
            //===================

            //verify page
            bool resultPage = false;
            QImage img(pagesToVerify[i].trimmed());
            img = img.scaled(IMG_WIDTH, IMG_HEIGHT);
            QVector<float> input;
            for (int y = 0; y < img.height(); y++)
            {
                for (int x = 0; x < img.width(); x++)
                {
                    input.push_back(img.pixel(x, y) & 0x00FFFFFF);
                }
            }
            pagesChecker.Propagate(input);
            if (pagesChecker.GetOutput()[0] > BORDER_PAGES)
                resultPage = true;

            //===================

            //verify params
            bool resultParams = false;
            paramsChecker.Propagate(booksToVerify[i]);
            if (paramsChecker.GetOutput()[0] > BORDER_PARAMS)
                resultParams = true;

            //===================

            result.push_back(resultPage && resultParams);
        }
        else
            result.push_back(-1);
    }

    return result;
}

//=============================================

bool BooksChecker::SavePagesNet(QString file){
    QFile outfile(file);
    if (!outfile.open(QIODevice::WriteOnly)) return false;
    QDataStream stream(&outfile);

    stream << pagesChecker;
    outfile.close();
    return true;
}

bool BooksChecker::SaveParamsNet(QString file){
    QFile outfile(file);
    if (!outfile.open(QIODevice::WriteOnly)) return false;
    QDataStream stream(&outfile);

    stream << paramsChecker;
    outfile.close();
    return true;
}

bool BooksChecker::LoadPagesNet(QString file){
    QFile outfile(file);
    if (!outfile.open(QIODevice::ReadOnly)) return false;
    QDataStream stream(&outfile);

    stream >> pagesChecker;
    outfile.close();
    return true;
}

bool BooksChecker::LoadParamsNet(QString file){
    QFile outfile(file);
    if (!outfile.open(QIODevice::ReadOnly)) return false;
    QDataStream stream(&outfile);

    stream >> paramsChecker;
    outfile.close();
    return true;
}
