#include "PlaybackControl.h"

#include <QHBoxLayout>
#include <QLabel>
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

    QHBoxLayout* lowerLayout = new QHBoxLayout();

    speedSlider = new QSlider(this);
    speedSlider->setRange(1, 20);
    speedSlider->setOrientation(Qt::Horizontal);
    connect(speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setTps(int)));
    lowerLayout->addWidget(speedSlider);

    speedLabel = new QLabel("0", this);
    lowerLayout->addWidget(speedLabel);

    mainLayout->addLayout(lowerLayout);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    _frame = 0;
    playing = true;
    playClicked(); //pause
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
    if (value < 0) return;
    _tps = value;
    if (value == 0)
    {
        playButton->setText(tr("Play"));
        timer->stop();
    }//if
    else
        timer->setInterval(1000 / _tps);
    speedSlider->setValue(_tps);
    speedLabel->setNum((int)_tps);
}//setTps

int PlaybackControl::totalFrames()
{
    return _totalFrames;
}//totalFrames

void PlaybackControl::setTotalFrames(int value)
{
    if (value < 0) value = 0;
    _totalFrames = value;
    slider->setMaximum(_totalFrames-1);
}//setTotalFrames

int PlaybackControl::frame()
{
    return _frame;
}//frame

void PlaybackControl::setFrame(int value)
{
    if (_frame == value) return;
    if (value < 0) value = 0;
    if (value >= _totalFrames) value = _totalFrames - 1;
    _frame = value;
    slider->setValue(_frame);
}//setFrame


void PlaybackControl::playClicked()
{
    playing = !playing;
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
}//playClicked

void PlaybackControl::timerTick()
{
    slider->setValue(_frame + 1);
    // emit tick();

    if (_frame == _totalFrames)
    {
        playing = true;
        playClicked();
        return;
    }//if
}//timerTick

