// This is a guide for implementing the SaveMyUI class

1)  Make a class pointer for SaveMyUI in mainwindow header file
// SaveMyUI* saveMyUI;

2)  Allocate the class pointer for SaveMyUI with the MainWindow instance
    pointer as the parameter and optional parameter of the xml file name
    in the mainwindow constructor
// saveMyUI = new SaveMyUI(this [,xmlFName]);

3)  Use method saveMyUI->saveToXML(); (usually in a button slot)

4)  Use method saveMyUI->loadFromXML(); (usually in a button slot)

/**************/
/** OPTIONAL **/
/**************/

There are methods in this class that will save your UI information to a platform
independent 'settings' file on your system.

This is useful if you want to keep your work in progress saved and loaded each time
you close and open you application. (You don't have to save/load to/from an XML file)

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

// ***** UNCOMMENT THESE LINES IN THE SaveMyUI.h FILE IF YOU ARE
// ***** USING INTERNAL SETTINGS FILE
// #define SAVE_SETTINGS
// #include <QSettings>
// extern const QString ORGANIZATION_NAME;
// extern const QString ORGANIZATION_DOMAIN;
// extern const QString APPLICATION_NAME;
// ===================================================================

5)  Use method saveMyUI->writeSettings(); ** ONLY IN MAINWINDOW DESTRUCTOR

6)  Use method saveMyUI->readSettings(); ** ONLY IN MAINWINDOW CONSTRUCTOR

* Working with this native 'settings' file outside of the constructor and destructor
* of you mainwindow class will slow your computer down significantly as it will be
* operated on automatically in the thread loop of mainwindow itself.
