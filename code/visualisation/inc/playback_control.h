#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include <QWidget>

class QHBoxLayout;
class QPushButton;
class QSlider;
class QTimer;
class QVBoxLayout;

class PlaybackControl : public QWidget
{
    Q_OBJECT
    public:
        PlaybackControl(QWidget* parent=0);
        ~PlaybackControl();

        int tps();
        void tps(int value);

        int totalFrames();
        void totalFrames(int value);

    signals:
        void play();
        void pause();
        void tick();
        void setFrame(int value);

    private slots:
        void playClicked();

    private:
        QVBoxLayout* mainLayout;
        QHBoxLayout* lowerLayout;

        bool playing;
        QSlider* slider;
        QPushButton* playButton;

        QTimer* timer;
        int _tps;
        int _totalFrames;
};//PlaybackControl

#endif
