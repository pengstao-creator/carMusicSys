#include "musicui.h"
#include "ui_musicui.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QRandomGenerator>
#include <QSlider>
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#else
// Qt5不需要单独包含QAudioOutput
#endif
musicUi::musicUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::musicUi)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , m_audioOutput(nullptr)
#endif
    , m_currentIndex(-1)
    , m_isRepeat(false)
    , m_isShuffle(false)
{
    ui->setupUi(this);
    m_musicDir = resolveMusicDir();
    m_player = new QMediaPlayer(this);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(ui->volumeSlider->value() / 100.0);
#else
    m_player->setVolume(ui->volumeSlider->value());
#endif

    connect(ui->playButton, &QPushButton::clicked, this, &musicUi::onPlayPause);
    connect(ui->prevButton, &QPushButton::clicked, this, &musicUi::onPrev);
    connect(ui->nextButton, &QPushButton::clicked, this, &musicUi::onNext);
    connect(ui->repeatButton, &QPushButton::clicked, this, &musicUi::onRepeatToggled);
    connect(ui->shuffleButton, &QPushButton::clicked, this, &musicUi::onShuffleToggled);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &musicUi::onVolumeChanged);
    connect(ui->progressBar, &QSlider::sliderMoved, this, &musicUi::onProgressSliderMoved);
    connect(ui->playlistList, &QListWidget::itemDoubleClicked, this, &musicUi::onPlaylistItemDoubleClicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &musicUi::onSearchTextChanged);

    connect(m_player, &QMediaPlayer::positionChanged, this, [this](qint64 pos) {
        if (!ui->progressBar->isSliderDown()) {
            ui->progressBar->setValue(static_cast<int>(pos));
        }
        updateTimeLabel(pos, m_player->duration());
    });
    connect(m_player, &QMediaPlayer::durationChanged, this, [this](qint64 dur) {
        ui->progressBar->setRange(0, static_cast<int>(dur));
        updateTimeLabel(m_player->position(), dur);
    });
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState) {
        updatePlayButtonState();
    });
#else
    connect(m_player, QOverload<QMediaPlayer::State>::of(&QMediaPlayer::stateChanged), this, [this](QMediaPlayer::State) {
        updatePlayButtonState();
    });
#endif
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            if (m_isRepeat) {
                playCurrent();
            } else {
                onNext();
            }
        }
    });

    loadPlaylist();
    updatePlayButtonState();
    updateTimeLabel(0, 0);
}

musicUi::~musicUi()
{
    delete ui;
}

QString musicUi::resolveMusicDir() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 8; ++i) {
        if (dir.exists("Music")) {
            return QDir::cleanPath(dir.absoluteFilePath("Music"));
        }
        if (!dir.cdUp()) {
            break;
        }
    }
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/Music");
}

void musicUi::loadPlaylist()
{
    const QDir dir(m_musicDir);
    const QStringList filters = {"*.mp3", "*.wav", "*.flac", "*.aac", "*.ogg", "*.m4a", "*.wma"};
    const QFileInfoList entries = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    m_allTracks.clear();
    for (const QFileInfo &fileInfo : entries) {
        m_allTracks << fileInfo.absoluteFilePath();
    }
    refreshPlaylistView(ui->searchLineEdit->text());
    if (!m_filteredTracks.isEmpty()) {
        m_currentIndex = 0;
        ui->playlistList->setCurrentRow(0);
        playCurrent();
    }
}

void musicUi::refreshPlaylistView(const QString &filter)
{
    const QString key = filter.trimmed();
    m_filteredTracks.clear();
    ui->playlistList->clear();
    for (const QString &path : m_allTracks) {
        const QString name = QFileInfo(path).baseName();
        if (key.isEmpty() || name.contains(key, Qt::CaseInsensitive)) {
            m_filteredTracks << path;
            ui->playlistList->addItem(name);
        }
    }
    if (m_filteredTracks.isEmpty()) {
        m_currentIndex = -1;
        ui->songTitle->setText("未找到歌曲");
        ui->artistName->setText(m_musicDir);
        ui->albumName->setText(QString());
        return;
    }
    if (m_currentIndex < 0 || m_currentIndex >= m_filteredTracks.size()) {
        m_currentIndex = 0;
    }
    ui->playlistList->setCurrentRow(m_currentIndex);
}

void musicUi::playIndex(int index)
{
    if (index < 0 || index >= m_filteredTracks.size()) {
        return;
    }
    m_currentIndex = index;
    ui->playlistList->setCurrentRow(index);
    playCurrent();
}

void musicUi::playCurrent()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_filteredTracks.size()) {
        return;
    }
    const QString path = m_filteredTracks[m_currentIndex];
    const QUrl url = QUrl::fromLocalFile(path);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_player->setSource(url);
#else
    m_player->setMedia(url);
#endif
    ui->songTitle->setText(QFileInfo(path).baseName());
    ui->artistName->setText("本地音乐");
    ui->albumName->setText(m_musicDir);
    m_player->play();
}

void musicUi::updatePlayButtonState()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const bool isPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
#else
    const bool isPlaying = m_player->state() == QMediaPlayer::PlayingState;
#endif
    ui->playButton->setText(isPlaying ? "⏸" : "▶");
}

void musicUi::updateTimeLabel(qint64 positionMs, qint64 durationMs)
{
    ui->currentTime->setText(formatTime(positionMs));
    ui->totalTime->setText(formatTime(durationMs));
}

int musicUi::pickNextIndex() const
{
    if (m_filteredTracks.isEmpty()) {
        return -1;
    }
    if (m_isShuffle && m_filteredTracks.size() > 1) {
        int next = m_currentIndex;
        while (next == m_currentIndex) {
            next = QRandomGenerator::global()->bounded(m_filteredTracks.size());
        }
        return next;
    }
    return (m_currentIndex + 1) % m_filteredTracks.size();
}

QString musicUi::formatTime(qint64 ms) const
{
    if (ms <= 0) {
        return "0:00";
    }
    const qint64 sec = ms / 1000;
    const qint64 m = sec / 60;
    const qint64 s = sec % 60;
    return QString("%1:%2").arg(m).arg(s, 2, 10, QChar('0'));
}

void musicUi::onPlayPause()
{
    if (m_currentIndex < 0 && !m_filteredTracks.isEmpty()) {
        playIndex(0);
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
#else
    if (m_player->state() == QMediaPlayer::PlayingState) {
#endif
        m_player->pause();
    } else {
        m_player->play();
    }
}

void musicUi::onPrev()
{
    if (m_filteredTracks.isEmpty()) {
        return;
    }
    int prev = m_currentIndex - 1;
    if (prev < 0) {
        prev = m_filteredTracks.size() - 1;
    }
    playIndex(prev);
}

void musicUi::onNext()
{
    const int next = pickNextIndex();
    if (next < 0) {
        return;
    }
    playIndex(next);
}

void musicUi::onProgressSliderMoved(int value)
{
    m_player->setPosition(value);
}

void musicUi::onVolumeChanged(int value)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (m_audioOutput) {
        m_audioOutput->setVolume(value / 100.0);
    }
#else
    m_player->setVolume(value);
#endif
}

void musicUi::onPlaylistItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    const int row = ui->playlistList->row(item);
    playIndex(row);
}

void musicUi::onSearchTextChanged(const QString &text)
{
    const QString currentPath = (m_currentIndex >= 0 && m_currentIndex < m_filteredTracks.size()) ? m_filteredTracks[m_currentIndex] : QString();
    refreshPlaylistView(text);
    if (!currentPath.isEmpty()) {
        const int idx = m_filteredTracks.indexOf(currentPath);
        if (idx >= 0) {
            m_currentIndex = idx;
            ui->playlistList->setCurrentRow(idx);
        }
    }
}

void musicUi::onRepeatToggled()
{
    m_isRepeat = !m_isRepeat;
    ui->repeatButton->setStyleSheet(QString("font-size: 20px; color: %1; background-color: transparent;").arg(m_isRepeat ? "#1DB954" : "#999999"));
}

void musicUi::onShuffleToggled()
{
    m_isShuffle = !m_isShuffle;
    ui->shuffleButton->setStyleSheet(QString("font-size: 20px; color: %1; background-color: transparent;").arg(m_isShuffle ? "#1DB954" : "#999999"));
}

void musicUi::on_exitButton_clicked()
{
    close();
    emit exit();
}

