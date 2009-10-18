#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include <QWidget>

class QPushButton;
class QSlider;
class QTimer;

class PlaybackControl : public QWidget
{
    Q_OBJECT
    public:
        PlaybackControl(QWidget* parent=0);
        ~PlaybackControl();

        int tps();

        int frame();

        int totalFrames();
        void setTotalFrames(int value);

    signals:
        void play();
        void pause();
        void tick();
        void frameChange(int value);

    public slots:
        void setTps(int value);
        void setFrame(int value);
        void setPlay(bool value);

    private slots:
        void playClicked();
        void timerTick();

    private:
        bool playing;
        QSlider* slider;
        QPushButton* playButton;

        QTimer* timer;
        int _tps;
        int _frame;
        int _totalFrames;
};//PlaybackControl

#endif
