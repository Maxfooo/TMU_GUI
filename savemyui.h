#ifndef SAVEMYUI_H
#define SAVEMYUI_H

#include <QDebug>
#include <QFile>
#include <QtXml>
#include <QString>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidget>
#include <QComboBox>



// ===================================================================
// IF YOU ARE USING THE SaveMyUI::writeSettings() and/or
// SaveMyUI::readSettings() METHODS, YOU MUST HAVE THE FOLLOWING CODE
// -------------------------------------------------------------------
// ***** ADD THESE GLOBALS IN MAIN.CPP
// ***** UPDATE AS NECESSARY
// #include <QString>
// const QString ORGANIZATION_NAME = "MyCompanyName";
// const QString ORGANIZATION_DOMAIN = "http://MyCompanyName.com"; // Not necessary
// const QString APPLICATION_NAME = "MyAppsName";

// ***** ADD IN MAIN.CPP - MAIN FUNCTION - UNDER THE QApplication INSTANCE
// a.setOrganizationName(ORGANIZATION_NAME);
// a.setApplicationName(APPLICATION_NAME);
// a.setOrganizationDomain(ORGANIZATION_DOMAIN); // Not Necessary

// ***** UNCOMMENT THESE LINES IF YOU ARE USING INTERNAL SETTINGS FILE
// #define SAVE_SETTINGS
// #include <QSettings>
// extern const QString ORGANIZATION_NAME;
// extern const QString ORGANIZATION_DOMAIN;
// extern const QString APPLICATION_NAME;
// ===================================================================


class SaveMyUI
{
public:
    SaveMyUI(QMainWindow* parent);
    SaveMyUI(QMainWindow* parent, QString xmlFName);
    ~SaveMyUI();

    void setFileName(QString xmlFName) {xmlFileName = xmlFName;}
    void setFileName();
    QString getFileName() {return xmlFileName;}
    void saveToXML(const QString& fileName);
    void saveToXML();
    void loadFromXML(const QString& fileName);
    void loadFromXML();


    QSettings::Status readSettings();
    QSettings::Status writeSettings();


private:

    QMainWindow* parent;

    // Explicitely forcing the names to the meta objects names so that
    // we can save the widgets in the gui in groups under these names
    const QString Q_RAD_BTN = QRadioButton::staticMetaObject.className();
    const QString Q_CHK_BOX = QCheckBox::staticMetaObject.className();
    const QString Q_LINE_EDIT = QLineEdit::staticMetaObject.className();
    const QString Q_LABEL = QLabel::staticMetaObject.className();
    const QString Q_TABLE = QTableWidget::staticMetaObject.className();
    const QString Q_CMBO_BOX = QComboBox::staticMetaObject.className();

    // Change appropriately
    const QString XML_ROOT = "ROOT";

    // This is the default save file, normal operation will have the user select
    // what the xml file name will be.
    const QString XML_FILE_NAME = QDir::currentPath() + "/mySettings.xml";
    QString xmlFileName = "";

    QMessageBox msgBox;

    #ifdef SAVE_SETTINGS
        // Platform independent settings file (stored deep in your computers internal files)
        QSettings* mySettings;

        // Use only if you have already setup and saved to the
        // internal 'settings' file (mySettings)
        void printMySettings();
    #endif

};

#endif // SAVEMYUI_H
