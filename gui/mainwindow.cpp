#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QKeyEvent>

#include "gui/config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // setup ui elements
    ui->setupUi(this);

    // connect ui elements to slots
    connect(this->ui->sliderAnimationSpeed, SIGNAL(valueChanged(int)), this, SLOT(setAnimationSpeed(int)));

    connect(this->ui->spinBox_radius, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
    connect(this->ui->checkLoop, SIGNAL(clicked(bool)), this, SLOT(setLoop(bool)));
    connect(this->ui->onlyShowerFront, SIGNAL(clicked(bool)), this, SLOT(setOnlyShowerFront(bool)));
    connect(this->ui->lookatShowerFront, SIGNAL(clicked(bool)), this, SLOT(setLookatShowerFront(bool)));

    connect(this->ui->enable_detectors, SIGNAL(clicked(bool)), this, SLOT(toggleDetector(bool)));

    connect(this->ui->checkBox_em, SIGNAL(clicked(bool)), this, SLOT(setComponentem(bool)));
    connect(this->ui->checkBox_mu, SIGNAL(clicked(bool)), this, SLOT(setComponentmu(bool)));
    connect(this->ui->checkBox_hd, SIGNAL(clicked(bool)), this, SLOT(setComponenthd(bool)));

    connect(this->ui->playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(this->ui->sliderTime, SIGNAL(valueChanged(int)), this, SLOT(setTime(int)));

    connect(this->ui->spinBox_levels, SIGNAL(valueChanged(int)),this, SLOT(setLevels(int)));

    connect(this->ui->spinBox_distance, SIGNAL(valueChanged(double)), this, SLOT(setDistance(double)));

    connect(this->ui->penrose_terrell, SIGNAL(clicked(bool)), this, SLOT(togglePTEffect(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAnimationSpeed(int value)
{
    Config::animationSpeed = float(value) / 2.0f;
    QString title = QString("Animation: ") + QString::number(Config::animationSpeed, 'x', 1) + "x";
    this->ui->groupBox_animation->setTitle(title);
}

void MainWindow::setLoop(bool value)
{
    Config::loop=value;
}

void MainWindow::setComponentem(bool value)
{
    Config::emComponent=value;
}
void MainWindow::setComponentmu(bool value)
{
    Config::muComponent=value;
}
void MainWindow::setComponenthd(bool value)
{
    Config::hdComponent=value;
}

void MainWindow::toggleDetector(bool value)
{
    Config::enableDetectors=value;
}

void MainWindow::togglePTEffect(bool value)
{
    Config::pteffect=value;
}

void MainWindow::setOnlyShowerFront(bool value)
{
    Config::onlyShowerFront=value;

    Config::changedAirshower=true;
}

void MainWindow::setLookatShowerFront(bool value)
{
    Config::lookatShowerFront=value;
}

void MainWindow::setRadius(double value)
{
    Config::radius=value;
}


void MainWindow::playPause()
{
    if(Config::isPlaying)
        Config::isPlaying=false;
    else
        Config::isPlaying=true;
}

void MainWindow::setTime(int value)
{
    if(Config::isPlaying)
        Config::isPlaying=false;

    float floatTime= value/100.f;
    Config::time=floatTime;
    Config::pauseTime=floatTime;

    QString title = QString("Elapsed time: ") + QString::number(Config::time*Config::maxTime*100000, 'x', 4) + "E-5 sec";
    this->ui->groupBox_time->setTitle(title);

}


void MainWindow::setLevels(int value)
{
    Config::crownLevels=value;
    Config::crownLevelsChanged=true;
}

void MainWindow::setDistance(double value)
{
    Config::detectorDistance=value;
    Config::distanceChanged=true;
}


/* Qt mouse and keyboard events */

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // enable control via keyboard
    switch (event->key()) {
    case Qt::Key_F:
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
        break;
    case Qt::Key_Escape:
    case Qt::Key_Q:
        close();
        break;
    }
}


