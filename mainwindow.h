#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <books_checker.h>
#include <logger.h>
#include <QLabel>
#include <train_thread.h>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    BooksChecker* checker;
    Logger* logger;
    QLabel* paramsStatus;
    QLabel* pagesStatus;
    void SetNetsStatus();
    QTimer* t;
    TrainThread* trainThread = NULL;
    QString pageToVerify;
    string GetLearningParamsFromGUI();
    string GetTestParamsFromGUI();

private slots:
    void on_pushButtonLearnPages_clicked();
    void on_pushButtonAddGood_clicked();
    void on_pushButtonClearGood_clicked();
    void on_pushButtonAddBad_clicked();
    void on_pushButtonClearBad_clicked();
    void trainPagesStatusbar();
    void trainPagesFinished();
    void on_pushButtonTestPage_clicked();
    void on_pushButtonTestLoadPage_clicked();
    void on_pushButtonClearParams_clicked();
    void on_pushButton_clicked();
    void on_pushButtonLearnParams_clicked();
    void trainParamsFinished();
    void trainParamsStatusbar();
    void on_pushButtonResetPages_clicked();
    void on_pushButtonResetParams_clicked();
    void on_pushButtonTestParams_clicked();
    void on_pushButtonTestBoth_clicked();
    void on_pushButtonLoadParamsDatasets_clicked();
    void on_pushButtonTestClear_clicked();
    void on_pushButtonLoadBatch_clicked();
    void on_pushButtonTestBatch_clicked();
};

#endif // MAINWINDOW_H
