/* -- THIS FILE IS GENERATED - DO NOT EDIT *//* -*- Mode: C; c-basic-offset: 4 -*- */

#include <Python.h>



#line 3 "../../gobject/pyntrack-gmonitor.override"
#include <Python.h>               
#include "pygobject.h"
#include "ntrack-gobject.h"
#include "ntrack-enumtypes.h"
#include "ntrack-gsource.h"
#line 14 "pyntrack-gmonitor.c"


/* ---------- types from other modules ---------- */
static PyTypeObject *_PyGObject_Type;
#define PyGObject_Type (*_PyGObject_Type)


/* ---------- forward type declarations ---------- */
PyTypeObject G_GNUC_INTERNAL PyNTrackGMonitor_Type;

#line 25 "pyntrack-gmonitor.c"



/* ----------- NTrackGMonitor ----------- */

PyTypeObject G_GNUC_INTERNAL PyNTrackGMonitor_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "pyntrack.GMonitor",                   /* tp_name */
    sizeof(PyGObject),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)NULL, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)0,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- functions ----------- */

static PyObject *
_wrap_ntrack_g_monitor_get(PyObject *self)
{
    NTrackGMonitor *ret;

    
    ret = ntrack_g_monitor_get();
    
    /* pygobject_new handles NULL checking */
    return pygobject_new((GObject *)ret);
}

static PyObject *
_wrap_ntrack_glib_get_state(PyObject *self)
{
    gint ret;

    
    ret = ntrack_glib_get_state();
    
    return pyg_enum_from_gtype(N_TYPE_TRACK_GLIB_STATE, ret);
}

static PyObject *
_wrap_ntrack_info_human_banner(PyObject *self)
{
    const gchar *ret;

    
    ret = ntrack_info_human_banner();
    
    if (ret)
        return PyString_FromString(ret);
    Py_INCREF(Py_None);
    return Py_None;
}

const PyMethodDef pyntrack_functions[] = {
    { "ntrack_g_monitor_get", (PyCFunction)_wrap_ntrack_g_monitor_get, METH_NOARGS,
      NULL },
    { "ntrack_glib_get_state", (PyCFunction)_wrap_ntrack_glib_get_state, METH_NOARGS,
      NULL },
    { "ntrack_info_human_banner", (PyCFunction)_wrap_ntrack_info_human_banner, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};


/* ----------- enums and flags ----------- */

void
pyntrack_add_constants(PyObject *module, const gchar *strip_prefix)
{
#ifdef VERSION
    PyModule_AddStringConstant(module, "__version__", VERSION);
#endif
  pyg_enum_add(module, "GlibState", strip_prefix, N_TYPE_TRACK_GLIB_STATE);
  pyg_enum_add(module, "GlibEvent", strip_prefix, N_TYPE_TRACK_GLIB_EVENT);

  if (PyErr_Occurred())
    PyErr_Print();
}

/* initialise stuff extension classes */
void
pyntrack_register_classes(PyObject *d)
{
    PyObject *module;

    if ((module = PyImport_ImportModule("gobject")) != NULL) {
        _PyGObject_Type = (PyTypeObject *)PyObject_GetAttrString(module, "GObject");
        if (_PyGObject_Type == NULL) {
            PyErr_SetString(PyExc_ImportError,
                "cannot import name GObject from gobject");
            return ;
        }
    } else {
        PyErr_SetString(PyExc_ImportError,
            "could not import gobject");
        return ;
    }


#line 163 "pyntrack-gmonitor.c"
    pygobject_register_class(d, "NTrackGMonitor", N_TYPE_TRACK_GMONITOR, &PyNTrackGMonitor_Type, Py_BuildValue("(O)", &PyGObject_Type));
    pyg_set_object_has_new_constructor(N_TYPE_TRACK_GMONITOR);
}
