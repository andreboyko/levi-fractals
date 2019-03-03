#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::setFractalType(bool toggle) {
    if (toggle) {
        FractalType type = FractalType::DEFAULT;

        auto sent = sender();
        if (sent == ui->radioDefault) {
            type = FractalType::DEFAULT;
        } else if (sent == ui->radioIsland) {
            type = FractalType::ISLAND;
        } else if (sent == ui->radioPi) {
            type = FractalType::PI_SHAPED;
        }

        ui->fractalView->setFractalType(type);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
