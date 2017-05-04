#ifndef BOOKS_CHECKER_H
#define BOOKS_CHECKER_H

#include <neuronet.h>

using namespace Neuronets;

class BooksChecker
{
public:
    BooksChecker();
    void LearnPages();
    void ClearGoodPages();
    void AddGoodPage(string file);

private:
    Neuronet<> pagesChecker;
    vector<string> goodPages;
    vector<string> badPages;
};

#endif // BOOKS_CHECKER_H
