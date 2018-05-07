#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QTableWidgetItem>
#include <Qtime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QString>
#include <QTableWidgetItem>
#include <QDebug>

class utils
{
public:
    utils();
    static void delaySec(void);
    static void delayMilli(int);
    static QTableWidgetItem* getQTWI(QString);
    static QTableWidgetItem* getQTWIchar(uchar);
    static QString ucharToQString(uchar);
    static QString ucharToHexQString(uchar c, bool header);
    static QString ucharArrayToQString(uchar *ucharArray, uchar numBytes);
    static uchar qstringToUchar(QString);
    static void printUCharArr(uchar[], uchar);
    static QString fNameFromPath(QString);
    static QString uchToBinQStr(uchar);
    static uchar qstringToHex(uchar *buffer, QString inputString);
    static uchar calcChecksum(uchar *buffer, uchar numBytes);
	static uchar calcCheckSum(std::vector<uchar> v);
    static QString uchVecToQStr(std::vector<uchar> v);
    static QString pad8bitHexToQStr(uchar c);
    static QString decToHex(uchar dec);
};

#endif // UTILS_H
