#ifndef APPMEDIAPLAYERBASE_H
#define APPMEDIAPLAYERBASE_H

#include <QObject>

class QMediaPlayer;
class QUrl;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QAudioOutput;
#endif

class AppMediaPlayerBase : public QObject
{
    Q_OBJECT
public:
    explicit AppMediaPlayerBase(QObject *parent = nullptr);
    ~AppMediaPlayerBase() override;

    QMediaPlayer *playerHandle() const;

    void setSource(const QString &url);
    void play();
    void pause();
    void stop();

    void setVolumePercent(int percent);
    int volumePercent() const;

    void setPositionMs(qint64 positionMs);
    qint64 positionMs() const;
    qint64 durationMs() const;

signals:
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void playbackStateChanged();

protected:
    QMediaPlayer *m_player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *m_audioOutput;
#endif
};

#endif // APPMEDIAPLAYERBASE_H
