#include "tmutempcycle.h"

TMUTempCycle::TMUTempCycle(uchar id, TMU* tmu)
{
    this->tmu = tmu;
    this->id = id;
    checkTimer = new QTimer;
    checkTimer->setInterval(TEMP_CYCLE_CHECK_PERIOD);
    checkTimer->setTimerType(Qt::CoarseTimer);
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(updateHeatLatch()));
}

TMUTempCycle::~TMUTempCycle()
{
    if (checkTimer->isActive())
    {
        checkTimer->stop();
    }
}

void TMUTempCycle::start()
{
    checkTimer->start();
}

void TMUTempCycle::pause()
{
    checkTimer->stop();
}

void TMUTempCycle::resume()
{
    this->start();
}

void TMUTempCycle::stop()
{
    checkTimer->stop();
    // Other stuff
}

void TMUTempCycle::updateHeatLatch()
{
    // Just check the tmu HEAT latch VARIABLE
    // If it meets the next temperature step
    // then, if necessary, update the HEAT latch
    // on the TMU chip.

    this->stop();


    this->start();
}

bool TMUTempCycle::setSawtooth(double tStart, double tStop, double period)
{
    // Build a piecewise set with evenly spaced temperatures to occupy the period
    // half up, half down

}

bool TMUTempCycle::setPiecewise(double* temp, int tempSize, double* time, int timeSize)
{

}

bool TMUTempCycle::setPiecewise(TTData* ttdata, int ttdataSize)
{

}

bool TMUTempCycle::setPiecewise(const std::vector<double>& temp, const std::vector<double>& time)
{

}

bool TMUTempCycle::setPiecewise(const std::vector<TTData>& ttdata)
{

}

bool TMUTempCycle::importTempProf(QString fname)
{
    fname = fname.prepend(CONFIG_FILE_PREFIX);
    fname = fname.contains(".xml") ? fname : fname.append(".xml");

    QDomDocument doc;
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
    {
        errStr = QString("Could not open %1 for IMPORTING temperature profile.").arg(fname);
#ifdef DEBUG
       qDebug() << errStr;
#endif
        return false;
    }


    QDomNodeList tempKeyList = doc.elementsByTagName(XML_TEMP).at(0).childNodes();
    QDomNodeList timeKeyList = doc.elementsByTagName(XML_TIME).at(0).childNodes();
    QDomNode key;
    bool status = false;
    int loopCount = std::max(tempKeyList.size(), timeKeyList.size()) > PIECEWISE_STEP_COUNT ?
                    PIECEWISE_STEP_COUNT : std::min(tempKeyList.size(), timeKeyList.size());

    int i = 0;
    for (i = 0; i < loopCount; i++)
    {
        // Temperature
        key = tempKeyList.at(i);
        tempProfile[i].temp = key.toElement().text().toDouble(&status);
        if (!status)
        {
            errStr = QString("Failed to IMPORT, file corrupted: %1").arg(fname);
#ifdef DEBUG
       qDebug() << errStr;
#endif
            return false;
        }

        // Time
        key = timeKeyList.at(i);
        tempProfile[i].time = key.toElement().text().toDouble(&status);
        if (!status)
        {
            errStr = QString("Failed to IMPORT, file corrupted: %1").arg(fname);
#ifdef DEBUG
       qDebug() << errStr;
#endif
            return false;
        }
    }
    file.close();
    return true;
}

bool TMUTempCycle::importTempProf()
{
    importTempProf(profileFileName);
}

bool TMUTempCycle::exportTempProf(QString fname)
{
    fname = fname.prepend(CONFIG_FILE_PREFIX);
    fname = fname.contains(".xml") ? fname : fname.append(".xml");

    QFile file(fname);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        errStr = QString("Could not open %1 for EXPORTING temperature profile.").arg(fname);
#ifdef DEBUG
       qDebug() << errStr;
#endif
        return false;
    }

    QDomDocument doc;
    QDomProcessingInstruction header = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(header);

    QDomElement root = doc.createElement(XML_ROOT);
    doc.appendChild(root);

    QDomElement subroot_temp;
    QDomElement tempN;
    QDomElement subroot_time;
    QDomElement timeN;
    QDomElement key;
    QDomText val1;
    QString val = "";
    subroot_temp = doc.createElement(XML_TEMP);
    root.appendChild(subroot_temp);
    subroot_time = doc.createElement(XML_TIME);
    root.appendChild(subroot_time);

    unsigned int i = 0;
    for (const auto t: tempProfile)
    {
        // Temperature
        tempN = doc.createElement(XML_TEMP_N.arg(i));
        subroot_temp.appendChild(tempN);
        val = QString::number(t.temp);
        val1 = doc.createTextNode(val);
        tempN.appendChild(val1);

        // Time
        timeN = doc.createElement(XML_TIME_N.arg(i));
        subroot_time.appendChild(timeN);
        val = QString::number(t.time);
        val1 = doc.createTextNode(val);
        timeN.appendChild(val1);

        i++;
    }

    QTextStream output(&file);
    output << doc.toString();
    file.close();
    return true;
}

bool TMUTempCycle::exportTempProf()
{
    this->exportTempProf(profileFileName);
}

void TMUTempCycle::debugExport()
{
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        tempProfile[i].temp = 90.1;
        tempProfile[i].time = 10;
    }
    this->exportTempProf();
}

void TMUTempCycle::debugExportImport()
{
    this->debugExport();
    this->importTempProf();
    for (const auto t: tempProfile)
    {
        qDebug() << "Debug Temp: " << QString::number(t.temp);
        qDebug() << "Debug Time: " << QString::number(t.time);
    }
}
