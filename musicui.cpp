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
#include "AppMusicPlayer.h"
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
constexpr const char * ICON_COVERART = ":/Resource/app/music/CD.png";
}


musicUi::musicUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::musicUi)
    , currentIndex(-1)
    , modeIndex(0)
    , userSeeking(false)
    , player(new AppMusicPlayer(this))
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
    if (ui->switchPlaybackType && !playMode.isEmpty()) {
        // 初始化按钮图标为当前模式（默认顺序播放）。
        ui->switchPlaybackType->setIcon(QIcon(playMode[modeIndex].second));
        ui->switchPlaybackType->setIconSize(ui->switchPlaybackType->size());
    }
    //设置默认音乐封面
    dcoverArt = QPixmap(ICON_COVERART);

    //绑定音乐信息加载信号
    connect(player,&AppMusicPlayer::laodOk,this,&musicUi::setMusicUi);

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

    if(musicinfo.Duration.isEmpty())
    {
        ui->timeTotalLabel->setText("00.00");
    }
    else
    {
        ui->timeTotalLabel->setText(musicinfo.Duration);
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
    QStringList musicFiles = QFileDialog::getOpenFileNames(this,tr("选择歌曲"),"D:/code/carMusicSys/Music",tr("文件类型(*.*)"));
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
                OncePaths.insert(filename.baseName(),musicFiles.size());
                musicFilePaths.append(formatFilePath);
                musicNames.append(filename.baseName());
            }
        }
    }

    if(ui->playlistList && !musicNames.isEmpty())
    {
        ui->playlistList->addItems(musicNames);
        qDebug() << musicFilePaths;
        player->setSource(musicFilePaths[0]);
    }
}

