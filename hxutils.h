#ifndef HXUTILS_H
#define HXUTILS_H

#include <QDebug>
#include <QString>
#include <QTableWidgetItem>
#include <Qtime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QString>
#include <QTableWidgetItem>

class HxUtils
{
public:
    HxUtils();
    static void delaySec(void);
    static void delayMilli(int);
    static QTableWidgetItem* getQTWI(QString);
    static QTableWidgetItem* getQTWIchar(uchar);
    static QString ucharToQString(uchar);
    static QString ucharArrayToQString(uchar *ucharArray, uchar numBytes);
    static uchar qstringToUchar(QString);
    static void printUCharArr(uchar[], uchar);
    static QString fNameFromPath(QString);
    static QString uchToBinQStr(uchar);
    static uchar qstringToHex(uchar *buffer, QString inputString);
    static uchar calcChecksum(uchar *buffer, uchar numBytes);
    static QByteArray ucharArrayToQByteArray(uchar *buffer, uchar size);
    static void qByteArrayToUcharArray(QByteArray &byteArray, uchar *buffer, uchar size);
};

#endif // HXUTILS_H
