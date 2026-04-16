#include "videoui.h"
#include "ui_videoui.h"
#include "AppVideoPlayer.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <QSlider>
#include <QString>
#include <QToolButton>
#include <QVideoWidget>
#include <QMediaPlayer>

namespace {
constexpr const char * APP_BILIBILI = "bilibili";
constexpr const char * BILIBILI_ICON = ":/Resource/app/video/icon.png";
constexpr const char * ICON_CLOSE = ":/Resource/app/video/close-2.png";
constexpr const char * ICON_PLAY = ":/Resource/app/video/play.png";
constexpr const char * ICON_PAUSE = ":/Resource/app/video/pause.png";
constexpr const char * ICON_PREV = ":/Resource/app/video/play-previous.png";
constexpr const char * ICON_NEXT = ":/Resource/app/video/play-next.png";
constexpr const char * ICON_MUTE_ON = ":/Resource/app/video/sound-on.png";
constexpr const char * ICON_MUTE_OFF = ":/Resource/app/video/sound-off.png";
constexpr const char * ICON_FULLSCREEN = ":/Resource/app/video/fullscreen-enter.png";
constexpr const char * ICON_SETTINGS = ":/Resource/app/video/settings.png";
constexpr const char * ICON_PLAYLIST = ":/Resource/app/music/bofangliebiao.png";

const QIcon &closeIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_CLOSE));
    return icon;
}
const QIcon &playIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_PLAY));
    return icon;
}
const QIcon &pauseIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_PAUSE));
    return icon;
}
const QIcon &prevIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_PREV));
    return icon;
}
const QIcon &nextIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_NEXT));
    return icon;
}
const QIcon &muteOnIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_MUTE_ON));
    return icon;
}
const QIcon &muteOffIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_MUTE_OFF));
    return icon;
}
const QIcon &fullScreenIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_FULLSCREEN));
    return icon;
}
const QIcon &settingsIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_SETTINGS));
    return icon;
}
const QIcon &playlistIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_PLAYLIST));
    return icon;
}
}

videoUi::videoUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::videoUi)
    , m_player(new AppVideoPlayer(this))
    , m_videoWidget(nullptr)
    , m_currentIndex(-1)
    , m_isPlaying(false)
    , m_isMuted(false)
    , m_userSeeking(false)
{
    ui->setupUi(this);
    m_videoWidget = ui->videoWidget;
    if (m_videoWidget) {
        m_player->setVideoOutput(m_videoWidget);
        m_videoWidget->show();
    }
    m_player->setVolumePercent(ui->volumeSlider ? ui->volumeSlider->value() : 70);
    if (ui->progressSlider) {
        ui->progressSlider->setRange(0, 0);
        ui->progressSlider->setTracking(true);
    }
    setButtonIcon(ui->exitButton, closeIcon());
    setButtonIcon(ui->playlistButton, playlistIcon());
    setButtonIcon(ui->settingsButton, settingsIcon());
    setButtonIcon(ui->prevButton, prevIcon());
    setButtonIcon(ui->nextButton, nextIcon());
    setButtonIcon(ui->fullscreenButton, fullScreenIcon());
    syncPlayStateUi();
    syncMuteStateUi();
    if (ui->timeCurrentLabel) ui->timeCurrentLabel->setText("00:00");
    if (ui->timeTotalLabel) ui->timeTotalLabel->setText("00:00");

    connect(m_player, &AppMediaPlayerBase::durationChanged, this, [this](qint64 durationMs) {
        if (ui->progressSlider) ui->progressSlider->setRange(0, static_cast<int>(qMax<qint64>(0, durationMs)));
        if (ui->timeTotalLabel) ui->timeTotalLabel->setText(formatTime(static_cast<int>(durationMs / 1000)));
    });
    connect(m_player, &AppMediaPlayerBase::positionChanged, this, [this](qint64 positionMs) {
        if (ui->progressSlider && !m_userSeeking) ui->progressSlider->setValue(static_cast<int>(positionMs));
        if (ui->timeCurrentLabel) ui->timeCurrentLabel->setText(formatTime(static_cast<int>(positionMs / 1000)));
    });
    connect(ui->progressSlider, &QSlider::sliderPressed, this, [this]() {
        m_userSeeking = true;
    });
    connect(ui->progressSlider, &QSlider::sliderReleased, this, [this]() {
        m_userSeeking = false;
        if (!ui->progressSlider) return;
        m_player->setPositionMs(ui->progressSlider->value());
    });
    connect(ui->progressSlider, &QSlider::sliderMoved, this, [this](int value) {
        if (ui->timeCurrentLabel) ui->timeCurrentLabel->setText(formatTime(value / 1000));
    });
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        m_player->setVolumePercent(value);
        const bool nowMuted = value <= 0;
        if (m_player->playerHandle()) m_player->playerHandle()->setMuted(nowMuted);
        m_isMuted = nowMuted;
        syncMuteStateUi();
    });
}

videoUi::~videoUi()
{
    delete ui;
}

const QString &videoUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_BILIBILI);
    return softName;
}

const QPixmap &videoUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(BILIBILI_ICON));
    return softIcon;
}

softwareUiBase *videoUi::getSingleton()
{
    static auto videoApp = new videoUi();
    return videoApp;
}

void videoUi::on_exitButton_clicked()
{
    close();
    emit exit();
}

void videoUi::on_playPauseButton_clicked()
{
    if (m_currentIndex < 0 && !m_playlist.isEmpty()) {
        playAt(0);
        return;
    }
    m_isPlaying = !m_isPlaying;
    if (m_isPlaying) {
        m_player->play();
    } else {
        m_player->pause();
    }
    syncPlayStateUi();
}

void videoUi::on_muteButton_clicked()
{
    m_isMuted = !m_isMuted;
    if (m_player->playerHandle()) {
        m_player->playerHandle()->setMuted(m_isMuted);
    }
    if (ui->volumeSlider && !m_isMuted && ui->volumeSlider->value() == 0) {
        ui->volumeSlider->setValue(20);
    }
    syncMuteStateUi();
}

void videoUi::on_prevButton_clicked()
{
    if (m_playlist.isEmpty()) return;
    int index = m_currentIndex - 1;
    if (index < 0) index = m_playlist.size() - 1;
    playAt(index);
}

void videoUi::on_nextButton_clicked()
{
    if (m_playlist.isEmpty()) return;
    int index = m_currentIndex + 1;
    if (index >= m_playlist.size()) index = 0;
    playAt(index);
}

void videoUi::on_settingsButton_clicked()
{
    const QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("选择视频"),
        QString(),
        tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv);;所有文件 (*.*)"));
    if (files.isEmpty()) return;
    for (const auto &file : files) {
        if (m_playlist.contains(file)) continue;
        m_playlist.append(file);
        if (ui->playlistListWidget) ui->playlistListWidget->addItem(QFileInfo(file).fileName());
    }
    if (m_currentIndex < 0 && !m_playlist.isEmpty()) {
        playAt(0);
    }
}

void videoUi::on_playlistListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item || !ui->playlistListWidget) return;
    const int index = ui->playlistListWidget->row(item);
    playAt(index);
}

void videoUi::setButtonIcon(QToolButton *button, const QIcon &icon)
{
    if (!button) return;
    button->setIcon(icon);
    button->setIconSize(button->size());
}

void videoUi::syncPlayStateUi()
{
    setButtonIcon(ui->playPauseButton, m_isPlaying ? pauseIcon() : playIcon());
}

void videoUi::syncMuteStateUi()
{
    setButtonIcon(ui->muteButton, m_isMuted ? muteOffIcon() : muteOnIcon());
}

void videoUi::playAt(int index)
{
    if (index < 0 || index >= m_playlist.size()) return;
    m_currentIndex = index;
    m_player->setSource(m_playlist[index]);
    m_player->play();
    m_isPlaying = true;
    syncPlayStateUi();
    if (ui->playlistListWidget) ui->playlistListWidget->setCurrentRow(index);
}

QString videoUi::formatTime(int seconds) const
{
    if (seconds < 0) seconds = 0;
    const int minute = seconds / 60;
    const int sec = seconds % 60;
    return QString("%1:%2").arg(minute, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
}
