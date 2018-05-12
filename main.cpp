#include "mainwindow.h"
#include <QApplication>

#ifdef DEBUG
QString CONFIG_FILE_PREFIX = QCoreApplication::applicationDirPath().replace("build-TMU_GUI-Desktop_Qt_5_8_0_MinGW_32bit-Debug/", "");
#elif RELEASE
QString CONFIG_FILE_PREFIX = QCoreApplication::applicationDirPath().replace("build-TMU_GUI-Desktop_Qt_5_8_0_MinGW_32bit-Release/release", "");
#elif EXE
QString CONFIG_FILE_PREFIX = QCoreApplication::applicationDirPath();
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
