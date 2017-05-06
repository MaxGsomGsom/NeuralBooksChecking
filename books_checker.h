#ifndef BOOKS_CHECKER_H
#define BOOKS_CHECKER_H

#include <neuronet.h>
#include <QString>

using namespace Neuronets;

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
    vector<int> VerifyBooks();

private:
    Neuronet<> pagesChecker;
    vector<QString> goodPages;
    vector<QString> badPages;
    Neuronet<> paramsChecker;
    vector<vector<float>> paramsToLearn;
    vector< vector<float> > booksToVerify;
    vector<QString> pagesToVerify;
};

#endif // BOOKS_CHECKER_H
