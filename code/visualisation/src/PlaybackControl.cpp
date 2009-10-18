#include "PlaybackControl.h"

#include <cstdio>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

PlaybackControl::PlaybackControl(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignVCenter);

    slider = new QSlider(this);
    slider->setMinimum(0);
    slider->setOrientation(Qt::Horizontal);
    slider->setTracking(true);
    connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(frameChange(int)));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    mainLayout->addWidget(slider);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignHCenter);

    playButton = new QPushButton(tr("NULL"), this);
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    buttonLayout->addWidget(playButton);
    mainLayout->addLayout(buttonLayout);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    _frame = 0;
    setPlay(false);
    setTotalFrames(0);
}//constructor

PlaybackControl::~PlaybackControl()
{
    delete timer;
}//destructor


int PlaybackControl::tps()
{
    return _tps;
}//tps

void PlaybackControl::setTps(int value)
{
    if (value < 1) value = 1;
    _tps = value;
    if (value == 0)
    {
        playButton->setText(tr("Play"));
        timer->stop();
    }//if
    else
        timer->setInterval(1000 / _tps);
}//setTps

int PlaybackControl::totalFrames()
{
    return _totalFrames;
}//totalFrames

void PlaybackControl::setTotalFrames(int value)
{
    if (value < 0) value = 0;
    _totalFrames = value;
    slider->setRange(0, _totalFrames - 1);
}//setTotalFrames

int PlaybackControl::frame()
{
    return _frame;
}//frame

void PlaybackControl::setFrame(int value)
{
    if (_frame == value) return;
    if (value < 0) value = 0;
    if (value >= _totalFrames)
    {
        value = _totalFrames - 1;
        setPlay(false);
    }//if
    _frame = value;
    slider->setValue(_frame);
}//setFrame

void PlaybackControl::setPlay(bool value)
{
    playing = value;
    if (playing)
    {
        playButton->setText(tr("Pause"));
        timer->start();
        emit play();
    }//if
    else
    {
        playButton->setText(tr("Play"));
        timer->stop();
        emit pause();
    }//else
}//setPlay


void PlaybackControl::playClicked()
{
    setPlay(!playing);
}//playClicked

void PlaybackControl::timerTick()
{
    setFrame(_frame + 1);
}//timerTick

