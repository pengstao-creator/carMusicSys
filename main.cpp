#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <QThread>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // // 重定向标准输出和标准错误到空设备
    // HANDLE hNull = CreateFile(TEXT("NUL"), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // if (hNull != INVALID_HANDLE_VALUE) {
    //     SetStdHandle(STD_OUTPUT_HANDLE, hNull);
    //     SetStdHandle(STD_ERROR_HANDLE, hNull);
    //     // 同时重定向C标准库的输出
    //     int hCrt = _open_osfhandle((intptr_t)hNull, _O_WRONLY);
    //     FILE *hf = _fdopen(hCrt, "w");
    //     *stdout = *hf;
    //     *stderr = *hf;
    //     setvbuf(stdout, NULL, _IONBF, 0);
    //     setvbuf(stderr, NULL, _IONBF, 0);
    // }
#endif
    
    // // 禁用所有Qt日志输出
    // QLoggingCategory::setFilterRules("*.debug=false\n*.info=false\n*.warning=false\n*.critical=false");
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "windows"); // 直接覆盖环境变量
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
