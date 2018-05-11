#include "tmudaemon.h"

TMUDaemon::TMUDaemon(TMU *tmu)
{
    this->tmu = tmu;
    checkTimer = new QTimer;
    checkTimer->setInterval(DAEMON_CHECK_PERIOD);
    connect(checkTimer, SIGNAL(timeout()), this, SLOT(checkTemperature());
}

TMUDaemon::~TMUDaemon()
{
    if (checkTimer->isActive())
    {
        checkTimer->stop();
    }
}

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
    tmu->mutex.unlock();
    this->start();
}
