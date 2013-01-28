/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2009,2010  Alexander Sack <asac@jwsdot.com>
 *
 * This file is part of:
 *     ntrack - Network Status Tracking for Desktop Applications
 *              http://launchpad.net/ntrack
 *
 *  ntrack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  ntrack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ntrack.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ntrack.h"
#include <pygobject.h>
 
void pyntrack_register_classes (PyObject *d); 
extern PyMethodDef pyntrack_functions[];
static PyMethodDef *pyntrack_functions_man;
static PyObject *pyntrack_error;

static PyObject *
wrap_ntrack_init(PyObject *self, PyObject *args)
{
	PyObject *o_list = 0;

	/* if fail to parse args, return quick */
	if(!PyArg_ParseTuple (args, "O", &o_list))
		return NULL;

	/* object must be of type pylist; if not we have to fail again */
	if (PyList_Check(o_list)) {
		int argc = PyList_Size (o_list);
		char **argv = malloc (sizeof (char*) * argc);
		Py_ssize_t i;
		for (i=0; i < argc; i++) {
			argv[i] =  PyString_AsString ( PyList_GetItem (o_list, i));
		}
		ntrack_init (&argc, &argv);
		free (argv);
	} else {
		PyErr_SetString(pyntrack_error, "pyntrack.ntrack_init failed; argument is not a pylist");
		return NULL;
	}

	return Py_None;
}

PyMODINIT_FUNC
initpyntrack (void)
{
    PyObject *m, *d;
    int c=0,i;
    PyMethodDef init_def = { "ntrack_init", wrap_ntrack_init, METH_VARARGS, "initialize ntrack system" };
    PyMethodDef end_def = { NULL, NULL, 0, NULL };

    init_pygobject ();

    /* count generated pyntrack fuctions */
    for (i=0; pyntrack_functions[i].ml_name; i++)
        c++;

    /* create extended pyntrack function table with room for one more func: ntrack_init */
    pyntrack_functions_man = malloc (sizeof (PyMethodDef) * (c+2));
    memcpy (pyntrack_functions_man, pyntrack_functions, sizeof(PyMethodDef) * c);

    pyntrack_functions_man[c] = init_def;
    pyntrack_functions_man[c+1] = end_def;

    /* setup module */
    m = Py_InitModule ("pyntrack", pyntrack_functions_man);
    d = PyModule_GetDict (m);
 
    pyntrack_register_classes (d);

    pyntrack_error = PyErr_NewException("pyntrack.error", NULL, NULL);
    Py_INCREF(pyntrack_error);
    PyModule_AddObject(m, "NtrackException", pyntrack_error);

    if (PyErr_Occurred ()) {
        Py_FatalError ("can't initialise module pyntrack");
    }
}

