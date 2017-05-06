#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <streambuf>
#include <QFileDialog>
#include <QLabel>
#include <QTimer>
#include <train_thread.h>
#include <iomanip>
#include <sstream>
#include <QFile>
#include <QTextStream>

#define DELAY 500

using namespace std;

/*TODO:
1. Реализовать все методы, представленные в интерфейсе
2. При запуске программы должны загружаться последние настройки нейросетей
3. Реализовать отдельный класс для хранения параметров с сериализацией в файл
6. Также отчеты о пакетной проверке книг должны сохраняться в отдельных лог-файлах
*/


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    logger = new Logger(cout, ui->textEditOutput);
    logger->registerQDebugMessageHandler();

    paramsStatus = new QLabel(this);
    pagesStatus = new QLabel(this);
    ui->statusBar->addPermanentWidget(paramsStatus);
    ui->statusBar->addPermanentWidget(pagesStatus);

    t = new QTimer(this);
    t->setInterval(DELAY);

    checker = new BooksChecker();
    SetNetsStatus();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete paramsStatus;
    delete pagesStatus;
    delete t;
    if (trainThread)
        delete trainThread;
    delete checker;
}

void MainWindow::SetNetsStatus()
{
    if (checker->IsPagesLearned())
    {
        pagesStatus->setText("Pages: learned");
        ui->pushButtonTestPage->setEnabled(true);
    }
    else
    {
        pagesStatus->setText("Pages: unknown");
        ui->pushButtonTestPage->setEnabled(false);
    }

    if (checker->IsParamsLearned())
    {
        paramsStatus->setText("Params: learned");
        ui->pushButtonTestParams->setEnabled(true);
    }
    else
    {
        paramsStatus->setText("Params: unknown");
        ui->pushButtonTestParams->setEnabled(false);
    }

    if (checker->IsPagesLearned() && checker->IsParamsLearned())
    {
        ui->pushButtonTestBoth->setEnabled(true);
        ui->pushButtonTestBatch->setEnabled(true);
    }
    else
    {
        ui->pushButtonTestBoth->setEnabled(false);
        ui->pushButtonTestBatch->setEnabled(false);
    }
}


string MainWindow::GetLearningParamsFromGUI()
{
    stringstream str;
    str << fixed << setprecision(1) << (ui->doubleSpinBoxWeight->value()) << "; "
        << (ui->doubleSpinBoxHeight->value()) << "; "
        << (ui->doubleSpinBoxWidth->value()) << "; "
        << (ui->doubleSpinBoxThickness->value()) << "; "
        << (ui->doubleSpinBoxPages->value());

    return str.str();
}

string MainWindow::GetTestParamsFromGUI()
{
    stringstream str;
    str << fixed << setprecision(1) << (ui->doubleSpinBoxWeightTest->value()) << "; "
        << (ui->doubleSpinBoxHeightTest->value()) << "; "
        << (ui->doubleSpinBoxWidthTest->value()) << "; "
        << (ui->doubleSpinBoxThicknessTest->value()) << "; "
        << (ui->doubleSpinBoxPagesTest->value());

    return str.str();
}

//=================================================

void MainWindow::on_pushButtonLearnPages_clicked()
{
    if (ui->textEditBad->toPlainText().length() == 0 || ui->textEditGood->toPlainText().length() == 0)
    {
        Logger::Print() << "Pages to learn not selected" << endl;
        return;
    }

    Logger::Print() << "Started learning of pages" << endl;
    trainThread = new TrainThread(checker, true, this);
    connect(trainThread, SIGNAL(finished()), this, SLOT(trainPagesFinished()));

    connect(t, SIGNAL(timeout()), this, SLOT(trainPagesStatusbar()));
    t->start();
    trainThread->start();
    ui->tabWidget->setEnabled(false);
}

void MainWindow::trainPagesFinished()
{
    t->stop();
    delete trainThread;
    trainThread = NULL;

    Logger::Print() << "Learning pages patterns finished with error " << to_string(checker->LastPagesError()) << endl;
    SetNetsStatus();
    ui->tabWidget->setEnabled(true);
}

void MainWindow::trainPagesStatusbar()
{
    QString text("Learning pages... Current error: ");
    text += QString::number(checker->LastPagesError());
    ui->statusBar->showMessage(text, DELAY);
}

void MainWindow::on_pushButtonAddGood_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select images"), QString(), tr("Image Files (*.png *.jpg *.bmp)"));

    foreach (QString file, files)
    {
        checker->AddGoodPage(file);
        ui->textEditGood->append(file.split("/").last());
    }

    Logger::Print() << "Added " << to_string(files.size()) << " good pages" << endl;
}

void MainWindow::on_pushButtonClearGood_clicked()
{
    checker->ClearGoodPages();
    ui->textEditGood->clear();
    Logger::Print() << "Cleared good pages list" << endl;
}

void MainWindow::on_pushButtonAddBad_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select images"), QString(), tr("Image Files (*.png *.jpg *.bmp)"));

    foreach (QString file, files)
    {
        checker->AddBadPage(file);
        ui->textEditBad->append(file.split("/").last());
    }

    Logger::Print() << "Added " << to_string(files.size()) << " bad pages" << endl;
}

void MainWindow::on_pushButtonClearBad_clicked()
{
    checker->ClearBadPages();
    ui->textEditBad->clear();
    Logger::Print() << "Cleared bad pages list" << endl;
}

//=================================================

void MainWindow::on_pushButtonTestPage_clicked()
{
    ui->statusBar->showMessage("Verifying page...", 2000);

    if (pageToVerify.length() > 0)
    {
        bool result = checker->VerifyPage(pageToVerify);
        Logger::Print() << "Verified page \"" << (pageToVerify.split("/").last().toUtf8().constData()) << "\" - " << (result ? "good" : "bad") << " page" << endl;
    }
    else
        Logger::Print() << "Page to verify not selected" << endl;
}

void MainWindow::on_pushButtonTestLoadPage_clicked()
{
    pageToVerify = QFileDialog::getOpenFileName(this, tr("Select image"), QString(), tr("Image Files (*.png *.jpg *.bmp)"));
    Logger::Print() << "Selected page to verify - \"" << (pageToVerify.split("/").last().toUtf8().constData()) << "\"" << endl;
}


void MainWindow::on_pushButtonTestParams_clicked()
{
    ui->statusBar->showMessage("Verifying book params...", 2000);

    bool result = checker->VerifyParams(QString::fromStdString(GetTestParamsFromGUI()));
    Logger::Print() << "Verified book with params " << GetTestParamsFromGUI() << " - " << (result ? "good" : "bad") << " print" << endl;

}

void MainWindow::on_pushButtonTestBoth_clicked()
{
    ui->statusBar->showMessage("Verifying book...", 2000);

    QFile file(pageToVerify);
    if (file.exists())
    {
        bool resultPage = checker->VerifyPage(pageToVerify);

        bool resultParams = checker->VerifyParams(QString::fromStdString(GetTestParamsFromGUI()));

        Logger::Print() << "Verified book with params " << GetTestParamsFromGUI()
                        << " and page \"" << (pageToVerify.split("/").last().toUtf8().constData()) << "\" - "
                        << ((resultPage && resultParams) ? "good" : "bad") << " print" << endl;
    }
    else
        Logger::Print() << "Page to verify not selected" << endl;
}

void MainWindow::on_pushButtonTestClear_clicked()
{
    checker->ClearBooksToVerify();
    ui->textEditParamsBatch->clear();
    Logger::Print() << "Cleared books to verify list" << endl;
}

void MainWindow::on_pushButtonLoadBatch_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select data file"), QString(), tr("All Files (*)"));

    if (filename.length() == 0)
        return;

    vector<QString> lines;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    while (!in.atEnd())
    {
        lines.push_back(in.readLine());
    }

    file.close();

    int addedNum = lines.size();

    for (uint i = 0; i < lines.size(); i++)
    {
        QStringList list = lines[i].split(";");

        if (list.length() == 6)
        {
            QString toShow = list[0] + "; " + list[1] + "; " + list[2] + "; " + list[3]
                             + "; " + list[4] + "; " + (list.last().split("/").last());

            checker->AddBooksToVerify(lines[i]);
            ui->textEditParamsBatch->append(toShow);
        }
        else
        {
            Logger::Print() << "Incorrect line " << to_string(i) << " of input file" << endl;
            addedNum--;
        }
    }

    Logger::Print() << "Added " << to_string(addedNum) << " books to verify" << endl;
}

void MainWindow::on_pushButtonTestBatch_clicked()
{
    if (ui->textEditParamsBatch->toPlainText().length() == 0)
    {
        Logger::Print() << "Books to verify not loaded" << endl;
        return;
    }

    ui->statusBar->showMessage("Verifying books...", 2000);
    vector<int> result = checker->VerifyBooks();
    QStringList books = ui->textEditParamsBatch->toPlainText().split("\n");

    for (uint i = 0; i < result.size(); i++)
    {
        if (result[i] == -1)
        {
            Logger::Print() << "Page to verify not found. Can't verify book " << to_string(i) << endl;
        }
        else
        {
            string pageFile = books[i].split(";").last().split("/").last().trimmed().toUtf8().constData();
            string params = books[i].mid(0, books[i].length() - books[i].split(";").last().length() - 1).toUtf8().constData();

            Logger::Print() << "Verified book " << to_string(i) << " with params " << params
                            << " and page \"" << pageFile << "\" - "
                            << ((result[i] == 1) ? "good" : "bad") << " print" << endl;
        }
    }
}


//=================================================


void MainWindow::on_pushButtonClearParams_clicked()
{
    checker->ClearParamsToLearn();
    ui->textEditParams->clear();
    Logger::Print() << "Cleared params to learn list" << endl;
}

void MainWindow::on_pushButton_clicked()
{
    checker->AddParamsToLearn(QString::fromStdString(GetLearningParamsFromGUI()));

    ui->textEditParams->append(QString::fromStdString(GetLearningParamsFromGUI()));
    Logger::Print() << "Added params dataset to learn" << endl;

}

void MainWindow::on_pushButtonLearnParams_clicked()
{
    if (ui->textEditParams->toPlainText().length() == 0)
    {
        Logger::Print() << "Params to learn not defined" << endl;
        return;
    }

    Logger::Print() << "Started learning of params" << endl;
    trainThread = new TrainThread(checker, false, this);
    connect(trainThread, SIGNAL(finished()), this, SLOT(trainParamsFinished()));

    connect(t, SIGNAL(timeout()), this, SLOT(trainParamsStatusbar()));
    t->start();
    trainThread->start();
    ui->tabWidget->setEnabled(false);
}

void MainWindow::trainParamsFinished()
{
    t->stop();
    delete trainThread;
    trainThread = NULL;

    Logger::Print() << "Learning params patterns finished with error " << to_string(checker->LastParamsError()) << endl;
    SetNetsStatus();
    ui->tabWidget->setEnabled(true);
}

void MainWindow::trainParamsStatusbar()
{
    QString text("Learning params... Current error: ");
    text += QString::number(checker->LastParamsError());
    ui->statusBar->showMessage(text, DELAY);
}

void MainWindow::on_pushButtonResetPages_clicked()
{
    checker->ResetPagesNet();
    SetNetsStatus();
    Logger::Print() << "Pages checker reset" << endl;
}

void MainWindow::on_pushButtonResetParams_clicked()
{
    checker->ResetParamsNet();
    SetNetsStatus();
    Logger::Print() << "Params checker reset" << endl;
}

void MainWindow::on_pushButtonLoadParamsDatasets_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select data file"), QString(), tr("All Files (*)"));

    if (filename.length() == 0)
        return;

    vector<QString> lines;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    while (!in.atEnd())
    {
        lines.push_back(in.readLine());
    }

    file.close();

    int addedNum = lines.size();

    for (uint i = 0; i < lines.size(); i++)
    {
        QStringList list = lines[i].split(";");

        //if in the end of line placed page filename
        QString toRemove;
        if (list.length() == 6)
        {
            toRemove  = list.last();
            list.removeLast();
        }

        //if line contains 5 params
        if (list.length() == 5)
        {
            QString params = lines[i].mid(0, lines[i].length() - toRemove.length() - 1);

            checker->AddParamsToLearn(params);
            ui->textEditParams->append(params);
        }
        else
        {
            Logger::Print() << "Incorrect line " << to_string(i) << " of input file" << endl;
            addedNum--;
        }
    }


    Logger::Print() << "Added " << to_string(addedNum) << " params datasets to learn" << endl;
}
