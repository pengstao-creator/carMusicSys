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
#include <QSlider>
#include <QUrl>
#include <QtGlobal>
#include <QFileDialog>
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
constexpr const char * ICON_MODE_SEQ = ":/Resource/app/music/shunxubofang.png";  // 顺序播放图标
constexpr const char * ICON_MODE_LIST_LOOP = ":/Resource/app/music/liebiaoxunhuan.png";  // 列表循环图标
constexpr const char * ICON_MODE_SINGLE_LOOP = ":/Resource/app/music/danquxunhuan.png";  // 单曲循环图标
constexpr const char * ICON_MODE_RANDOM = ":/Resource/app/music/suijibofang.png";  // 随机播放图标
}


musicUi::musicUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::musicUi)
    , player(new QMediaPlayer(this))
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    , audioOutput(new QAudioOutput(this))
#endif
    , currentIndex(-1)
    , modeIndex(0)
    , userSeeking(false)
{
    // 预渲染播放模式图标，避免每次切换都重复从资源路径加载。
    playMode.append({PlayMode::Sequential, QPixmap(QString::fromUtf8(ICON_MODE_SEQ))});
    playMode.append({PlayMode::ListLoop, QPixmap(QString::fromUtf8(ICON_MODE_LIST_LOOP))});
    playMode.append({PlayMode::SingleLoop, QPixmap(QString::fromUtf8(ICON_MODE_SINGLE_LOOP))});
    playMode.append({PlayMode::Random, QPixmap(QString::fromUtf8(ICON_MODE_RANDOM))});
    initSet();
}

void musicUi::initSet()
{
    ui->setupUi(this);  // 初始化UI

    setBackground(QString::fromUtf8(BACKGROUND_ICON));

    //初始化音乐播放器
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    audioOutput->setMuted(true);
    audioOutput->setVolume(0.0);
    player->setAudioOutput(audioOutput);
#endif

    if (ui->switchPlaybackType && !playMode.isEmpty()) {
        // 初始化按钮图标为当前模式（默认顺序播放）。
        ui->switchPlaybackType->setIcon(QIcon(playMode[modeIndex].second));
        ui->switchPlaybackType->setIconSize(QSize(24, 24));
    }
}

void musicUi::setBackground(const QString &iconpath)
{
    QString backgroundPath = iconpath.trimmed();
    auto backgroundPixmap = QPixmap(backgroundPath);
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
}


void musicUi::on_playPauseButton_clicked()
{
    //暂停
}


void musicUi::on_nextButton_clicked()
{
    //切换下一首
}


void musicUi::on_playlistButton_clicked()
{
    //选择播放路径,然后在playlistList中显示
    QStringList musicFiles = QFileDialog::getOpenFileNames(this,tr("选择歌曲"),"./",tr("文件类型(*.mp3,*.ogg)"));
    QFileInfo filename;//用于提取文件名
    QStringList musicNames;//用本次选择于添加到窗口容器进行显示
    for(auto filePath : musicFiles)
    {
        filename = filePath;
        //添加去重
        if(musicPaths.find(filePath) == musicPaths.end())
        {
//            musicPaths.insert(filePath,musicInfos.size());
            //需要提取该文件的各种信息

//            musicInfos.append(filePath);
            musicNames.append(filename.baseName());
        }
    }

    if(ui->playlistList)
    {
        ui->playlistList->addItems(musicNames);
    }
}

