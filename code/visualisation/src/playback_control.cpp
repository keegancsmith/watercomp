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
    connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(setFrame(int)));
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
    connect(timer, SIGNAL(timeout()), this, SIGNAL(tick()));

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

