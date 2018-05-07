/* #############################################################################
 * Module: hxusbcomm.h
 *
 * Creator: Joseph Mattern
 *
 * Last Edited: 7/27/2017
 *
 * Description: USB Interface Object for Hexius USB Devices
 *
 * ###########################################################################*/

#ifndef HXUSBCOMM_H
#define HXUSBCOMM_H


#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include "ftd2xx.h"
#include "hx_commstd.h"
#include "hxutils.h"
#include "hxusbcommworker.h"
#include "hxguiproperties.h"

#ifdef _COMM_DEBUG_ENABLED
#include <QDebug>
#endif

enum USB_READBACK_TYPE{
    READBACK_NULL,
    READBACK_DIRECT,
    READBACK_DATA,
    READBACK_CHIP_INFO,
    READBACK_STATUS,
    READBACK_LAST_PACKET,
    READBACK_INPUT_BUFFER,
    READBACK_DEBUG_VAR,
    READBACK_FLUSH_OUTPUT,
    READBACK_FLUSH_INPUT
};

enum USB_COMM_STATE{
    USB_NULL,
    USB_ERROR,
    USB_DIRECT,
    USB_DATA,
    USB_DATA_START,
    USB_DATA_BUSY_STATUS_READBACK,
    USB_DATA_READBACK,
    USB_CHIP_INFO,
    USB_STATUS_READBACK,
    USB_LAST_PACKET,
    USB_INPUT_BUFFER,
    USB_DEBUG_VAR,
    USB_FLUSH_OUTPUT,
    USB_FLUSH_INPUT
};

enum RESEND_STATE{
    RESEND,
    REREAD,
    DO_NOTHING,
    BUSY,
};


class HxUSBComm : public QObject
{
    Q_OBJECT
public:
    HxUSBComm();
    ~HxUSBComm();
    void abort();

    //Put User Interfacing Functions Here
    void writeDirect(uchar *inBuffer, uchar txBytes); //Directly write/read the bytes across the UART line
    void getReadbackDirect(uchar *outBuffer, uchar *numBytes); //get the information from the USB for display

    void writeData(uchar cmdByte, uchar *inBuffer, uchar packetBytes); //Write a data packet to the UART line (auto calculates checksum)
    uchar getReadbackData(uchar *cmdByte, uchar *outBuffer, uchar *packetBytes); //get the information from the USB for display

    void getReadbackInnerPacket(uchar *outBuffer, uchar *packetBytes);
    void flushInputBuffer();
    void flushOutputBuffer();
    void requestChipIDandRevision();
    void requestInputBuffer();
    void requestDebugVariable();
    void requestPreviousPacket();

signals:
//    void updateDirectReady(); //Signal to tell GUI that a direct readback is ready
    void updateDataReady(USB_READBACK_TYPE readbackType);     //Signal to tell GUI that a data readback is ready
    void commError(QString errorString);    //Signal that emits an error string describing the error that occured
    void commBusy();    //Signal that occurs when an operation is attempted while the comm line is busy
    void connectedStateChanged(bool connectedState);   //Signal that notifies the main window of a change in connected state

private slots:
    void timerTimeout();  //Timeout function for port checking
    void updateReadback();  //Function that emits the appropriate main window update signal based on the readback data
    void retryTimerTimeout();   //Function that sets up the retry packet to be sent
    void getConnectedState(bool connectedState); //Function that emits a change in connected state to the main window

private:
    QString errorString;        //Error String Sent When commError is emitted
    uchar inputBuffer[64];      //Buffer to send to USB Comm Worker
    uchar numBytesSend;         //Numbers of Bytes to send to USB Comm Worker
    uchar numRetries;           //Number of retries attempted
    uchar retryLimit;           //Number of retries allowed before timeout
    uchar packetCmd;            //Initial Inner Packet Command Sent to USB Comm Worker
    USB_COMM_STATE commState;   //Comm State controlling what outer packet is expected
    RESEND_STATE resendState;   //Controls what operation is done when a retry is attempted
    bool _abort;                //Closes all threads
    bool _lastConnectedState;   //Last device connected state (used to check if the connected state changed)

    QThread *usbThread;
    HxUSBCommWorker *hxUSBCommWorker;
    QTimer *portCheckTimer;
    QTimer *retryCommTimer;

    void resetTimer();
    void startRetry(uchar statusReturn);    //Initiates Retry
    void startRetryTimer(RESEND_STATE retryState);      //Initiates Retry Timer
    uchar statusReadback(uchar cmdByte, uchar *outBuffer, uchar numBytes);  //Verifies Status Return Packet
    void setCommState(USB_COMM_STATE usbState); //Sets commState and Resets Relevant Variables
    void handleStatusReturn(uchar statusReadback, USB_READBACK_TYPE readbackType);  //Handles General Returns
};

#endif // HXUSBCOMM_H
