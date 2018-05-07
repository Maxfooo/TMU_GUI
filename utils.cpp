#include "utils.h"


// HEXIUS developed utility file
// Author: Max Ruiz
// Status: Working


void utils::delaySec(void)
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void utils::delayMilli(int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
        while( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        }
}

QTableWidgetItem* utils::getQTWI(QString s)
{
    QTableWidgetItem *itab;
    itab = new QTableWidgetItem;
    itab->setText(s);
    return itab;
}

QTableWidgetItem* utils::getQTWIchar(uchar c)
{
    QString s = ucharToQString(c);
    QTableWidgetItem *itab;
    itab = new QTableWidgetItem;
    itab->setText(s);
    return itab;
}

QString utils::ucharToQString(uchar c)
{
    QString temp;
    temp = QStringLiteral("%1").arg(c,0,16);
    return temp;

}

QString utils::ucharToHexQString(uchar c, bool header)
{
    QString temp = QStringLiteral("%1").arg(c, 0, 16);
    if (temp.size() == 1)
    {
        temp.prepend("0");
    }
    if (header)
    {
        temp.prepend("0x");
    }
    return temp;
}

QString utils::ucharArrayToQString(uchar *ucharArray, uchar numBytes)
{
    QString returnString = "";
    QString temp = "";
    for (uchar i=0;i<numBytes;i++)
    {
        temp = utils::ucharToQString(ucharArray[i]);
        if (temp.size() == 1)
        {
            temp.prepend("0");
        }
        returnString.append(temp);
        returnString.append(" ");
    }
    return returnString;

}
uchar utils::qstringToUchar(QString s)
{
    bool bStatus = false;
    uchar c;
    c = (uchar)s.toUInt(&bStatus,16);
    return c;
}

void utils::printUCharArr(uchar uch[], uchar size)
{
    for (int i = 0; i < size; i++)
    {
        qDebug() << ucharToQString(uch[i]);
    }
}

QString utils::fNameFromPath(QString path)
{
    QRegularExpression filename_re("\\/");
    QStringList fname_hierarchy = path.split(filename_re);
    return fname_hierarchy[fname_hierarchy.size()-1];
}


QString utils::uchToBinQStr(uchar c)
{
    QString cQStr;
    for (int i = 0; i < 8; ++i) {
        cQStr.prepend(ucharToQString((c >> i) & 1));
    }
    return cQStr;
}

uchar utils::qstringToHex(uchar * buffer, QString inputString)
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
uchar utils::calcChecksum(uchar *buffer, uchar numBytes)
{
    uchar checksum = 0;
    for (uchar i=0;i<numBytes;i++)
    {
        checksum += buffer[i];
    }
    checksum = (~checksum) + 1;
    return checksum;
}

uchar utils::calcCheckSum(std::vector<uchar> v)
{
    uchar chsum = 0;
    for (auto const& value: v)
    {
        chsum += value;
    }
    chsum = (chsum ^ 0xFF) + 1;
    return chsum;
}

QString utils::uchVecToQStr(std::vector<uchar> v)
{
    QString temp = "";
    for (auto const& value: v)
    {
        temp += utils::pad8bitHexToQStr(value);
    }
    return temp;
}

QString utils::pad8bitHexToQStr(uchar c)
{
    QString temp = utils::ucharToQString(c);
    if (temp.length() < 2)
    {
        temp.prepend("0");
    }
    return temp;
}

QString utils::decToHex(uchar dec)
{
    QString temp = QString("%1").arg(dec, 2, 16, QLatin1Char('0'));
    return temp;
}
