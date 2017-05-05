#ifndef TRAIN_THREAD
#define TRAIN_THREAD
#include <QThread>
#include <books_checker.h>

class TrainThread: public QThread
{
public:
    TrainThread(BooksChecker* checker, bool trainPages, QObject* parent = 0): QThread(parent)
    {
        this->trainPages = trainPages;
        this->checker = checker;
        this->trainParams = !trainPages;
    }

    ~TrainThread() {}

protected:
    void run() override
    {
        if (trainPages)
            checker->LearnPages();
        //if (trainParams) checker->
    }

private:
    bool trainPages;
    bool trainParams;
    BooksChecker* checker;
};

#endif // TRAIN_THREAD

