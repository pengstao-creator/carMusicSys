#ifndef CACHEMANAGER_HPP
#define CACHEMANAGER_HPP

//用于各种api请求数据缓存

#include <QString>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QThread>

template<typename DataType>
class CacheManager
{
public:
    CacheManager(const QString &cacheSubDir, int expireHours = 23)
        : cacheSubDir_(cacheSubDir), expireHours_(expireHours)
    {
        QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        cacheDir_ = baseDir + "/" + cacheSubDir_;
        QDir dir(cacheDir_);
        if (!dir.exists()) dir.mkpath(".");
    }

    bool load(const QString &key, DataType &data) const
    {

        QString filePath = cacheFilePath(key);
        if (!isValid(filePath)) return false;

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return false;
        QByteArray rawData = file.readAll();
        file.close();
        return data.fromJson(rawData);
    }

    bool save(const QString &key, const DataType &data) const
    {
        QString filePath = cacheFilePath(key);
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) return false;
        file.write(data.toJson());
        file.close();
        return true;
    }

    bool clear(const QString &key) const
    {
        return QFile::remove(cacheFilePath(key));
    }

    void clearAll() const
    {
        QDir dir(cacheDir_);
        for (const QString &file : dir.entryList(QDir::Files))
            dir.remove(file);
    }

    const QString& getcacheDir()const {return cacheDir_;};
private:
    QString cacheFilePath(const QString &key) const
    {
        return cacheDir_ + "/" + key + ".json";
    }

    bool isValid(const QString &filePath) const
    {
        QFileInfo info(filePath);
        if (!info.exists()) return false;
        qint64 ageSecs = info.lastModified().secsTo(QDateTime::currentDateTime());
        return ageSecs < expireHours_ * 3600;
    }

    QString cacheDir_;
    QString cacheSubDir_;
    int expireHours_;
};




#endif // CACHEMANAGER_HPP
