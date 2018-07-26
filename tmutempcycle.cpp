#include "tmutempcycle.h"

TMUTempCycle::TMUTempCycle(uchar id, TMU* tmu, QMainWindow *parent)
{
    this->tmu = tmu;
    this->id = id;
    checkTimer = new QTimer;
    checkTimer->setInterval(TEMP_CYCLE_CHECK_PERIOD);
    checkTimer->setTimerType(Qt::CoarseTimer);
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(updateHeatLatch()));

    saveMyUI = new SaveMyUI(parent, profileFileName);
}

TMUTempCycle::~TMUTempCycle()
{
    if (checkTimer->isActive())
    {
        checkTimer->stop();
    }
}

void TMUTempCycle::setFileName(QString fname)
{
    profileFileName = fname;
    //saveMyUI->setFileName(fname);
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
    // then update the HEAT latch on the TMU chip.

    this->stop();

    // EMIT A SIGNAL--WITH PARAMETER TMU_ID--TO HAVE THE MAINWINDOW
    // COMMUNICATE TO THE CORRECT TMU
    emit this->updateTMU(this->id);

    this->start();
}

void TMUTempCycle::setSawtooth(double tStart, double tStop, double period)
{
    // Build a piecewise set with evenly spaced temperatures to occupy the period
    // half up, half down
    cycleType = SET_SAWTOOTH;
    this->period = period;
    unsigned int timeStep = std::round(period / PIECEWISE_STEP_COUNT);
    double tempStep = (tStop - tStart) / (PIECEWISE_STEP_COUNT / 2);
    double temperature = tStart;
    int j = 0;
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        // Half up and half back down
        if (i < PIECEWISE_STEP_COUNT/2)
        {
            temperature = tStart + i*tempStep;
        }
        else
        {
            temperature = tStop - j*tempStep;
            j++;
        }
        tempProfile[i].temp = std::min(temperature, static_cast<double>(TEMP_CELSIUS_MAX));
        tempProfile[i].time = timeStep;
    }
    piecewiseCount = PIECEWISE_STEP_COUNT;
    //return this->exportTempProf(profileFileName);
    this->exportProfile();

}

void TMUTempCycle::setPiecewise(double* temp, int tempSize, double* time, int timeSize)
{
    cycleType = SET_PIECEWISE;
    piecewiseCount = static_cast<unsigned int>(std::min(std::min(tempSize, timeSize), PIECEWISE_STEP_COUNT));
    for (int i = 0; i < piecewiseCount; i++)
    {
        tempProfile[i].temp = temp[i];
        tempProfile[i].time = time[i];
    }
    //return this->exportTempProf(profileFileName);
    this->exportProfile();
}

void TMUTempCycle::setPiecewise(TTData* ttdata, int ttdataSize)
{
    cycleType = SET_PIECEWISE;
    piecewiseCount = static_cast<unsigned int>(ttdataSize, PIECEWISE_STEP_COUNT);
    for (int i = 0; i < piecewiseCount; i++)
    {
        tempProfile[i] = ttdata[i];
    }
    //return this->exportTempProf(profileFileName);
    this->exportProfile();
}

void TMUTempCycle::setPiecewise(const std::vector<double>& temp, const std::vector<double>& time)
{
    cycleType = SET_PIECEWISE;
    piecewiseCount = static_cast<unsigned int>(std::min(std::min(temp.size(), time.size()), static_cast<const unsigned int>(PIECEWISE_STEP_COUNT)));
    for (int i = 0; i < piecewiseCount; i++)
    {
        tempProfile[i].temp = temp.at(i);
        tempProfile[i].time = time.at(i);
    }
    //return this->exportTempProf(profileFileName);
    this->exportProfile();
}

void TMUTempCycle::setPiecewise(const std::vector<TTData>& ttdata)
{
    cycleType = SET_PIECEWISE;
    piecewiseCount = static_cast<unsigned int>(std::min(static_cast<int>(ttdata.size()), PIECEWISE_STEP_COUNT));
    for (int i =0; i < piecewiseCount; i++)
    {
        tempProfile[i] = ttdata.at(i);
    }
    //return this->exportTempProf(profileFileName);
    this->exportProfile();
}

void TMUTempCycle::setPiecewise(const std::vector<TTData*>& ttdata)
{
    cycleType = SET_PIECEWISE;
    piecewiseCount = static_cast<unsigned int>(std::min(static_cast<int>(ttdata.size()), PIECEWISE_STEP_COUNT));
    for (int i =0; i < piecewiseCount; i++)
    {
        tempProfile[i].temp = ttdata.at(i)->temp;
        tempProfile[i].time = ttdata.at(i)->time;
    }
    //return this->exportTempProf(profileFileName);
    this->exportProfile();
}

std::vector<TTData> TMUTempCycle::getPiecewise()
{
    std::vector<TTData> ttdVect;
    for (int i = 0; i < piecewiseCount; i++)
    {
        ttdVect.push_back(tempProfile[i]);
    }
    return ttdVect;
}

void TMUTempCycle::getTempSpan(double& tStart, double& tStop)
{
    tStart = minTemp().temp;
    tStop = maxTemp().temp;
}

void TMUTempCycle::importProfile(QString fname)
{
    profileFileName = fname;
    //saveMyUI->setFileName(fname);
    saveMyUI->loadFromXML(profileFileName);
}

void TMUTempCycle::importProfile()
{
    importProfile(profileFileName);
}

void TMUTempCycle::exportProfile(QString fname)
{
    profileFileName = fname;
    //saveMyUI->setFileName(fname);
    saveMyUI->saveToXML(profileFileName);
}

void TMUTempCycle::exportProfile()
{
    exportProfile(profileFileName);
}

bool TMUTempCycle::importTempProf(QString fname)
{
    fname = fname.contains(".xml") ? fname : fname.append(".xml");

    QDomDocument doc;
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
    {
        errStr = QString("Could not open %1 for IMPORTING temperature profile.").arg(fname);
        #ifdef DEBUG
               qDebug() << errStr;
        #endif
        tmuTempCycleReady = false;
        return false;
    }

    bool status = false;
    cycleType = static_cast<bool>(doc.elementsByTagName(XML_CYCLE_TYPE).at(0).toElement().text().toUInt(&status));
    period = doc.elementsByTagName(XML_PERIOD).at(0).toElement().text().toDouble(&status);
    QDomNodeList tempKeyList = doc.elementsByTagName(XML_TEMP).at(0).childNodes();
    QDomNodeList timeKeyList = doc.elementsByTagName(XML_TIME).at(0).childNodes();
    QDomNode key;

    piecewiseCount = std::max(tempKeyList.size(), timeKeyList.size()) > PIECEWISE_STEP_COUNT ?
                    PIECEWISE_STEP_COUNT : std::min(tempKeyList.size(), timeKeyList.size());

    int i = 0;
    for (i = 0; i < piecewiseCount; i++)
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
            tmuTempCycleReady = false;
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
            tmuTempCycleReady = false;
            return false;
        }
    }

    file.close();
    tmuTempCycleReady = true;
    return true;
}

bool TMUTempCycle::importTempProf()
{
    return importTempProf(profileFileName);
}

bool TMUTempCycle::exportTempProf(QString fname)
{
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


    QDomElement subroot_cycleType = doc.createElement(XML_CYCLE_TYPE);
    root.appendChild(subroot_cycleType);
    subroot_cycleType.appendChild(doc.createTextNode(QString::number(cycleType)));

    QDomElement subroot_period = doc.createElement(XML_PERIOD);
    root.appendChild(subroot_period);
    subroot_period.appendChild(doc.createTextNode(QString::number(period)));

    QDomElement subroot_temp = doc.createElement(XML_TEMP);
    QDomElement tempN;
    QDomElement subroot_time = doc.createElement(XML_TIME);
    QDomElement timeN;
    QDomElement key;
    QDomText val1;
    QString val = "";
    root.appendChild(subroot_temp);
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

TTData TMUTempCycle::minTemp()
{
    int minIndex = 0;
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        minIndex == tempProfile[i].temp < tempProfile[minIndex].temp ?
                    i : minIndex;
    }
    return tempProfile[minIndex];
}

TTData TMUTempCycle::minTime()
{
    int minIndex = 0;
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        minIndex == tempProfile[i].time < tempProfile[minIndex].time ?
                    i : minIndex;
    }
    return tempProfile[minIndex];
}

TTData TMUTempCycle::maxTemp()
{
    int maxIndex = 0;
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        maxIndex == tempProfile[i].temp > tempProfile[maxIndex].temp ?
                    i : maxIndex;
    }
    return tempProfile[maxIndex];
}

TTData TMUTempCycle::maxTime()
{
    int maxIndex = 0;
    for (int i = 0; i < PIECEWISE_STEP_COUNT; i++)
    {
        maxIndex == tempProfile[i].time > tempProfile[maxIndex].time ?
                    i : maxIndex;
    }
    return tempProfile[maxIndex];
}
