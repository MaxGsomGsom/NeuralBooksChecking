#ifndef TRAIN_THREAD
#define TRAIN_THREAD

#include <books_checker.h>
#include <QThread>

class TrainThread: public QThread
{
public:
    TrainThread(BooksChecker* checker, bool trainPages, QObject* parent = 0): QThread(parent)
    {
        this->trainPages = trainPages;
        this->checker = checker;
    }

    ~TrainThread() {}

protected:
    void run() override
    {
        if (trainPages)
            checker->LearnPages();
        else
            checker->LearnParams();
    }

private:
    bool trainPages;
    BooksChecker* checker;
};

#endif // TRAIN_THREAD

