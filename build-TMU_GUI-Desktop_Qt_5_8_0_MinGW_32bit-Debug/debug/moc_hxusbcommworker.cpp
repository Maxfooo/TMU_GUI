/****************************************************************************
** Meta object code from reading C++ file 'hxusbcommworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../TMU_GUI/hxusbcommworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hxusbcommworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HxUSBCommWorker_t {
    QByteArrayData data[8];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HxUSBCommWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HxUSBCommWorker_t qt_meta_stringdata_HxUSBCommWorker = {
    {
QT_MOC_LITERAL(0, 0, 15), // "HxUSBCommWorker"
QT_MOC_LITERAL(1, 16, 16), // "usbCommRequested"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 14), // "usbCommClosing"
QT_MOC_LITERAL(4, 49, 17), // "usbCommPacketDone"
QT_MOC_LITERAL(5, 67, 18), // "sendConnectedState"
QT_MOC_LITERAL(6, 86, 14), // "connectedState"
QT_MOC_LITERAL(7, 101, 7) // "usbComm"

    },
    "HxUSBCommWorker\0usbCommRequested\0\0"
    "usbCommClosing\0usbCommPacketDone\0"
    "sendConnectedState\0connectedState\0"
    "usbComm"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HxUSBCommWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void HxUSBCommWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HxUSBCommWorker *_t = static_cast<HxUSBCommWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->usbCommRequested(); break;
        case 1: _t->usbCommClosing(); break;
        case 2: _t->usbCommPacketDone(); break;
        case 3: _t->sendConnectedState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->usbComm(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HxUSBCommWorker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBCommWorker::usbCommRequested)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HxUSBCommWorker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBCommWorker::usbCommClosing)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HxUSBCommWorker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBCommWorker::usbCommPacketDone)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (HxUSBCommWorker::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBCommWorker::sendConnectedState)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject HxUSBCommWorker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HxUSBCommWorker.data,
      qt_meta_data_HxUSBCommWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *HxUSBCommWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HxUSBCommWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_HxUSBCommWorker.stringdata0))
        return static_cast<void*>(const_cast< HxUSBCommWorker*>(this));
    return QObject::qt_metacast(_clname);
}

int HxUSBCommWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void HxUSBCommWorker::usbCommRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void HxUSBCommWorker::usbCommClosing()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void HxUSBCommWorker::usbCommPacketDone()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void HxUSBCommWorker::sendConnectedState(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
