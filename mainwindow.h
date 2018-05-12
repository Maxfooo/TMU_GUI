#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QInputDialog>
#include "tmu.h"
#include "hxusbcomm.h"
#include "hx_commstd.h"
#include "savemyui.h"
#include "processrunningle.h"
#include "tmudaemon.h"
#include "tmutempcycle.h"

#define CONFIG_FILE_NAMES_DELIMETER "$"
#define NUM_OF_TEMP_CYCLE_TMUS 9

extern QString CONFIG_FILE_PREFIX;

typedef enum CommState {
    COMM_IDLE,

    COMM_DETECT_DUT_RD,

    COMM_TX_REG_FROM_INTERFACE,
    COMM_RX_REG_TO_INTERFACE,
    COMM_TX_OTP_FROM_INTERFACE,
    COMM_RX_OTP_TO_INTERFACE,

    COMM_TX_REG_FROM_FILE,
    COMM_RX_REG_TO_FILE,
    COMM_TX_OTP_FROM_FILE,
    COMM_RX_OTP_TO_FILE,

    COMM_RX_MUX
};

enum {
    TMU_LOW_LEFT,
    TMU_LOW_MID,
    TMU_LOW_RIGHT,
    TMU_MID_LEFT,
    TMU_MID_MID,
    TMU_MID_RIGHT,
    TMU_UP_LEFT,
    TMU_UP_MID,
    TMU_UP_RIGHT
};

const uchar PRIMARY_TMU_ID = TMU_MID_MID;

typedef enum TempCycleState{
    TEMP_CYCLE_STOP,
    TEMP_CYCLE_START,
    TEMP_CYCLE_PAUSE,
    TEMP_CYCLE_RESUME
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_8_clicked();

    void on_radioButton_9_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_comboBox_3_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_radioButton_10_clicked();

    void on_infoButton_HeaterOptions_clicked();

    void on_infoButton_InternalHeater_clicked();

    void on_infoButton_ExternalHeater_clicked();

    void on_infoButton_SetTemperature_clicked();

    void on_infoButton_ReferenceVoltage_clicked();

    void on_infoButton_LoadToRegister_clicked();

    void on_infoButton_ProgramOTP_clicked();

    void on_infoButton_Multiplexor_clicked();

    void on_lineEdit_6_editingFinished();

    void on_lineEdit_9_editingFinished();

    void on_lineEdit_7_editingFinished();

    void on_lineEdit_8_editingFinished();

    void on_lineEdit_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_comboBox_4_currentIndexChanged(int index);

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_lineEdit_15_editingFinished();

    void on_lineEdit_16_editingFinished();

    void on_infoButton_HexConfigurations_clicked();

    void on_infoButton_GenerateFromInterface_clicked();

    void on_infoButton_ImportFromHexFile_clicked();

    void on_infoButton_ExportHexFile_clicked();

    void on_infoButton_ThermalParameters_clicked();

    void on_lineEdit_23_editingFinished();

    void on_lineEdit_24_editingFinished();

    void on_lineEdit_25_editingFinished();

    void updateDataInfo();

    void on_actionRefresh_Micro_triggered();

    void on_lineEdit_3_editingFinished();

    void on_lineEdit_4_editingFinished();

    void on_lineEdit_5_editingFinished();

    void on_comboBox_6_currentIndexChanged(int index);

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_radioButton_5_clicked();
    
    void on_radioButton_6_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

public slots:
    void updateDirectGUI();
    void updateDataGUI(USB_READBACK_TYPE);
    void updateConnectedState(bool);
    void reportError(QString);
    void reportBusy();

private:
    Ui::MainWindow *ui;

    SaveMyUI* saveMyUI;

    QString mySettings = "mySettings.xml";
    QString CONFIG_FILE_NAMES_FILE_NAME = "ConfigFileNames.txt";
    QString TEMP_CYCLE_PROFILE_FILE_NAME = "TempCycleProfiles.txt";

    QRegExpValidator *hexFileValidator;
    QRegExpValidator* hexValidator;
    QRegExpValidator* hex2ByteValidator;
    QRegExpValidator* hexByteValidator;
    QRegExpValidator* decValidator;
    QRegExpValidator* floatValidator;

    ProcessRunningLE* procRunLE;
    QString procRunLEName;
    double procRunPeriod = 1;


    //USB Comm Objects and Variables
    HxUSBComm *hxUSBComm;
    uchar txBuffer[64];
    uchar txIndex = 0;
    uchar rxBuffer[64];
    uchar rxIndex = 0;
    CommState commState = COMM_IDLE;
    TmuTxPkt tmuTxPkt;
    TmuRxPkt tmuRxPkt;
    TmuTxPkt readbackPkt;
    uchar tmu_index;

    // Comm functions
    void handleI2CWrite(uchar* data, uchar numOfTx, uchar numOfRx);
    void writeRegFromInterface();
    void readRegToInterface();
    void fillReadbackPacket();
    void detectTMUWrite();
    void detectTMURead();
    void TMUDetected(bool d);
    void OTPProgrammed(bool o);
    void debugReadback();
    void refreshMicro();

    //TMU object
    TMU* tmu;
    TMUDaemon* tmuDaemon;
    TMUTempCycle* tmuTempCycle[NUM_OF_TEMP_CYCLE_TMUS];
    TempCycleState tempCycleState = TEMP_CYCLE_STOP;

    void initGeneralConfigTab();
    void initTempCycleTab();
    void initTempCycleTMUs();
    void initAdvancedTab();
    void updateAdvancedTab();
    void post(QString s);
    void enableIHBox();
    void enableEHBox(bool nmosOrNpn);
    void exportHex();
    void importHex();
    void updateGeneralTab();

    void saveConfigFileName(QString fname);
    void removeConfigFileName(QString fname);
    void loadConfigFileNames();

    void loadTempCycleCB();
    void loadTempCycleTab(TMUTempCycle* ttc);
    QString saveTempCycleProfileFileName(QString fname);

    QString info_HeaterOptions  = "<html>"
                                  "<body>"
                                  "<h1>Heater Options</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_InternalHeater = "<html>"
                                  "<body>"
                                  "<h1>Internal Heater</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_ExternalHeater = "<html>"
                                  "<body>"
                                  "<h1>External Heater</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_SetTemperature = "<html>"
                                  "<body>"
                                  "<h1>Set Temperature</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_ReferenceVoltage = "<html>"
                                  "<body>"
                                  "<h1>Reference Voltage</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_LoadToRegisters = "<html>"
                                  "<body>"
                                  "<h1>Load To Registers</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_ProgramOTP     = "<html>"
                                  "<body>"
                                  "<h1>Program OTP</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_Multiplexor    = "<html>"
                                  "<body>"
                                  "<h1>Multiplexor</h1>"
                                  "<p></p>"
                                  "<p></p>"
                                  "<p></p>"
                                  "</body>"
                                  "</html>";

    QString info_hexConfigurations = "<html>"
                                     "<body>"
                                     "<h1>HEX Configurations</h1>"
                                     "<p>To ease development, configurations of the part can loaded, saved and deleted. Up to 10 configurations can be stored along with user generated notes. Register contents are displayed in HEX code.</p>"
                                     "<p>Register configurations can be imported multiple ways:</p>"
                                     "<ol>"
                                     "<li>Importing from the selections in the Configuration and Parameter Tuning tabs by pressing the ‘Generate from Interface’ button.</li>"
                                     "<li>Direct HEX input into the register field boxes.</li>"
                                     "<li>Importing a separate HEX file by pressing the ‘Import HEX File’ button.</li>"
                                     "</ol>"
                                     "<p>Register configurations may also be exported by pressing the ‘Export HEX File’ button.</p>"
                                     "</body>"
                                     "</html>";

    QString info_genFromInterface = "<html>"
                                    "<body>"
                                    "<h1>Generate from Interface</h1>"
                                    "<p>Placeholder Notes.</p>"
                                    "</body>"
                                    "</html>";

    QString info_exportHex =        "<html>"
                                    "<body>"
                                    "<h1>Export Hex File</h1>"
                                    "<p>Placeholder Notes.</p>"
                                    "</body>"
                                    "</html>";

    QString info_importHex =        "<html>"
                                    "<body>"
                                    "<h1>Import Hex File</h1>"
                                    "<p>Placeholder Notes.</p>"
                                    "</body>"
                                    "</html>";

    QString info_thermalParameters = "<html>"
                                     "<body>"
                                     "<h1>Thermal Parameters</h1>"
                                     "<p></p>"
                                     "<p></p>"
                                     "<p></p>"
                                     "</body>"
                                     "</html>";

};

#endif // MAINWINDOW_H
