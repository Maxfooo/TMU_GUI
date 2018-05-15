#include "tmudaemon.h"

TMUDaemon::TMUDaemon(uchar id, TMU *tmu)
{
    this->id = id;
    this->tmu = tmu;
    checkTimer = new QTimer;
    checkTimer->setInterval(DAEMON_CHECK_PERIOD);
    checkTimer->setTimerType(Qt::CoarseTimer);
    //connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkTemperature()));
}

TMUDaemon::~TMUDaemon()
{
    if (checkTimer->isActive())
    {
        checkTimer->stop();
    }
}

//void TMUDaemon::initDaemon()
//{

//}

void TMUDaemon::start()
{
    checkTimer->start();
}

void TMUDaemon::stop()
{
    checkTimer->stop();
}

void TMUDaemon::checkTemperature()
{
    this->stop();
    tmu->mutex.lock();
    // check temperature of TMU
    // will be through the MUX in the TMU

    // MUST EMIT SIGNAL THAT THE MAINWINDOW PICKS UP
    // IN ORDER TO SEND THE INFORMATION VIA THE COMM LINE
    emit this->rxTemperature(this->id);


    tmu->mutex.unlock();
    this->start();
}
