#ifndef BOOKS_CHECKER_H
#define BOOKS_CHECKER_H

#include <neuronet.h>

using namespace Neuronets;

namespace books_checker
{

class BooksChecker
{
public:
    BooksChecker();
    float LearnPages(); //returns error
    void ClearGoodPages();
    void AddGoodPage(QString file);
    void ClearBadPages();
    void AddBadPage(QString file);
    bool VerifyPage(QString file); //returns result
    bool IsPagesLearned() { return pagesChecker.IsTrained(); }
    float LastPagesError() { return pagesChecker.LastError(); }
    bool IsParamsLearned() { return paramsChecker.IsTrained(); }
    float LastParamsError() { return paramsChecker.LastError(); }
    void ClearParamsToLearn();
    void AddParamsToLearn(QString params); //weight; height; width; thickness; pagesNum
    float LearnParams(); //returns error
    void ResetParamsNet();
    void ResetPagesNet();
    bool VerifyParams(QString params); //weight; height; width; thickness; pagesNum
    void AddBooksToVerify(QString params); //weight; height; width; thickness; pagesNum; page filename
    void ClearBooksToVerify();
    QVector<int> VerifyBooks();
    bool SavePagesNet(QString file);
    bool SaveParamsNet(QString file);
    bool LoadPagesNet(QString file);
    bool LoadParamsNet(QString file);

private:
    Neuronet<> pagesChecker;
    QVector<QString> goodPages;
    QVector<QString> badPages;
    Neuronet<> paramsChecker;
    QVector<QVector<float>> paramsToLearn;
    QVector< QVector<float> > booksToVerify;
    QVector<QString> pagesToVerify;
};

}

#endif // BOOKS_CHECKER_H
