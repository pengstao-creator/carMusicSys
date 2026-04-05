#include "musicui.h"
#include "ui_musicui.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QIcon>
#include <QPixmap>
#include <QRandomGenerator>
#include <QSize>
#include <QUrl>
#include <QtGlobal>
#include <QFileDialog>
#include <QRandomGenerator>
#include "AppMusicPlayer.h"
#include "clickableslider.h"
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#else
// Qt5不需要单独包含QAudioOutput
#endif

namespace {
constexpr const char * APP_QQMUSIC = "QQMusic";  // 应用名称
constexpr const char * QQMUSIC_ICON = ":/Resource/app/music/icon.png";  // 应用图标路径
constexpr const char * BACKGROUND_ICON = ":/Resource/app/common/app_icon.jpg";  // 随机播放图标
constexpr const char * ICON_PLAY = ":/Resource/app/music/bofangzhong.png";  // 播放图标
constexpr const char * ICON_PAUSE = ":/Resource/app/music/zanting.png";  // 暂停图标
constexpr const char * ICON_MODE_SEQ = ":/Resource/app/music/shunxubofang.png";  // 播放列表图标
constexpr const char * ICON_MODE_LIST_LOOP = ":/Resource/app/music/liebiaoxunhuan.png";  // 列表循环图标
constexpr const char * ICON_MODE_SINGLE_LOOP = ":/Resource/app/music/danquxunhuan.png";  // 单曲循环图标
constexpr const char * ICON_MODE_RANDOM = ":/Resource/app/music/suijibofang.png";  // 随机播放图标
constexpr const char * ICON_COVERART = ":/Resource/app/music/CD.png";
constexpr const char * ICON_PREV = ":/Resource/app/music/shangyiqu.png";
constexpr const char * ICON_NEXT = ":/Resource/app/music/xiayiqu.png";
constexpr const char * ICON_PLAYLIST = ":/Resource/app/music/tianjiadao.png";
constexpr const char * ICON_VOLUME_ON = ":/Resource/app/music/yinliang.png";
constexpr const char * ICON_VOLUME_OFF = ":/Resource/app/music/yinleguan.png";

const QString &backgroundIconPath()
{
    static const QString path = QString::fromUtf8(BACKGROUND_ICON);
    return path;
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
const QIcon &playlistIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_PLAYLIST));
    return icon;
}
const QIcon &volumeOnIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_VOLUME_ON));
    return icon;
}
const QIcon &volumeOffIcon()
{
    static const QIcon icon(QString::fromUtf8(ICON_VOLUME_OFF));
    return icon;
}
const QPixmap &modeListLoopPixmap()
{
    static const QPixmap pixmap(QString::fromUtf8(ICON_MODE_LIST_LOOP));
    return pixmap;
}
const QPixmap &modeSingleLoopPixmap()
{
    static const QPixmap pixmap(QString::fromUtf8(ICON_MODE_SINGLE_LOOP));
    return pixmap;
}
const QPixmap &modeRandomPixmap()
{
    static const QPixmap pixmap(QString::fromUtf8(ICON_MODE_RANDOM));
    return pixmap;
}
const QPixmap &coverArtPixmap()
{
    static const QPixmap pixmap(QString::fromUtf8(ICON_COVERART));
    return pixmap;
}
}
musicUi::musicUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::musicUi)
    , currentIndex(-1)
    , modeIndex(0)
    , isPlay(false)
    , isMuted(false)
    , userSeeking(false)
    , player(new AppMusicPlayer(this))
{
    initSet();
}

void musicUi::initSet()
{
    playMode.append({PlayMode::ListLoop, modeListLoopPixmap()});
    playMode.append({PlayMode::SingleLoop, modeSingleLoopPixmap()});
    playMode.append({PlayMode::Random, modeRandomPixmap()});

    ui->setupUi(this);  // 初始化UI
    setBackground(backgroundIconPath());

    // 初始化按钮图标为当前模式（默认顺序播放）。
    setButtonIcon(ui->switchPlaybackType,QIcon(playMode[modeIndex].second));
    //设置暂停图标
    setButtonIcon(ui->playPauseButton,pauseIcon());
    //设置上一首图标
    setButtonIcon(ui->prevButton,prevIcon());
    //设置下一首图标
    setButtonIcon(ui->nextButton,nextIcon());
    //设置添加列表图标
    setButtonIcon(ui->playlistButton,playlistIcon());
    //设置音量键图标
    setButtonIcon(ui->volumeButton,volumeOnIcon());
    //设置默认音乐封面
    dcoverArt = coverArtPixmap();
    //绑定音乐信息加载信号
    connect(player,&AppMusicPlayer::laodOk,this,&musicUi::setMusicUi);
    if (ui && ui->progressSlider) {
        //进度条最大值和最小值，等价setMinimum(0) 和 setMaximum(0)
        ui->progressSlider->setRange(0, 0);
        ui->progressSlider->setTracking(true);
    }
    // 使用播放器时长信号驱动进度条范围，避免手动计时器累计误差。
    connect(player, &AppMediaPlayerBase::durationChanged, this, [this](qint64 durationMs) {
        if (!ui || !ui->progressSlider || !ui->timeTotalLabel) {
            return;
        }
        const int sliderMax = static_cast<int>(qMax<qint64>(0, durationMs));
        ui->progressSlider->setRange(0, sliderMax);
        ui->timeTotalLabel->setText(toMMSS(static_cast<int>(durationMs / 1000)));
    });
    // 使用播放器播放位置信号刷新当前时间和进度条；用户拖动时不覆盖滑块位置。
    connect(player, &AppMediaPlayerBase::positionChanged, this, [this](qint64 positionMs) {
        if (!ui || !ui->progressSlider || !ui->timeCurrentLabel) {
            return;
        }
        if (!ui->progressSlider->isSliderDown()) {
            ui->progressSlider->setValue(static_cast<int>(qMax<qint64>(0, positionMs)));
            ui->timeCurrentLabel->setText(toMMSS(static_cast<int>(positionMs / 1000)));
        }
    });
    connect(ui->progressSlider, &ClickableSlider::sliderPressed, this, [this]() {
        userSeeking = true;
    });
    connect(ui->progressSlider, &ClickableSlider::sliderReleased, this, [this]() {
        userSeeking = false;
        on_progressSlider_actionTriggered(QAbstractSlider::SliderMove);
    });
    connect(ui->progressSlider, &ClickableSlider::sliderMoved, this, [this](int value) {
        if (!ui || !ui->timeCurrentLabel) {
            return;
        }
        ui->timeCurrentLabel->setText(toMMSS(value / 1000));
    });
    //绑定音乐切换信号,实现自动切换逻辑
    connect(player->playerHandle(),&QMediaPlayer::mediaStatusChanged,this,[this](QMediaPlayer::MediaStatus status){

        if(status != QMediaPlayer::MediaStatus::EndOfMedia)return;
        //列表循环
        if(playMode[modeIndex].first == PlayMode::ListLoop)
        {
            on_nextButton_clicked();
        }//单曲循环
        else if(playMode[modeIndex].first == PlayMode::SingleLoop)
        {
            if(player)
            {
                player->setPositionMs(0);
                player->play();
            }
        }//随机播放
        else
        {
            auto randomInRange = QRandomGenerator::global()->bounded(0,musicFilePaths.size()-1);
            ui->playlistList->setCurrentRow(randomInRange);
        }
    });
}

void musicUi::setBackground(const QString &iconpath)
{
    QString backgroundPath = iconpath.trimmed();
    backgroundPixmap = QPixmap(backgroundPath);
    if (backgroundLabel) {
        if (!backgroundPixmap.isNull()) {
            const QPixmap scaled = backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            backgroundLabel->setPixmap(scaled);
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, QBrush(scaled));
            setPalette(pal);
            setAutoFillBackground(true);
        } else {
            backgroundLabel->setPixmap(QPixmap());
            setAutoFillBackground(false);
        }
    }
}

void musicUi::setMusicUi()
{
    //切换音乐加载信号完成之后设置该音乐的UI
    AppMusicPlayer::MusicInfo musicinfo = player->getMusicInfo();
    if(!musicinfo.CoverArt.isNull())
    {
        ui->albumArtLabel->setPixmap(musicinfo.CoverArt);
    }
    else
    {
        ui->albumArtLabel->setPixmap(dcoverArt);
        ui->albumArtLabel->setScaledContents(true);
    }
    if(musicinfo.Artist.isEmpty())
    {
        ui->artistLabel->setText(tr("未知歌手"));
    }
    else
    {
        ui->artistLabel->setText(musicinfo.Artist);
    }

    if(musicinfo.Artist.isEmpty())
    {
        ui->trackTitleLabel->setText(tr("未知专辑"));
    }
    else
    {
        ui->trackTitleLabel->setText(musicinfo.AlbumTitle);
    }
    if(musicinfo.Title.isEmpty())
    {
        ui->titleLabel->setText(tr("未知歌名"));
    }
    else
    {
        ui->titleLabel->setText(musicinfo.Title);
    }
}

void musicUi::setMusic(const QString& name)
{
    //查询是否存在
    if(!OncePaths.isEmpty() && OncePaths.contains(name))
    {
        //返回索引
        int index = OncePaths[name];
        setMusic(index);
    }
    else
    {
        //歌曲不存在
        qDebug() << "歌曲不存在";
    }
}

void musicUi::setMusic(int index)
{
    if(index <musicFilePaths.size())
    {
        currentIndex = index;
        player->setSource(musicFilePaths[currentIndex]);
        if (ui && ui->progressSlider) {
            ui->progressSlider->setValue(0);
        }
        if (ui && ui->timeCurrentLabel) {
            ui->timeCurrentLabel->setText("00:00");
            ui->timeTotalLabel->setText("00.00");
        }
        isPlay = false;
        on_playPauseButton_clicked();
    }
}

void musicUi::setButtonIcon(QPushButton *but, const QIcon &icon)
{
    if(but)
    {
        but->setIcon(icon);
        but->setIconSize(but->size());
    }
}

QString musicUi::toMMSS(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    QString HHSS = QString("%1:%2")
                           .arg(minutes)
                           .arg(secs, 2, 10, QChar('0'));
    return HHSS;
}


musicUi::~musicUi()
{
    if (player) {
        player->stop();  // 停止播放
    }
    delete ui;  // 释放UI资源
}


const QString &musicUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_QQMUSIC);  // 静态变量，只初始化一次
    return softName;
}
const QPixmap &musicUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(QQMUSIC_ICON));  // 静态变量，只初始化一次
    return softIcon;
}
softwareUiBase *musicUi::getSingleton()
{
    static auto musicApp = new musicUi();  // 静态变量，只初始化一次
    return musicApp;
}


void musicUi::on_exitButton_clicked()
{
    close();
    emit exit();
}


void musicUi::on_switchPlaybackType_clicked()
{
    // 切换播放方式：顺序 -> 列表循环 -> 单曲循环 -> 随机 -> 顺序...
    if (playMode.isEmpty()) {
        return;
    }
    modeIndex = (modeIndex + 1) % playMode.size();
    if (ui->switchPlaybackType) {
        ui->switchPlaybackType->setIcon(QIcon(playMode[modeIndex].second));
        ui->switchPlaybackType->setIconSize(ui->switchPlaybackType->size());
    }
}


void musicUi::on_prevButton_clicked()
{
    //切换上一首
    currentIndex--;
    if(currentIndex < 0)
    {
        currentIndex = ui->playlistList->count() - 1;
    }
    if(ui->playlistList)
    {
        ui->playlistList->setCurrentRow(currentIndex);
    }
}


void musicUi::on_playPauseButton_clicked()
{
    isPlay = !isPlay;
    //暂停
    if(ui->playPauseButton)
    {
        if(!isPlay)
        {
            player->pause();
        }
        else
        {
            player->play();
        }
        setButtonIcon(ui->playPauseButton,isPlay ? playIcon() : pauseIcon());
    }

}


void musicUi::on_nextButton_clicked()
{
    //切换下一首
    currentIndex++;
    if(currentIndex >= ui->playlistList->count())
    {
        currentIndex = 0;
    }
    if(ui->playlistList)
    {
        ui->playlistList->setCurrentRow(currentIndex);
    }
}


void musicUi::on_playlistButton_clicked()
{
    //选择播放路径,然后在playlistList中显示
    QStringList musicFiles = QFileDialog::getOpenFileNames(this,tr("选择歌曲"),"D:/code/github/car-music-sys/Music",tr("文件类型(*.*)"));
    QFileInfo filename;//用于提取文件名
    QStringList musicNames;//用本次选择于添加到窗口容器进行显示
    QString formatFilePath;//将文件路劲格式化保存,使用/分割
    for(auto filePath : musicFiles)
    {
        filename = filePath;
        formatFilePath = filename.canonicalFilePath();
        if(filename.isFile())
        {
            //添加去重
            if(OncePaths.isEmpty() || OncePaths.find(filename.baseName()) == OncePaths.end())
            {
                OncePaths.insert(filename.baseName(),musicFilePaths.size());
                musicFilePaths.append(formatFilePath);
                musicNames.append(filename.baseName());
            }
        }
    }

    if(ui->playlistList && !musicNames.isEmpty())
    {
        ui->playlistList->addItems(musicNames);
    }
}
void musicUi::on_playlistList_currentTextChanged(const QString &currentText)
{
    setMusic(currentText);
}
void musicUi::on_volumeButton_clicked()
{
    isMuted = !isMuted;
    if (player && player->playerHandle()) {
        player->playerHandle()->setMuted(isMuted);
    }
    setButtonIcon(ui->volumeButton, isMuted ? volumeOffIcon() : volumeOnIcon());
}
void musicUi::on_volumeSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    if (!ui || !ui->volumeSlider || !player) {
        return;
    }
    const int volume = ui->volumeSlider->value();
    player->setVolumePercent(volume);
    const bool muteNow = (volume <= 0);
    if (player->playerHandle()) {
        player->playerHandle()->setMuted(muteNow);
    }
    isMuted = muteNow;
    setButtonIcon(ui->volumeButton, isMuted ? volumeOffIcon() : volumeOnIcon());
}


void musicUi::on_progressSlider_actionTriggered(int action)
{
    if (!ui || !ui->progressSlider || !player) {
        return;
    }
    if (action == QAbstractSlider::SliderNoAction) {
        return;
    }
    if (ui->progressSlider->isSliderDown() || userSeeking) {
        return;
    }
    const int targetMs = ui->progressSlider->sliderPosition();
    player->setPositionMs(targetMs);
    if (ui->timeCurrentLabel) {
        ui->timeCurrentLabel->setText(toMMSS(targetMs / 1000));
    }
}


