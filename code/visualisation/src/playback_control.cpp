#include "playback_control.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

PlaybackControl::PlaybackControl(QWidget* parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);

    slider = new QSlider(this);
    slider->setMinimum(0);
    slider->setOrientation(Qt::Horizontal);
    slider->setTracking(true);
    connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(frameChange(int)));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    mainLayout->addWidget(slider);

    lowerLayout = new QHBoxLayout();
    lowerLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->addLayout(lowerLayout);

    playing = true;
    playButton = new QPushButton(tr("NULL"), this);
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    lowerLayout->addWidget(playButton);

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    _frame = 0;
    tps(60);
    playClicked();
    totalFrames(0);
}//constructor

PlaybackControl::~PlaybackControl()
{
    delete timer;
}//destructor


int PlaybackControl::tps()
{
    return _tps;
}//tps

void PlaybackControl::tps(int value)
{
    if (value < 0) return;
    _tps = value;
    if (value == 0) timer->stop();
    else timer->setInterval(1000 / _tps);
}//tps

int PlaybackControl::totalFrames()
{
    return _totalFrames;
}//totalFrames

void PlaybackControl::totalFrames(int value)
{
    if (value < 0) value = 0;
    _totalFrames = value;
    slider->setMaximum(_totalFrames);
}//totalFrames

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

