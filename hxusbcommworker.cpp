#include "hxusbcommworker.h"


/*******************************************************************************
 * -----------------------------------------------------------------------------
 * Startup/Closing Functions
 * -----------------------------------------------------------------------------
 ******************************************************************************/


HxUSBCommWorker::HxUSBCommWorker(QObject *parent) : QObject(parent)
{
    _abort = false;
    _connectedState = false;
    _commBusy = false;
    _checkPorts = false;
    _commFlag = false;
    _packetProcessing = false;
}


/*********************************************************************
 * Function Name: requestUSBComm()
 *
 * Description: Starts USB Comm Thread
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes:
 ********************************************************************/
void HxUSBCommWorker::requestUSBComm()
{
    mutex.lock();
    _abort = false;
    #ifdef _COMM_DEBUG_ENABLED
    qDebug() << "USB Comm Requested";
    #endif
    mutex.unlock();
    emit usbCommRequested();
}


/*********************************************************************
 * Function Name: abort()
 *
 * Description: Starts the process of closing the threads
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes:
 ********************************************************************/
void HxUSBCommWorker::abort()
{
    mutex.lock();
    _abort = true;
    #ifdef _COMM_DEBUG_ENABLED
    qDebug() << "Aborting...";
    #endif
    mutex.unlock();
}




/*******************************************************************************
 * -----------------------------------------------------------------------------
 * Public Interface Functions
 * -----------------------------------------------------------------------------
 ******************************************************************************/

/*********************************************************************
 * Function Name: getConnectedState()
 *
 * Description: Returns the connected state of the device
 *
 * Parameters: None
 *
 * Returns: _connectedState
 *
 * Notes:
 ********************************************************************/
bool HxUSBCommWorker::getConnectedState()
{
    bool returnValue;
    mutex.lock();
    returnValue = _connectedState;
    mutex.unlock();
    return returnValue;
}


/*********************************************************************
 * Function Name: getCommReadback(uchar *outBuffer, uchar *bytesRead)
 *
 * Description: Returns the readback data from USB Communication
 *
 * Parameters:
 *      uchar *outBuffer
 *          -Used for returning values
 *      uchar *bytesRead
 *          -Used for returning values
 *
 * Returns:
 *      Number of Bytes Read at *bytesRead
 *      Readback Bytes into the array starting at *outBuffer
 *
 * Notes: Should only be called after usbCommPacketDone() is emitted
 ********************************************************************/
void HxUSBCommWorker::getCommReadback(uchar *outBuffer, uchar *bytesRead)
{
    mutex.lock();
    _packetProcessing = false;
    *bytesRead = numBytesRead;
    memcpy(outBuffer, rxBuffer, numBytesRead);
    mutex.unlock();
}


/*********************************************************************
 * Function Name: commUSB(uchar *inBuffer, uchar txBytes)
 *
 * Description: Sends a data packet to the USB line and waits for
 *      a response.
 *
 * Parameters:
 *      uchar *inBuffer
 *          -Start of array containing send packet
 *      uchar txBytes
 *          -Number of bytes to send to the device
 *
 * Returns: None
 *
 * Notes: True if packet is okay to send
 ********************************************************************/
bool HxUSBCommWorker::commUSB(uchar *inBuffer, uchar txBytes)
{
    mutex.lock();
    if(_commBusy)
    {
        #ifdef _COMM_DEBUG_ENABLED
        qDebug() << "Sending Too Many Packets";
        #endif
        mutex.unlock();
        return false;
    }
    memcpy(txBuffer, inBuffer, txBytes);
    numTxBytes = (DWORD)txBytes;
    _commFlag = true;
    mutex.unlock();
    return true;
}


/*********************************************************************
 * Function Name: checkPorts()
 *
 * Description: Signals the device to check the connected state
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: Usually this is called from a timeout
 ********************************************************************/
void HxUSBCommWorker::checkPorts()
{
    mutex.lock();
    _checkPorts = true;
    mutex.unlock();
}




/*******************************************************************************
 * -----------------------------------------------------------------------------
 * Active Thread Function
 * -----------------------------------------------------------------------------
 ******************************************************************************/

/*********************************************************************
 * Function Name: usbComm()
 *
 * Description: Main Control Handle for USB Comm Thread
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBCommWorker::usbComm()
{
    FT_STATUS ftstatus = FT_OTHER_ERROR;
    DWORD numDev;
    DWORD bytesWritten;
    DWORD bytesRead;
    uchar numBytes;
    uchar txCheckBuffer[3];
    uchar rxCheckBuffer[4];
    txCheckBuffer[0] = HX_PC_STATUS_REQUEST;
    txCheckBuffer[1] = 0x01;
    txCheckBuffer[2] = HxUtils::calcChecksum(txCheckBuffer, 2);

    #ifdef _COMM_DEBUG_ENABLED
    qDebug() << "Working";
    #endif
    while(!_abort) //******Device Control Thread Loop******************************************
    {
        //Update Flags
        mutex.lock();
        bool checkPorts = _checkPorts;
        bool commFlag = _commFlag;
        bool connectedState  = _connectedState;
        bool packetProcessing = _packetProcessing;
        mutex.unlock();
        if(commFlag) //******If Communication is Signaled**************************************
        {
            mutex.lock();
            _commBusy = true;
            _packetProcessing = true;
            mutex.unlock();
            if(connectedState)
            {
                //Purge the input/output buffers in the FT232 chip
                ftstatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
                if(ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Buffers Not Purged";
                    #endif
                    //Emit Error Signal
                    continue;
                }
                clearRxBuffer();
                //Check to see if the device is still connected
                //Send data packet to check if the device will respond properly
                if(numTxBytes)
                {                \
                        ftstatus = FT_Write(ftHandle, txBuffer, numTxBytes, &bytesWritten);
                        if (ftstatus != FT_OK){
                            #ifdef _COMM_DEBUG_ENABLED
                            qDebug() << "Failed to Write";
                            #endif
                            //Emit Error Signal
                            continue;
                        }

                        /******Uncomment To Display Bytes Written******/
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Bytes Written" << QString::number(bytesWritten);
                        for (unsigned int i = 0; i < bytesWritten; i++){
                            qDebug() << QString::number(txBuffer[i],16);
                        }
                        #endif
                        /******End of Debug Comment Section******/
                }
                //Read back packet and validate packet
                ftstatus = FT_Read(ftHandle, &rxBuffer[0], 1, &numBytesRead);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Failed to Read";
                    #endif
                    //Emit Error Signal
                    FT_Close(ftHandle);
                    continue;
                }
                //Read back packet and validate packet
                ftstatus = FT_Read(ftHandle, &rxBuffer[1], 1, &numBytesRead);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Failed to Read";
                    #endif
                    //Emit Error Signal
                    FT_Close(ftHandle);
                    continue;
                }
                //Read back packet and validate packet
                ftstatus = FT_Read(ftHandle, &rxBuffer[2], rxBuffer[1], &numBytesRead);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Failed to Read";
                    #endif
                    //Emit Error Signal
                    FT_Close(ftHandle);
                    continue;
                }
                numBytesRead = numBytesRead+2;

                /******Uncomment To Display Bytes Read******/
                #ifdef _COMM_DEBUG_ENABLED
                qDebug() << "Bytes Read" << QString::number(numBytesRead);
                for (unsigned int i = 0; i < numBytesRead; i++){
                    qDebug() << QString::number(rxBuffer[i],16);
                }
                #endif
                /******End of Debug Comment Section******/

                mutex.lock();
                _commFlag = false;
                _commBusy = false;
                mutex.unlock();
                emit usbCommPacketDone();
            }
            else
            {
                #ifdef _COMM_DEBUG_ENABLED
                qDebug() << "Not Connected";
                #endif
                mutex.lock();
                _commFlag = false;
                _commBusy = false;
                _packetProcessing = false;
                mutex.unlock();
            }
        }
        if(checkPorts) //******If Port Check is Signaled**************************************
        {
            if(packetProcessing)
            {
                mutex.lock();
                _checkPorts = false;
                mutex.unlock();
                continue;
            }
            if(!connectedState)
            {
                //Generate Port Info List for all FT devices
                ftstatus = FT_CreateDeviceInfoList(&numDev);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "No Devices Detected";
                    #endif
                    mutex.lock();
                    _connectedState = false;
                    _checkPorts = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    continue;
                }
                devNode = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDev);
                ftstatus = FT_GetDeviceInfoList(devNode, &numDev);
                #ifdef _COMM_DEBUG_ENABLED
                qDebug() << "Number of FT Devices" << QString::number(numDev);
                #endif

                //Open all devices in the device list and send a communication packet to check if they are a recognized device
                for(uchar i = 0; i < numDev; i++){

                    //Open the device and set the UART parameters
                    ftstatus = FT_Open(i, &ftHandle);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Failed to Open";
                        #endif
                        continue;
                    }
                    ftstatus = FT_SetBaudRate(ftHandle, 9600);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Baud Rate Not Set";
                        #endif
                        continue;
                    }
                    ftstatus = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Flow Control Not Set";
                        #endif
                        continue;
                    }
                    ftstatus = FT_SetTimeouts(ftHandle, 500, 500);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Timeouts Not Set";
                        #endif
                        continue;
                    }
                    ftstatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1,FT_PARITY_NONE);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Communication Characteristics Not Set";
                        #endif
                        continue;
                    }
                    //Purge the input/output buffers in the FT232 chip
                    ftstatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
                    if(ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Buffers Not Purged";
                        #endif
                        continue;
                    }

                    //Send data packet to check if the device will respond properly
                    ftstatus = FT_Write(ftHandle, txCheckBuffer, 3, &bytesWritten);
                    if (ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Failed to Write";
                        #endif
                        continue;
                    }

                    /******Uncomment To Display Bytes Written******/
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Bytes Written" << QString::number(bytesWritten);
                    for (uchar j = 0; j < bytesWritten; j++){
                        qDebug() << QString::number(txCheckBuffer[j],16);
                    }
                    #endif
                    /******End of Debug Comment Section******/

                    //Read back packet and validate packet
                    ftstatus = FT_Read(ftHandle, rxCheckBuffer, 4, &bytesRead);
                    if (ftstatus != FT_OK){
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Failed to Read";
                        #endif
                        continue;
                    }

                    /******Uncomment To Display Bytes Read******/
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Bytes Read" << QString::number(bytesRead);
                    for (unsigned int j = 0; j < bytesRead; j++){
                        qDebug() << QString::number(rxCheckBuffer[j],16);
                    }
                    #endif
                    /******End of Debug Comment Section******/

                    if ((bytesRead == 4) && (rxCheckBuffer[0] == HX_PC_STATUS_REQUEST)){
                        numBytes = rxCheckBuffer[1];
                        //Read everything but the checksum
                        for (i=0;i<(numBytes-1);i++){
                        //Check CommState
                        }
                        //Check the Checksum
                        if(HxUtils::calcChecksum(rxCheckBuffer, 4))
                        {
                            #ifdef _COMM_DEBUG_ENABLED
                            qDebug() << "Checksum Error";
                            #endif
                            continue;
                        }
                        mutex.lock();
                        _connectedState = true;
                        mutex.unlock();
                        emit sendConnectedState(true);
                        break;
                    }
                    else
                    {
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Invalid Status Check Packet";
                        #endif
                        FT_Close(ftHandle);
                    }
                }
            }
            else
            {
                //Purge the input/output buffers in the FT232 chip
                ftstatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);
                if(ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Buffers Not Purged";
                    #endif
                    mutex.lock();
                    _connectedState = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    FT_Close(ftHandle);
                    continue;
                }
                //Check to see if the device is still connected
                //Send data packet to check if the device will respond properly
                ftstatus = FT_Write(ftHandle, txCheckBuffer, 3, &bytesWritten);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Failed to Write";
                    #endif
                    mutex.lock();
                    _connectedState = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    FT_Close(ftHandle);
                    continue;
                }

                /******Uncomment To Display Bytes Written******/
                #ifdef _COMM_DEBUG_ENABLED
//                qDebug() << "Bytes Written" << QString::number(bytesWritten);
//                for (unsigned int i = 0; i < bytesWritten; i++){
//                    qDebug() << QString::number(txCheckBuffer[i],16);
//                }
                #endif
                /******End of Debug Comment Section******/

                //Read back packet and validate packet
                ftstatus = FT_Read(ftHandle, rxCheckBuffer, 4, &bytesRead);
                if (ftstatus != FT_OK){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Failed to Read";
                    #endif
                    mutex.lock();
                    _connectedState = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    FT_Close(ftHandle);
                    continue;
                }
                if(bytesRead != 4){
                    #ifdef _COMM_DEBUG_ENABLED
                    qDebug() << "Readback Error";
                    #endif
                    mutex.lock();
                    _connectedState = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    FT_Close(ftHandle);
                    continue;
                }

                /******Uncomment To Display Bytes Read******/
                #ifdef _COMM_DEBUG_ENABLED
//                qDebug() << "Bytes Read" << QString::number(bytesRead);
//                for (unsigned int i = 0; i < bytesRead; i++){
//                    qDebug() << QString::number(rxCheckBuffer[i],16);
//                }
                #endif
                /******End of Debug Comment Section******/

                if (rxCheckBuffer[0] == HX_PC_STATUS_REQUEST){
                    numBytes = rxCheckBuffer[2];
                    //Read everything but the checksum
                    for (uchar i=0;i<(numBytes-1);i++){
                    //Check CommState
                    }
                    //Check the Checksum
                    if(HxUtils::calcChecksum(rxCheckBuffer, 4))
                    {
                        #ifdef _COMM_DEBUG_ENABLED
                        qDebug() << "Readback Checksum Error";
                        #endif
                        mutex.lock();
                        _connectedState = false;
                        mutex.unlock();
                        emit sendConnectedState(false);
                        FT_Close(ftHandle);
                    }
                }
                else
                {
                    mutex.lock();
                    _connectedState = false;
                    mutex.unlock();
                    emit sendConnectedState(false);
                    FT_Close(ftHandle);
                }
            }
            mutex.lock();
            _checkPorts = false;
            connectedState = _connectedState;
            mutex.unlock();
            if(!connectedState)
            {
                free(devNode);
            }
        }
    }
    #ifdef _COMM_DEBUG_ENABLED
    qDebug() << "USB Comm Finished";
    #endif
    FT_Close(ftHandle);
    emit usbCommClosing();
}



/*******************************************************************************
 * -----------------------------------------------------------------------------
 * Private Functions
 * -----------------------------------------------------------------------------
 ******************************************************************************/

/*********************************************************************
 * Function Name: clearRxBuffer()
 *
 * Description: Clears the rxBuffer
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBCommWorker::clearRxBuffer()
{
    for(int i=0;i<64;i++)
    {
        rxBuffer[i] = 0;
    }
}
