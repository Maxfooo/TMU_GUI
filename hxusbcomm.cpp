#include "hxusbcomm.h"

/**********************************************************************************************************
 * --------------------------------------------------------------------------------------------------------
 * Startup/Closing Functions
 * --------------------------------------------------------------------------------------------------------
 **********************************************************************************************************/
HxUSBComm::HxUSBComm()
{
    _abort = false;
    _lastConnectedState = false;
    setCommState(USB_NULL);
    resendState = DO_NOTHING;
    packetCmd = HX_UC_NACK;
    usbThread = new QThread;
    hxUSBCommWorker = new HxUSBCommWorker;
    portCheckTimer = new QTimer;
    retryCommTimer = new QTimer;

    hxUSBCommWorker->moveToThread(usbThread);
    connect(hxUSBCommWorker, SIGNAL(usbCommRequested()), usbThread, SLOT(start()));
    connect(usbThread, SIGNAL(started()), hxUSBCommWorker, SLOT(usbComm()));
    connect(hxUSBCommWorker, SIGNAL(usbCommClosing()), usbThread, SLOT(quit()), Qt::DirectConnection);
    connect(portCheckTimer,SIGNAL(timeout()),this,SLOT(timerTimeout()));
    connect(hxUSBCommWorker, SIGNAL(usbCommPacketDone()), this, SLOT(updateReadback()));
    connect(retryCommTimer,SIGNAL(timeout()),this,SLOT(retryTimerTimeout()));
    connect(hxUSBCommWorker, SIGNAL(sendConnectedState(bool)), this, SLOT(getConnectedState(bool)));

    hxUSBCommWorker->requestUSBComm();
    portCheckTimer->setInterval(500);
    portCheckTimer->setSingleShot(true);
    portCheckTimer->start();
}

HxUSBComm::~HxUSBComm()
{
    hxUSBCommWorker->abort();
    usbThread->wait();
    portCheckTimer->stop();
    delete usbThread;
    delete hxUSBCommWorker;
    delete portCheckTimer;
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
void HxUSBComm::abort()
{
    _abort = true;
}


/**********************************************************************************************************
 * --------------------------------------------------------------------------------------------------------
 * User API Functions
 * --------------------------------------------------------------------------------------------------------
 **********************************************************************************************************/

/*********************************************************************
 * Function Name: writeDirect(uchar *inBuffer, uchar txBytes)
 *
 * Description: Directly Writes data to the USB Line
 *
 * Parameters:
 *      uchar *inBuffer
 *          -Start of Array with data to send to the device
 *      uchar txBytes
 *          -Number of Bytes to send to the device
 *
 * Returns: None
 *
 * Notes: After this operation completes sucessfully, updateDirectReady()
 *      will be emitted and the will be prepared to return all bytes
 *      recieved from the USB Line.
 ********************************************************************/
void HxUSBComm::writeDirect(uchar *inBuffer, uchar txBytes)
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_DIRECT);
        resetTimer();
        numBytesSend = txBytes;
        memcpy(inputBuffer,inBuffer, txBytes);
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}


/*********************************************************************
 * Function Name: writeData(uchar cmdByte, uchar *inBuffer, uchar packetBytes)
 *
 * Description: Writes an Inner packet to the USB line and handles
 *      all layers of outer packet communication
 *
 * Parameters:
 *      uchar cmdByte
 *          -Inner Packet Command Byte that signals the UC to perform
 *              a specific operation
 *      uchar *inBuffer
 *          -Start of Array with inner data to send to the device
 *              after it sends the cmdByte
 *      uchar txBytes
 *          -Number of inBuffer[] Bytes to send to the device
 *
 * Returns: None
 *
 * Notes: After this operation completes sucessfully, updateDataReady()
 *      will be emitted and the will be prepared to return the inner
 *      packet received across the USB Line (if there is one).
 ********************************************************************/
void HxUSBComm::writeData(uchar cmdByte, uchar *inBuffer, uchar packetBytes)
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        packetCmd = cmdByte;
        setCommState(USB_DATA);
        resetTimer();
        inputBuffer[0] = HX_PC_DATA_WRITE;
        inputBuffer[1] = packetBytes+2;
        inputBuffer[2] = packetCmd;
        memcpy(&inputBuffer[3], inBuffer, packetBytes);
        inputBuffer[packetBytes+3] = HxUtils::calcChecksum(inputBuffer, packetBytes+3);
        numBytesSend = packetBytes+4;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::flushInputBuffer()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_FLUSH_INPUT);
        resetTimer();
        inputBuffer[0] = HX_PC_FLUSH_INPUT_BUFFER;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::flushOutputBuffer()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_FLUSH_OUTPUT);
        resetTimer();
        inputBuffer[0] = HX_PC_FLUSH_OUTPUT_BUFFER;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::requestChipIDandRevision()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_CHIP_INFO);
        resetTimer();
        inputBuffer[0] = HX_PC_REQUEST_CHIP_ID;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::requestInputBuffer()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_INPUT_BUFFER);
        resetTimer();
        inputBuffer[0] = HX_PC_REQUEST_INPUT_BUFFER;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::requestDebugVariable()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_DEBUG_VAR);
        resetTimer();
        inputBuffer[0] = HX_PC_REQUEST_DEBUG_VAR;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}

void HxUSBComm::requestPreviousPacket()
{
    if((commState == USB_NULL) || (commState == USB_ERROR))
    {
        setCommState(USB_LAST_PACKET);
        resetTimer();
        inputBuffer[0] = HX_PC_REQUEST_PREVIOUS_PKT;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
    }
    else
    {
        emit commBusy();
    }
}


/*********************************************************************
 * Function Name: getReadbackDirect(uchar *outBuffer, uchar *numBytes)
 *
 * Description: Returns the full packet received from the USB Line
 *
 * Parameters:
 *      uchar *outBuffer
 *          -Used for return
 *      uchar *numBytes
 *          -Used for return
 *
 * Returns:
 *      Number of Bytes read at *numBytes
 *      Readback Bytes into the array starting at *outBuffer
 *
 * Notes: Should only be called after updateDataReady() is emitted
 ********************************************************************/
void HxUSBComm::getReadbackDirect(uchar *outBuffer, uchar *numBytes)
{
    resetTimer();
    hxUSBCommWorker->getCommReadback(outBuffer, numBytes);
}


/*********************************************************************
 * Function Name: getReadbackData(uchar *outBuffer, uchar *numBytes)
 *
 * Description: Returns the recieved inner packet from a series of
 *      complete USB communication events
 *
 * Parameters:
 *      uchar *outBuffer
 *          -Used for return
 *      uchar *numBytes
 *          -Used for return
 *
 * Returns:
 *      The first byte of inner packet as a status return value
 *      Number of inner packet bytes read at *numBytes
 *      Inner Packet Readback Bytes into the array starting at *outBuffer
 *
 * Notes: Should only be called after updateDataReady() is emitted
 ********************************************************************/
uchar HxUSBComm::getReadbackData(uchar *cmdByte, uchar *outBuffer, uchar *packetBytes)
{
    uchar returnValue = HX_DUT_UNDEFINED;
    resetTimer();
    uchar outBufferTemp[64];
    uchar tempNumBytes;
    hxUSBCommWorker->getCommReadback(outBufferTemp, &tempNumBytes);
    *cmdByte = packetCmd;
    if(tempNumBytes > 3)
    {
        returnValue = outBufferTemp[2];
        *packetBytes = tempNumBytes - 4;
        memcpy(outBuffer, &outBufferTemp[3], *packetBytes);
    }
    else
    {
        *packetBytes = 0;
    }
    return returnValue;
}

/*********************************************************************
 * Function Name: getReadbackInnerPacket(uchar *outBuffer, uchar *numBytes)
 *
 * Description: Returns the recieved inner packet
 *
 * Parameters:
 *      uchar *outBuffer
 *          -Used for return
 *      uchar *numBytes
 *          -Used for return
 *
 * Returns:
 *      Number of inner packet bytes read at *numBytes
 *      Inner Packet Readback Bytes into the array starting at *outBuffer
 *
 * Notes: Should only be called after updateDataReady() is emitted
 ********************************************************************/
void HxUSBComm::getReadbackInnerPacket(uchar *outBuffer, uchar *packetBytes)
{
    resetTimer();
    uchar outBufferTemp[64];
    uchar tempNumBytes;
    hxUSBCommWorker->getCommReadback(outBufferTemp, &tempNumBytes);
    if(tempNumBytes > 3)
    {
        *packetBytes = tempNumBytes - 3;
        memcpy(outBuffer, &outBufferTemp[2], *packetBytes);
    }
    else
    {
        *packetBytes = 0;
    }
}

/**********************************************************************************************************
 * --------------------------------------------------------------------------------------------------------
 * Readback Interface
 * --------------------------------------------------------------------------------------------------------
 **********************************************************************************************************/

/*********************************************************************
 * Function Name: updateReadback()
 *
 * Description: State Machine Control function to control what
 *      to do with the readback data.
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: This function gets called whenever the USB Comm Worker
 *      emits the signal that data is ready to read.  This function
 *      should emit a signal to tell the GUI what type of readback
 *      data to get.
 ********************************************************************/
void HxUSBComm::updateReadback()
{
    uchar outBufferTemp[64];
    uchar numBytesTemp;
    uchar statusReturn;

    resetTimer();
    switch(commState){
    //*************USB DATA READBACK STATES****************************************************
    //*************USB DATA READBACK STATES****************************************************
    case USB_DATA:      //Handles Readback Values for HX_PC_DATA_WRITE
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_DATA_WRITE, outBufferTemp, numBytesTemp);
        if(statusReturn == HX_UC_ACK)
        {
            setCommState(USB_DATA_START);
            inputBuffer[0] = HX_PC_INITIATE_MAIN_CODE;
            inputBuffer[1] = 0x01;
            inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
            numBytesSend = 3;
            hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
            break;
        }
        else if(((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR) || ((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR))
        {
            startRetry(statusReturn);
            break;
        }
        else
        {
            setCommState(USB_ERROR);
            emit commError(errorString);
            break;
        }
    case USB_DATA_START:      //Add Readback Signal for the commState
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_INITIATE_MAIN_CODE, outBufferTemp, numBytesTemp);
        if(statusReturn == HX_UC_ACK)
        {
            setCommState(USB_DATA_BUSY_STATUS_READBACK);
            inputBuffer[0] = HX_PC_STATUS_REQUEST;
            inputBuffer[1] = 0x01;
            inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
            numBytesSend = 3;
            hxUSBCommWorker->commUSB(inputBuffer, 3);
            break;
        }
        else if(((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR) || ((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR))
        {
            startRetry(statusReturn);
            break;
        }
        else
        {
            setCommState(USB_ERROR);
            emit commError(errorString);
            break;
        }
    case USB_DATA_BUSY_STATUS_READBACK:      //Add Readback Signal for the commState
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_STATUS_REQUEST, outBufferTemp, numBytesTemp);
        if(statusReturn == HX_UC_BUSY)
        {
            setCommState(USB_DATA_BUSY_STATUS_READBACK); //---------------CHECK MIGHT LEAD TO STALLING--------------------------
            startRetryTimer(BUSY);
            break;
        }
        else if(statusReturn == HX_UC_COMPLETE)
        {
            setCommState(USB_DATA_READBACK);
            inputBuffer[0] = HX_PC_ACK_COMPLETE;
            inputBuffer[1] = 0x01;
            inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
            numBytesSend = 3;
            hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
            break;
        }
        else if(((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR) || ((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR))
        {
            startRetry(statusReturn);
            break;
        }
        else if((statusReturn & HX_STATUS_MASK) == HX_UC_TO_DUT_ERROR)
        {
            errorString = "Error: Communication from UC to DUT failed";
            setCommState(USB_ERROR);
            emit commError(errorString);
            break;
        }
        else
        {
            setCommState(USB_ERROR);
            emit commError(errorString);
            break;
        }
    case USB_DATA_READBACK:     //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_ACK_COMPLETE, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_DATA);
        break;
    //*************USB MISC READBACK STATES****************************************************
    //*************USB MISC READBACK STATES****************************************************
    case USB_DIRECT:      //Handles Readback Values for a direct packet write
        emit updateDataReady(READBACK_DIRECT);
        setCommState(USB_NULL);
        break;
    case USB_CHIP_INFO:         //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_REQUEST_CHIP_ID, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_CHIP_INFO);
        break;
    case USB_STATUS_READBACK:   //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_STATUS_REQUEST, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_STATUS);
        break;
    case USB_LAST_PACKET:       //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_REQUEST_PREVIOUS_PKT, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_LAST_PACKET);
        break;
    case USB_INPUT_BUFFER:      //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_REQUEST_INPUT_BUFFER, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_INPUT_BUFFER);
        break;
    case USB_DEBUG_VAR:         //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_REQUEST_DEBUG_VAR, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_DEBUG_VAR);
        break;
    case USB_FLUSH_OUTPUT:      //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_FLUSH_OUTPUT_BUFFER, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_FLUSH_OUTPUT);
        break;
    case USB_FLUSH_INPUT:       //-------------------------------------------------------------------------
        getReadbackDirect(outBufferTemp, &numBytesTemp);
        statusReturn = statusReadback(HX_PC_FLUSH_INPUT_BUFFER, outBufferTemp, numBytesTemp);
        handleStatusReturn(statusReturn, READBACK_FLUSH_INPUT);
        break;

    //*************USB ERROR READBACK STATES****************************************************
    //*************USB ERROR READBACK STATES****************************************************
    case USB_NULL:  //Error catching state
        setCommState(USB_NULL);
        errorString = "Error: No Data Available";
        emit commError(errorString);
        break;
    default:
        setCommState(USB_ERROR);    //Error catching state
        errorString = "Error: Data Returned in Invalid State";
        emit commError(errorString);
        break;
    }
}


/*********************************************************************
 * Function Name: handleStatusReturn(uchar statusReadback, USB_READBACK_TYPE readbackType)
 *
 * Description: handles status returns for values that emit signals
 *
 * Parameters:
 *      uchar statusReadback
 *          -status return value from statusReadback command
 *      uchar *outBuffer
 *          -start of readback data array
 *      uchar numBytes
 *          -number of bytes read back from device
 *
 * Returns: uchar
 *          -either: Status byte returned from the device or status
 *                  byte indicating improper returned packet
 *
 * Notes: None
 ********************************************************************/

void HxUSBComm::handleStatusReturn(uchar statusReadback, USB_READBACK_TYPE readbackType)
{
    if(statusReadback == HX_UC_ACK)
    {
        setCommState(USB_NULL);
        emit updateDataReady(readbackType);
    }
    else if(((statusReadback & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR) || ((statusReadback & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR))
    {
        startRetry(statusReadback);
    }
    else
    {
        setCommState(USB_ERROR);
        emit commError(errorString);
    }
}

/*********************************************************************
 * Function Name: statusReadback(uchar cmdByte, uchar *outBuffer, uchar numBytes)
 *
 * Description: Verifies a general USB packet
 *
 * Parameters:
 *      uchar cmdByte
 *          -Outer Packet command byte send that prompted the readback
 *      uchar *outBuffer
 *          -start of readback data array
 *      uchar numBytes
 *          -number of bytes read back from device
 *
 * Returns: uchar
 *          -either: HX_UC_ACK if verified or status
 *                  byte indicating improper returned packet
 *
 * Notes: None
 ********************************************************************/
uchar HxUSBComm::statusReadback(uchar cmdByte, uchar *outBuffer, uchar numBytes)
{
    if(numBytes == 0)
    {
        errorString = "Error: Incorrect Number of Bytes Read";
        return HX_PC_ERROR_UC_COMM;
    }
    else if(outBuffer[0] != cmdByte)
    {
        errorString = "Error: Incorrect Command Byte Received";
        return HX_PC_ERROR_UC_COMM;
    }
    else if(outBuffer[1] != (numBytes - 2))
    {
        errorString = "Error: Incorrect Number of Bytes";
        return HX_PC_ERROR_UC_COMM;
    }
    else if(HxUtils::calcChecksum(outBuffer, numBytes))
    {
        errorString = "Error: Invalid Checksum";
        return HX_PC_ERROR_UC_CHECKSUM;
    }
    else
    {
        errorString = "OK";
        if(cmdByte == HX_PC_STATUS_REQUEST) //For Status Readback, return the status btye
        {
            if(outBuffer[1] != 2)
            {
                errorString = "Error: Incorrect Number of Bytes";
                return HX_PC_ERROR_UC_COMM;
            }
            else
            {
                return outBuffer[2];
            }
        }
        else
        {
            return HX_UC_ACK;   //For general readback return HX_UC_ACK
        }
    }
}

/*********************************************************************
 * Function Name: startRetry(uchar statusReturn)
 *
 * Description: Starts the retry operation
 *
 * Parameters:
 *      uchar statusReturn
 *          -status byte returned from a verification function that
 *
 * Returns:
 *
 * Notes: Will either start the retry timer with the appropriate operation
 *      or will emit an error when the number of retries in exeeded
 ********************************************************************/
void HxUSBComm::startRetry(uchar statusReturn)
{
    if((statusReturn & HX_STATUS_MASK) == HX_PC_TO_UC_ERROR)
    {
        numRetries++;
        if(numRetries > retryLimit)
        {
            setCommState(USB_ERROR);
            errorString = "Error: Retry Limit Reached";
            emit commError(errorString);
        }
        else
        {
            startRetryTimer(RESEND);
        }
    }
    else if((statusReturn & HX_STATUS_MASK) == HX_UC_TO_PC_ERROR)
    {
        numRetries++;
        if(numRetries > retryLimit)
        {
            setCommState(USB_ERROR);
            errorString = "Error: Retry Limit Reached";
            emit commError(errorString);
        }
        else
        {
            startRetryTimer(REREAD);
        }
    }
}


/*********************************************************************
 * Function Name: setCommState(USB_COMM_STATE usbState)
 *
 * Description: Sets USB state and resets retry information
 *
 * Parameters: USB_COMM_STATE usbState
 *
 * Returns:
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::setCommState(USB_COMM_STATE usbState)
{
    numRetries = 0;
    commState = usbState;
    retryLimit = 6;
}


/*********************************************************************
 * Function Name: getConnectedState(bool connectedState)
 *
 * Description: Tells the Main Window when the connected state has
 *      been changed and passes the connected state to the Main Window
 *
 * Parameters: bool connectedState
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::getConnectedState(bool connectedState)
{
    if (connectedState != _lastConnectedState)
    {
        _lastConnectedState = connectedState;
        emit connectedStateChanged(connectedState);
    }
}

/**********************************************************************************************************
 * --------------------------------------------------------------------------------------------------------
 * Timer Interface
 * --------------------------------------------------------------------------------------------------------
 **********************************************************************************************************/

/*********************************************************************
 * Function Name: resetTimer()
 *
 * Description: Resets the Port Check Timer
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::resetTimer()
{
    portCheckTimer->stop();
    portCheckTimer->setInterval(2000);
    portCheckTimer->setSingleShot(true);
    portCheckTimer->start();
}


/*********************************************************************
 * Function Name: timerTimeout()
 *
 * Description: Signals the Comm Worker to check the ports then
 *  resets the timer depending on the connected state of the device
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::timerTimeout()
{
    hxUSBCommWorker->checkPorts();
    if(hxUSBCommWorker->getConnectedState())
    {
        portCheckTimer->setInterval(2000);
        portCheckTimer->start();
    }
    else
    {
        portCheckTimer->setInterval(500);
        portCheckTimer->start();
    }
}


/*********************************************************************
 * Function Name: startRetryTimer(RESENT_STATE retryState)
 *
 * Description: Starts a delay timer that signals the communication
 *      state to perform a specific operation
 *
 * Parameters: retryState
 *              -Signals what operation to perform at timeout
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::startRetryTimer(RESEND_STATE retryState)
{
    resendState = retryState;
    retryCommTimer->setInterval(1000);
    retryCommTimer->setSingleShot(true);
    retryCommTimer->start();
}


/*********************************************************************
 * Function Name: retryTimerTimeout()
 *
 * Description: Performs the retry operation specificed when the
 *      retry timer was set up.
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes: None
 ********************************************************************/
void HxUSBComm::retryTimerTimeout()
{
    uchar requestPreviousPacketBuffer[3] = {HX_PC_REQUEST_PREVIOUS_PKT, 0x01, 0xf6};
    switch(resendState)
    {
    case RESEND:
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);        //Resend Last Packet
        break;

    case REREAD:
        hxUSBCommWorker->commUSB(requestPreviousPacketBuffer, 3);   //Request Last Packet
        break;

    case BUSY:
        inputBuffer[0] = HX_PC_STATUS_REQUEST;
        inputBuffer[1] = 0x01;
        inputBuffer[2] = HxUtils::calcChecksum(inputBuffer, 2);
        numBytesSend = 3;
        hxUSBCommWorker->commUSB(inputBuffer, numBytesSend);
        break;

    case DO_NOTHING:
        break;
    }
    resendState = DO_NOTHING;
}
