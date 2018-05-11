#include "processrunningle.h"

ProcessRunningLE::ProcessRunningLE(QLineEdit* procRunLE)
{
    this->procRunLE = procRunLE;
    initProcRunLE();
}

ProcessRunningLE::ProcessRunningLE(QLineEdit* procRunLE, double period)
{
    this->procRunLE = procRunLE;
    this->period = period*500;
    initProcRunLE();
}

ProcessRunningLE::~ProcessRunningLE()
{
    if (procRunTimer->isActive())
    {
        procRunTimer->stop();
    }
}

void ProcessRunningLE::initProcRunLE()
{
    //leLength = procRunLE->maxLength();
    leLength = std::floor(procRunLE->width()/3);
    sliderLength = std::floor(0.25*leLength);
    timeStep = period/sliderLength;
    procRunTimer = new QTimer;
    procRunTimer->setInterval(timeStep);
    procRunTimer->setTimerType(Qt::CoarseTimer);
    initProcRunText();

    connect(procRunTimer, SIGNAL(timeout()), this, SLOT(updateRunProcLE()));
}

void ProcessRunningLE::initProcRunText()
{
    for (int i = 0; i < leLength; i++)
    {
        if (i < sliderLength)
        {
            procRunText.push_back("|");
        }
        else
        {
            procRunText.push_back(" ");
        }
    }
}

void ProcessRunningLE::setPeriod(double period)
{
    this->period = 500*period;
    timeStep = this->period/leLength;
}

void ProcessRunningLE::startProcRun()
{
    initProcRunText();
    procRunLE->setText(getLEText());
    procRunTimer->start();
}

void ProcessRunningLE::pauseProcRun()
{
    procRunTimer->stop();
}

void ProcessRunningLE::resumeProcRun()
{
    procRunTimer->start();
}

void ProcessRunningLE::stopProcRun()
{
    procRunTimer->stop();
    procRunLE->setText("");
}

void ProcessRunningLE::updateRunProcLE()
{
    procRunTimer->stop();
    procRunText = rotVect<QString>(true, procRunText, 1);
    procRunLE->setText(getLEText());
    procRunTimer->start();
}

QString ProcessRunningLE::getLEText()
{
    QString text = "";
    for (int i = 0; i < procRunText.size(); i++)
    {
        text += procRunText.at(i);
    }
    return text;
}

template <class T>
std::vector<T> ProcessRunningLE::rotVect(bool toRight, std::vector<T> v, unsigned int rot)
{
    std::vector<T> vrot = v;
    for (int i = 0; i < rot; i++)
    {
        if (toRight)
        {
            vrot.insert(vrot.begin(), vrot.back());
            vrot.pop_back();
        }
        else
        {
            vrot.push_back(vrot.front());
            vrot.erase(vrot.begin());
        }
    }
    return vrot;
}
