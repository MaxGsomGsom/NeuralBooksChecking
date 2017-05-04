#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <streambuf>
#include <QFileDialog>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
}

void MainWindow::on_pushButtonLearnPages_clicked()
{
    checker.LearnPages();
}

void MainWindow::on_pushButtonAddGood_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
        tr("Select images"), QString(), tr("Image Files (*.png *.jpg *.bmp)"));

    foreach (QString file, files) {
        checker.AddGoodPage(file.toUtf8().constData());
        ui->textEditGood->append(file.split("/").last());
    }

    Logger::Print() << "Added " << to_string(files.size()) << " good pages" << endl;
}

void MainWindow::on_pushButtonClearGood_clicked()
{
    checker.ClearGoodPages();
    ui->textEditGood->clear();
    Logger::Print() << "Cleared good pages list" << endl;
}
