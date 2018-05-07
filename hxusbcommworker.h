/* #############################################################################
 * Module: hxusbcommworker.h
 *
 * Creator: Joseph Mattern
 *
 * Last Edited: 7/27/2017
 *
 * Description: USB Handle Worker for Hexius USB Devices
 *
 * ###########################################################################*/

#ifndef HXUSBCOMMWORKER_H
#define HXUSBCOMMWORKER_H

#include "hxguiproperties.h"
#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include "ftd2xx.h"
#include "hx_commstd.h"
#include "hxutils.h"

#ifdef _COMM_DEBUG_ENABLED
#include <QDebug>
#endif

class HxUSBCommWorker : public QObject
{
    Q_OBJECT
public:
    explicit HxUSBCommWorker(QObject *parent = 0);
    void requestUSBComm(); //Thread Startup Function
    void abort(); //Sets Flag to Signal Thread to Abort
    bool commUSB(uchar *inBuffer, uchar txBytes);   //Setups values to initiate communication to the device
    bool getConnectedState();   //Returns the connected state of the device
    void getCommReadback(uchar *outBuffer, uchar *bytesRead);   //Returns the values in the buffer (Should only be called after the usbCommPacketDone() signal is emitted)
    void checkPorts(); //Sets Flag to Signal Thread to Check Connection

private:
    bool _abort;            //Stops thread operation
    bool _connectedState;   //Current Connected State of the Device
    bool _commBusy;         //Set when USB Communication is Busy
    bool _checkPorts;       //Flag set by the timer that controls when the ports need to be checked
    bool _commFlag;         //Set when communication packet is set up to send/recieve from the device
    bool _packetProcessing; //Flag that tells the port check routine to skip if the comm line is processing info

    QMutex mutex;
    FT_HANDLE ftHandle;
    FT_DEVICE_LIST_INFO_NODE *devNode;
    uchar txBuffer[64];
    uchar rxBuffer[64];
    DWORD numTxBytes;
    DWORD numRxBytes;
    DWORD numBytesRead;

    void clearRxBuffer();   //Clears the reading buffer before performing a read operation to prevent carryover of information

signals:
    void usbCommRequested(); //Startup Thread Signal
    void usbCommClosing(); //Ending Thread Signal
    void usbCommPacketDone(); //Signal to signify the end of communication
    void sendConnectedState(bool connectedState);   //Signal that sends the connected state of the device when certain conditions are met

public slots:
    void usbComm(); //Connection Thread

};
#endif // HXUSBCOMMWORKER_H
