#ifndef APPTHREADPOOL_H
#define APPTHREADPOOL_H

class QThreadPool;

class AppThreadPool
{
public:
    static QThreadPool *globalPool();
};

#endif // APPTHREADPOOL_H
