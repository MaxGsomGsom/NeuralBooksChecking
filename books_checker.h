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

    bool PagesLearned = false;
    bool ParamsLearned = false;
    float LastPagesError() { return pagesChecker.LastError(); }

private:
    Neuronet<> pagesChecker;
    vector<QString> goodPages;
    vector<QString> badPages;
};

#endif // BOOKS_CHECKER_H
