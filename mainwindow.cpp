#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <neuronet.h>

using namespace std;
#define PATTERN_COUNT 4
#define EPOCHS 10000

#define PATTERN_SIZE 2
#define NETWORK_INPUTNEURONS 5
#define NETWORK_OUTPUT 1


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

void MainWindow::on_pushButton_clicked()
{
    //Create some patterns
    //playing with xor
    //XOR input values
    vector< vector<float> > pattern;
    pattern.resize(PATTERN_COUNT);
    pattern[0].push_back(0);
    pattern[0].push_back(0);

    pattern[1].push_back(0);
    pattern[1].push_back(1);

    pattern[2].push_back(1);
    pattern[2].push_back(0);

    pattern[3].push_back(1);
    pattern[3].push_back(1);

    //XOR desired output values
    vector< vector<float> > desiredout;
    desiredout.resize(PATTERN_COUNT);
    desiredout[0].push_back(0);
    desiredout[1].push_back(1);
    desiredout[2].push_back(1);
    desiredout[3].push_back(0);

    Neuronet net;//Our neural network object
    float error;

    vector<int> hidden;
    hidden.push_back(5);
    //We create the network
    net.Create(PATTERN_SIZE, NETWORK_INPUTNEURONS, NETWORK_OUTPUT, &hidden);

    //Start the neural network training
    for (int i = 0; i < EPOCHS; i++)
    {
        error = 0;
        for (int j = 0; j < PATTERN_COUNT; j++)
        {
            error += net.Train(&(desiredout[j]), &(pattern[j]), 0.2f, 0.1f);
        }
        error /= PATTERN_COUNT;
        //display error
        cout << "ERROR:" << error << "\r";
        if (error < 0.001)
            break;
    }

    //once trained test all patterns

    for (int i = 0; i < PATTERN_COUNT; i++)
    {

        net.Propagate(&pattern[i]);

        //display result
        cout << "TESTED PATTERN " << i << " INPUT: " << pattern[i][0] << "," << pattern[i][1] << " DESIRED OUTPUT: " << desiredout[i][0] << " NET RESULT: " << net.GetOutput().neurons[0].output << endl;
    }

}
