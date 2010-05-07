/*-
 * pylibssh2 - python bindings for libssh2 library
 *
 * Copyright (C) 2005 Keyphrene.com.
 * Copyright (C) 2010 Wallix Inc.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <Python.h>
#define PYLIBSSH2_MODULE
#include "pylibssh2.h"

/*
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 *   {  'name', (PyCFunction)PYLIBSSH2_Sftphandle_name, METH_VARARGS }
 * for convenience
 */
#define ADD_METHOD(name) \
{ #name, (PyCFunction)PYLIBSSH2_Sftphandle_##name, METH_VARARGS, PYLIBSSH2_Sftphandle_##name##_doc }

static PyMethodDef PYLIBSSH2_Sftphandle_methods[] =
{
    { NULL, NULL }
};

PYLIBSSH2_SFTPHANDLE *
PYLIBSSH2_Sftphandle_New(LIBSSH2_SFTP_HANDLE *sftphandle, int dealloc)
{
    PYLIBSSH2_SFTPHANDLE *self;

    self = PyObject_New(PYLIBSSH2_SFTPHANDLE, &PYLIBSSH2_Sftphandle_Type);
    if (self == NULL) {
        return NULL;
    }

    self->sftphandle = sftphandle;
    self->dealloc = dealloc;

    return self;
}

static void
PYLIBSSH2_Sftphandle_dealloc(PYLIBSSH2_SFTPHANDLE *self)
{
    PyObject_Del(self);
}

static PyObject *
PYLIBSSH2_Sftphandle_getattr(PYLIBSSH2_SFTPHANDLE *self, char *name)
{
    return Py_FindMethod(PYLIBSSH2_Sftphandle_methods, (PyObject *)self, name);
}

/*
 * see /usr/include/python2.5/object.c line 261
 */
PyTypeObject PYLIBSSH2_Sftphandle_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                     /* ob_size */
    "Sftphandle",                          /* tp_name */
    sizeof(PYLIBSSH2_SFTPHANDLE),               /* tp_basicsize */
    0,                                     /* tp_itemsize */
    (destructor)PYLIBSSH2_Sftphandle_dealloc,    /* tp_dealloc */
    0,                                     /* tp_print */
    (getattrfunc)PYLIBSSH2_Sftphandle_getattr,  /* tp_getattr */
    0,                                     /* tp_setattr */
    0,                                     /* tp_compare */
    0,                                     /* tp_repr */
    0,                                     /* tp_as_number */
    0,                                     /* tp_as_sequence */
    0,                                     /* tp_as_mapping */
    0,                                     /* tp_hash  */
    0,                                     /* tp_call */
    0,                                     /* tp_str */
    0,                                     /* tp_getattro */
    0,                                     /* tp_setattro */
    0,                                     /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                    /* tp_flags */
    "Sftphandle objects",                  /* tp_doc */
};

int
init_libssh2_Sftphandle(PyObject *dict)
{
    PYLIBSSH2_Sftphandle_Type.ob_type = &PyType_Type;
    Py_XINCREF(&PYLIBSSH2_Sftphandle_Type);
    PyDict_SetItemString(dict, "SftphandleType", (PyObject *)&PYLIBSSH2_Sftphandle_Type);

    return 1;
}
