#ifndef MUSICUI_H
#define MUSICUI_H

#include "softwareuibase.h"
#include <QStringList>

namespace Ui {
class musicUi;
}
class QMediaPlayer;
class QPixmap;
class QString;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QAudioOutput;
#endif
class QListWidgetItem;

class musicUi : public softwareUiBase
{
    Q_OBJECT
public:
    explicit musicUi(QWidget *parent = nullptr);
    ~musicUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon();
    static softwareUiBase *getSingleton();

private slots:
    void onPlayPause();
    void onPrev();
    void onNext();
    void onProgressSliderMoved(int value);
    void onVolumeChanged(int value);
    void onPlaylistItemDoubleClicked(QListWidgetItem *item);
    void onSearchTextChanged(const QString &text);
    void onRepeatToggled();
    void onShuffleToggled();

    void on_exitButton_clicked();

private:
    QString resolveMusicDir() const;
    void loadPlaylist();
    void refreshPlaylistView(const QString &filter);
    void playIndex(int index);
    void playCurrent();
    void updatePlayButtonState();
    void updateTimeLabel(qint64 positionMs, qint64 durationMs);
    int pickNextIndex() const;
    QString formatTime(qint64 ms) const;

    Ui::musicUi *ui;
    QMediaPlayer *m_player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *m_audioOutput;
#endif
    QStringList m_allTracks;
    QStringList m_filteredTracks;
    QString m_musicDir;
    int m_currentIndex;
    bool m_isRepeat;
    bool m_isShuffle;
};

#endif // MUSICUI_H
