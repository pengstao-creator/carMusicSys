#include "AppMusicPlayer.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QMediaPlayer>
#include <QVariant>
#include "thirdparty/inc/taglib/attachedpictureframe.h"
#include "thirdparty/inc/taglib/audioproperties.h"
#include "thirdparty/inc/taglib/fileref.h"
#include "thirdparty/inc/taglib/id3v2tag.h"
#include "thirdparty/inc/taglib/mpegfile.h"
#include "thirdparty/inc/taglib/flacfile.h"
#include "thirdparty/inc/taglib/flacpicture.h"
#include "thirdparty/inc/taglib/mp4file.h"
#include "thirdparty/inc/taglib/mp4item.h"
#include "thirdparty/inc/taglib/mp4tag.h"
#include "thirdparty/inc/taglib/oggfile.h"
#include "thirdparty/inc/taglib/vorbisfile.h"
#include "thirdparty/inc/taglib/xiphcomment.h"
#include "thirdparty/inc/taglib/tag.h"
AppMusicPlayer::AppMusicPlayer(QObject *parent)
    : AppMediaPlayerBase(parent)
{
    //直接绑定metaDataChanged信号用来获取元数据
    connect(
    m_player,
    QOverload<>::of(&QMediaPlayer::metaDataChanged),/*消除重载歧义*/
    this,&AppMusicPlayer::getMetaData);


}

void AppMusicPlayer::getMetaData()
{
    cleanMusicInfo();
    //这是QT自带的元数据读取方式，有些格式不支持需要安装新的模块进行解决，
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMediaMetaData metaData = m_player->metaData();
//    musicInfo.Title = metaData.stringValue(QMediaMetaData::Title);
//    musicInfo.Artist = metaData.stringValue(QMediaMetaData::Artist);
//    musicInfo.AlbumTitle = metaData.stringValue(QMediaMetaData::AlbumTitle);
//    QVariant CoverArt = metaData(QMediaMetaData::CoverArtImage);
    musicInfo.filepath = m_player->source().toLocalFile();
#else
//    musicInfo.Title = m_player->metaData("Title").toString();
//    musicInfo.Artist = m_player->metaData("Artist").toString();
//    musicInfo.AlbumTitle = m_player->metaData("AlbumTitle").toString();
//    QVariant CoverArt = m_player->metaData("CoverArtImage");
    musicInfo.filepath = m_player->media().canonicalUrl().toLocalFile();
#endif
//    musicInfo.Duration = m_player->duration();
//    //将CoverArt转化为图片
//    if (CoverArt.isValid() && CoverArt.canConvert<QImage>()) {
//        QImage img = CoverArt.value<QImage>();
//        musicInfo.CoverArt = QPixmap::fromImage(img);
//    }
    //这里直接使用三方库TagLib来进行处理
    //- Windows 下走 wchar_t* （ toStdWString() ）传入 TagLib::FileRef
    //-非 Windows 下用 QFile::encodeName(...) 传本地编码
    const QString nativePath = QDir::toNativeSeparators(musicInfo.filepath);
#ifdef Q_OS_WIN
    const std::wstring widePath = nativePath.toStdWString();
    TagLib::FileRef f(widePath.c_str());
#else
    const QByteArray localPath = QFile::encodeName(nativePath);
    TagLib::FileRef f(localPath.constData());
#endif
    if(f.isNull() || !f.tag())
    {
        qWarning() << "TagLib 无法打开或解析文件:" << musicInfo.filepath.toUtf8().constData();
        cleanMusicInfo();
        return;
    }
    TagLib::Tag * tag = f.tag();
    musicInfo.Title = tagStringToQString(tag->title());
    musicInfo.Artist = tagStringToQString(tag->artist());
    musicInfo.AlbumTitle = tagStringToQString(tag->album());
    if (f.audioProperties()) {
        int seconds = f.audioProperties()->lengthInSeconds();
        int minutes = seconds / 60;
        int secs = seconds % 60;
        musicInfo.Duration = QString("%1:%2")
                                .arg(minutes)
                                .arg(secs, 2, 10, QChar('0'));

    }

    const QString suffix = QFileInfo(musicInfo.filepath).suffix().toLower();
    QImage coverImage;
    if (suffix == "mp3") {
        coverImage = getCoverFromMP3(musicInfo.filepath);
    } else if (suffix == "m4a" || suffix == "mp4") {
        coverImage = getCoverFromMP4(musicInfo.filepath);
    } else if (suffix == "ogg" || suffix == "oga" || suffix == "opus") {
        coverImage = getCoverFromOGG(musicInfo.filepath);
    }
    if (!coverImage.isNull()) {
        musicInfo.CoverArt = QPixmap::fromImage(coverImage);
    }
    


    qDebug() << "AlbumTitle:"<< musicInfo.AlbumTitle;
    qDebug() << "Artist:"<<musicInfo.Artist;
    qDebug() << "Title:"<<musicInfo.Title;
    qDebug() << "Duration:"<<musicInfo.Duration;

    emit laodOk();
}

void AppMusicPlayer::cleanMusicInfo()
{
    musicInfo.Title.clear();
    musicInfo.AlbumTitle.clear();
    musicInfo.Artist.clear();
    musicInfo.CoverArt = QPixmap();
    musicInfo.Duration.clear();
    musicInfo.filepath.clear();
}

QString AppMusicPlayer::tagStringToQString(const TagLib::String &ts)
{
    // TagLib 内部通常使用 UTF-8 编码
    return QString::fromUtf8(ts.toCString(true));
}

QImage AppMusicPlayer::getCoverFromMP3(const QString &filePath)
{
    const QString nativePath = QDir::toNativeSeparators(filePath);
#ifdef Q_OS_WIN
    TagLib::MPEG::File file(nativePath.toStdWString().c_str());
#else
    const QByteArray localPath = QFile::encodeName(nativePath);
    TagLib::MPEG::File file(localPath.constData());
#endif
    if (!file.isValid()) {
        return QImage();
    }

    TagLib::ID3v2::Tag *id3v2 = file.ID3v2Tag(false);
    if (!id3v2) {
        return QImage();
    }
    const auto frameList = id3v2->frameListMap()["APIC"];
    if (frameList.isEmpty()) {
        return QImage();
    }
    auto *picFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
    if (!picFrame) {
        return QImage();
    }

    const TagLib::ByteVector data = picFrame->picture();
    return QImage::fromData(reinterpret_cast<const uchar *>(data.data()), data.size());
}

QImage AppMusicPlayer::getCoverFromMP4(const QString &filePath)
{
    const QString nativePath = QDir::toNativeSeparators(filePath);
#ifdef Q_OS_WIN
    TagLib::MP4::File file(nativePath.toStdWString().c_str());
#else
    const QByteArray localPath = QFile::encodeName(nativePath);
    TagLib::MP4::File file(localPath.constData());
#endif
    if (!file.isValid()) {
        return QImage();
    }

    TagLib::MP4::Tag *tag = file.tag();
    if (!tag || !tag->contains("covr")) {
        return QImage();
    }
    const TagLib::MP4::CoverArtList artList = tag->item("covr").toCoverArtList();
    if (artList.isEmpty()) {
        return QImage();
    }
    const TagLib::ByteVector data = artList.front().data();
    return QImage::fromData(reinterpret_cast<const uchar *>(data.data()), data.size());
}

QImage AppMusicPlayer::getCoverFromOGG(const QString &filePath)
{
    const QString nativePath = QDir::toNativeSeparators(filePath);
#ifdef Q_OS_WIN
    TagLib::Ogg::Vorbis::File file(nativePath.toStdWString().c_str());
#else
    const QByteArray localPath = QFile::encodeName(nativePath);
    TagLib::Ogg::Vorbis::File file(localPath.constData());
#endif
    if (!file.isValid()) {
        return QImage();
    }

    TagLib::Ogg::XiphComment *xiph = file.tag();
    if (!xiph) {
        return QImage();
    }
    const auto pictures = xiph->pictureList();
    if (pictures.isEmpty() || !pictures.front()) {
        return QImage();
    }
    const TagLib::ByteVector data = pictures.front()->data();
    return QImage::fromData(reinterpret_cast<const uchar *>(data.data()), data.size());
}

AppMusicPlayer::~AppMusicPlayer() = default;
