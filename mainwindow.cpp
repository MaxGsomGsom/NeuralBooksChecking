#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <neuronet.h>

using namespace std;
using namespace Neuronets;



/*TODO:
1. Реализовать все методы, представленные в интерфейсе
2. При запуске программы должны загружаться последние настройки нейросетей
3. Реализовать отдельный класс для хранения параметров с сериализацией в файл
4. В окне output выводятся все действия с системой: загрузка и сохранение параметров, обучение, проверка книг
5. В статусбаре отображается состояние двух нейросетей (обучена - не обучена - обучается) и последняя операция, например: загружено 100 строк или сеть обучена с ошибкой 0.01 и т.д.
6. Также отчеты о пакетной проверке книг должны сохраняться в отдельных лог-файлах
*/

#define ITERS 10000
#define STOP_ERR 0.001

#define INPUT_SIZE 2
#define NETWORK_INPUTNEURONS 5
#define NETWORK_OUTPUTNEURONS 1

#define PATTERNS 4


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Test()
{
    //Create some patterns
    //playing with xor
    //XOR input values
    vector< vector<float> > patterns;
    patterns.resize(PATTERNS);
    patterns[0].push_back(0);
    patterns[0].push_back(0);

    patterns[1].push_back(0);
    patterns[1].push_back(1);

    patterns[2].push_back(1);
    patterns[2].push_back(0);

    patterns[3].push_back(1);
    patterns[3].push_back(1);

    //XOR desired output values
    vector< vector<float> > desiredouts;
    desiredouts.resize(PATTERNS);
    desiredouts[0].push_back(0);
    desiredouts[1].push_back(1);
    desiredouts[2].push_back(1);
    desiredouts[3].push_back(0);

    vector<int> hidden;
    hidden.push_back(5); //one hidden layer with 5 neurons

    Neuronet<> net;//Our neural network object
    net.Create(INPUT_SIZE, NETWORK_INPUTNEURONS, NETWORK_OUTPUTNEURONS, &hidden);
    net.SetParams();

    float error = net.TrainAll(&desiredouts, &patterns);

    cout << "ERROR " << error << endl;

    //once trained test all patterns
    for (int i = 0; i < PATTERNS; i++)
    {
        net.Propagate(&patterns[i]);
        //display result
        cout << "TESTED PATTERN " << i << " | INPUT: " << patterns[i][0] << "," << patterns[i][1] << " | DESIRED OUTPUT: " << desiredouts[i][0] << " | RESULT: " << net.GetOutput()[0] << endl;
    }

}
