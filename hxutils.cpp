#include "hxutils.h"


// HEXIUS developed utility file
// Author: Max Ruiz
// Status: Working


void HxUtils::delaySec(void)
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void HxUtils::delayMilli(int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
        while( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        }
}

QTableWidgetItem* HxUtils::getQTWI(QString s)
{
    QTableWidgetItem *itab;
    itab = new QTableWidgetItem;
    itab->setText(s);
    return itab;
}

QTableWidgetItem* HxUtils::getQTWIchar(uchar c)
{
    QString s = ucharToQString(c);
    QTableWidgetItem *itab;
    itab = new QTableWidgetItem;
    itab->setText(s);
    return itab;
}

QString HxUtils::ucharToQString(uchar c)
{
    QString temp;
    temp = QStringLiteral("%1").arg(c,0,16);
    return temp;

}

QString HxUtils::ucharArrayToQString(uchar *ucharArray, uchar numBytes)
{
    QString returnString = "";
    QString temp = "";
    for (uchar i=0;i<numBytes;i++)
    {
        temp = ucharToQString(ucharArray[i]);
        if (temp.size() == 1)
        {
            temp.prepend("0");
        }
        returnString.append(temp);
        returnString.append(" ");
    }
    return returnString;

}
uchar HxUtils::qstringToUchar(QString s)
{
    bool bStatus = false;
    uchar c;
    c = (uchar)s.toUInt(&bStatus,16);
    return c;
}

void HxUtils::printUCharArr(uchar uch[], uchar size)
{
    for (int i = 0; i < size; i++)
    {
        qDebug() << ucharToQString(uch[i]);
    }
}

QString HxUtils::fNameFromPath(QString path)
{
    QRegularExpression filename_re("\\/");
    QStringList fname_hierarchy = path.split(filename_re);
    return fname_hierarchy[fname_hierarchy.size()-1];
}


QString HxUtils::uchToBinQStr(uchar c)
{
    QString cQStr;
    for (int i = 0; i < 8; ++i) {
        cQStr.prepend(ucharToQString((c >> i) & 1));
    }
    return cQStr;
}

uchar HxUtils::qstringToHex(uchar *buffer, QString inputString)
{
    QString temp ="";
    bool status;
    int stringSize = inputString.length();
    uchar numBytes = 0;

    if (inputString.isEmpty())
    {
//        qDebug() << "String is empty";
    }
    else
    {
        if (stringSize%2 == 0)
        {
            for(int i=0;i<stringSize;i++)
            {
//                qDebug() << QString::number(i%2);
                if (i == 0)
                {
                    temp = inputString[i];
                }
                else if(i%2 == 0)
                {
                    //qDebug() << temp;
                    temp = inputString[i];

                }
                else
                {
                    temp.append(inputString[i]);
                    buffer[numBytes++] = (uchar)temp.toUInt(&status, 16);

                }
            }
        }
        else
        {
            qDebug() << "Invalid String";
        }
    }
    return numBytes;
}
//To validate a checksum, include the checksum byte in numBytes
//To create a checksum, do not include the checksum byte in numBytes
uchar HxUtils::calcChecksum(uchar *buffer, uchar numBytes)
{
    uchar checksum = 0;
    for (uchar i=0;i<numBytes;i++)
    {
        checksum += buffer[i];
    }
    checksum = (~checksum) + 1;
    return checksum;
}

QByteArray HxUtils::ucharArrayToQByteArray(uchar *buffer, uchar size)
{
    QByteArray returnArray;
    returnArray.resize(size);
    for(uchar i = 0; i < size; i++)
    {
        returnArray[i] = buffer[i];
    }
    return returnArray;
}

void HxUtils::qByteArrayToUcharArray(QByteArray &byteArray, uchar *buffer, uchar size)
{
    for(uchar i = 0; i < size; i++)
    {
        buffer[i] = byteArray[i];
    }
}

template <class T>
T HxUtils::arrMin(T* arr, int size)
{
    T min = arr[0];
    for (int i = 0; i < size; i++)
    {
        min = std::min(min, arr[i]);
    }
    return min;
}

template <class T>
T HxUtils::arrMax(T* arr, int size)
{
    T max = arr[0];
    for (int i = 0; i < size; i++)
    {
        max = std::max(max, arr[i]);
    }
    return max;
}
