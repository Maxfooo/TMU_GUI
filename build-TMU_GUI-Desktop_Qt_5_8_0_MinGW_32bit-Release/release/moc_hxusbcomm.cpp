/****************************************************************************
** Meta object code from reading C++ file 'hxusbcomm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../TMU_GUI/hxusbcomm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hxusbcomm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HxUSBComm_t {
    QByteArrayData data[14];
    char stringdata0[190];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HxUSBComm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HxUSBComm_t qt_meta_stringdata_HxUSBComm = {
    {
QT_MOC_LITERAL(0, 0, 9), // "HxUSBComm"
QT_MOC_LITERAL(1, 10, 15), // "updateDataReady"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "USB_READBACK_TYPE"
QT_MOC_LITERAL(4, 45, 12), // "readbackType"
QT_MOC_LITERAL(5, 58, 9), // "commError"
QT_MOC_LITERAL(6, 68, 11), // "errorString"
QT_MOC_LITERAL(7, 80, 8), // "commBusy"
QT_MOC_LITERAL(8, 89, 21), // "connectedStateChanged"
QT_MOC_LITERAL(9, 111, 14), // "connectedState"
QT_MOC_LITERAL(10, 126, 12), // "timerTimeout"
QT_MOC_LITERAL(11, 139, 14), // "updateReadback"
QT_MOC_LITERAL(12, 154, 17), // "retryTimerTimeout"
QT_MOC_LITERAL(13, 172, 17) // "getConnectedState"

    },
    "HxUSBComm\0updateDataReady\0\0USB_READBACK_TYPE\0"
    "readbackType\0commError\0errorString\0"
    "commBusy\0connectedStateChanged\0"
    "connectedState\0timerTimeout\0updateReadback\0"
    "retryTimerTimeout\0getConnectedState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HxUSBComm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       7,    0,   60,    2, 0x06 /* Public */,
       8,    1,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   64,    2, 0x08 /* Private */,
      11,    0,   65,    2, 0x08 /* Private */,
      12,    0,   66,    2, 0x08 /* Private */,
      13,    1,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void HxUSBComm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HxUSBComm *_t = static_cast<HxUSBComm *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDataReady((*reinterpret_cast< USB_READBACK_TYPE(*)>(_a[1]))); break;
        case 1: _t->commError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->commBusy(); break;
        case 3: _t->connectedStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->timerTimeout(); break;
        case 5: _t->updateReadback(); break;
        case 6: _t->retryTimerTimeout(); break;
        case 7: _t->getConnectedState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HxUSBComm::*_t)(USB_READBACK_TYPE );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBComm::updateDataReady)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HxUSBComm::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBComm::commError)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HxUSBComm::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBComm::commBusy)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (HxUSBComm::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HxUSBComm::connectedStateChanged)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject HxUSBComm::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HxUSBComm.data,
      qt_meta_data_HxUSBComm,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *HxUSBComm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HxUSBComm::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_HxUSBComm.stringdata0))
        return static_cast<void*>(const_cast< HxUSBComm*>(this));
    return QObject::qt_metacast(_clname);
}

int HxUSBComm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void HxUSBComm::updateDataReady(USB_READBACK_TYPE _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HxUSBComm::commError(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HxUSBComm::commBusy()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void HxUSBComm::connectedStateChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
