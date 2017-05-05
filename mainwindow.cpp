#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <streambuf>
#include <QFileDialog>
#include <QLabel>
#include <QTimer>
#include <train_thread.h>

using namespace std;

/*TODO:
1. Реализовать все методы, представленные в интерфейсе
2. При запуске программы должны загружаться последние настройки нейросетей
3. Реализовать отдельный класс для хранения параметров с сериализацией в файл
4. В окне output выводятся все действия с системой: загрузка и сохранение параметров, обучение, проверка книг
5. В статусбаре отображается состояние двух нейросетей (обучена - не обучена - обучается) и последняя операция, например: загружено 100 строк или сеть обучена с ошибкой 0.01 и т.д.
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
    SetStatus();

    t = new QTimer(this);
    t->setInterval(1000);

    checker = new BooksChecker();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete paramsStatus;
    delete pagesStatus;
    delete t;
    delete trainThread;
    delete checker;
}

void MainWindow::SetStatus()
{
    if (checker->PagesLearned)
        pagesStatus->setText("Pages: learned");
    else
        pagesStatus->setText("Pages: unknown");

    if (checker->ParamsLearned)
        paramsStatus->setText("Params: learned");
    else
        paramsStatus->setText("Params: unknown");
}

void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
}

void MainWindow::on_pushButtonLearnPages_clicked()
{
    trainThread = new TrainThread(checker, true, this);
    connect(trainThread, SIGNAL(finished()), this, SLOT(trainPagesFinished()));

    connect(t, SIGNAL(timeout()), this, SLOT(trainPagesStatusbar()));
    t->start();
    trainThread->start();
}

void MainWindow::trainPagesFinished()
{
    t->stop();
    delete trainThread;

    Logger::Print() << "Learned pages patterns with error " << to_string(checker->LastPagesError()) << endl;
    checker->PagesLearned = true;
    SetStatus();
}

void MainWindow::trainPagesStatusbar()
{
    QString text("Learning... Current error: ");
    text += QString::number(checker->LastPagesError());
    ui->statusBar->showMessage(text, 1000);
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
