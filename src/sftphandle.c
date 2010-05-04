/*-
 * Copyright (C) 2005 Keyphrene.com.
 * Copyright (c) 2010 Wallix Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of any co-contributors
 *       may be used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
