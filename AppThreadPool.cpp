#include "AppThreadPool.h"
#include <QThread>
#include <QThreadPool>

QThreadPool *AppThreadPool::globalPool()
{
    static QThreadPool pool;
    static bool initialized = false;
    if (!initialized) {
        // 全局线程池仅用于后台任务（文件IO/JSON解析等），不直接操作UI和QNetwork对象。
        const int ideal = QThread::idealThreadCount();
        pool.setMaxThreadCount(qMax(2, ideal - 1));
        pool.setExpiryTimeout(60 * 1000);
        initialized = true;
    }
    return &pool;
}
