/****************************************************************************
** Meta object code from reading C++ file 'QNtrack.h'
**
** Created: Mon Jan 28 21:56:57 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QNtrack.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QNtrack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNtrack[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x05,
      60,   42,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     107,  100,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QNtrack[] = {
    "QNtrack\0\0event\0monitorEvent(QNTrackEvent)\0"
    "oldState,newState\0"
    "stateChanged(QNTrackState,QNTrackState)\0"
    "socket\0socketActivated(int)\0"
};

void QNtrack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QNtrack *_t = static_cast<QNtrack *>(_o);
        switch (_id) {
        case 0: _t->monitorEvent((*reinterpret_cast< QNTrackEvent(*)>(_a[1]))); break;
        case 1: _t->stateChanged((*reinterpret_cast< QNTrackState(*)>(_a[1])),(*reinterpret_cast< QNTrackState(*)>(_a[2]))); break;
        case 2: _t->socketActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QNtrack::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QNtrack::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QNtrack,
      qt_meta_data_QNtrack, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QNtrack::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QNtrack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QNtrack::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNtrack))
        return static_cast<void*>(const_cast< QNtrack*>(this));
    return QObject::qt_metacast(_clname);
}

int QNtrack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QNtrack::monitorEvent(QNTrackEvent _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QNtrack::stateChanged(QNTrackState _t1, QNTrackState _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_QNtrackListener[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      46,   44,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QNtrackListener[] = {
    "QNtrackListener\0\0monitorEvent(QNTrackEvent)\0"
    ",\0stateChanged(QNTrackState,QNTrackState)\0"
};

void QNtrackListener::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QNtrackListener *_t = static_cast<QNtrackListener *>(_o);
        switch (_id) {
        case 0: _t->monitorEvent((*reinterpret_cast< QNTrackEvent(*)>(_a[1]))); break;
        case 1: _t->stateChanged((*reinterpret_cast< QNTrackState(*)>(_a[1])),(*reinterpret_cast< QNTrackState(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QNtrackListener::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QNtrackListener::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QNtrackListener,
      qt_meta_data_QNtrackListener, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QNtrackListener::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QNtrackListener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QNtrackListener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNtrackListener))
        return static_cast<void*>(const_cast< QNtrackListener*>(this));
    return QObject::qt_metacast(_clname);
}

int QNtrackListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
