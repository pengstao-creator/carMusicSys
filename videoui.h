#ifndef VIDEOUI_H
#define VIDEOUI_H

#include "softwareuibase.h"
#include <QStringList>
class QPixmap;
class QString;
class AppVideoPlayer;
class QVideoWidget;
class QToolButton;
class QListWidgetItem;

namespace Ui {
class videoUi;
}

class videoUi : public softwareUiBase
{
    Q_OBJECT

public:
    explicit videoUi(QWidget *parent = nullptr);
    ~videoUi();
    static const QString &getSoftname();
    static const QPixmap &getSofticon();
    static softwareUiBase *getSingleton();

private slots:
    void on_exitButton_clicked();
    void on_playPauseButton_clicked();
    void on_muteButton_clicked();
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_settingsButton_clicked();
    void on_playlistListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void setButtonIcon(QToolButton *button, const QIcon &icon);
    void syncPlayStateUi();
    void syncMuteStateUi();
    void playAt(int index);
    QString formatTime(int seconds) const;
    Ui::videoUi *ui;
    AppVideoPlayer *m_player;
    QVideoWidget *m_videoWidget;
    QStringList m_playlist;
    int m_currentIndex;
    bool m_isPlaying;
    bool m_isMuted;
    bool m_userSeeking;
};

#endif // VIDEOUI_H
