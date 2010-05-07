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

/* {{{ PYLIBSSH2_Listener_accept
 */
static char PYLIBSSH2_Listener_accept_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Listener_accept(PYLIBSSH2_LISTENER *self, PyObject *args)
{
    LIBSSH2_CHANNEL *channel;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    channel = libssh2_channel_forward_accept(self->listener);
    MY_END_ALLOW_THREADS(self->tstate);

    if (channel == NULL) {
        PyErr_SetString(PYLIBSSH2_Error, "Unable to accept listener on channel.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)PYLIBSSH2_Channel_New(channel, 1);
}
/* }}} */

/* {{{ PYLIBSSH2_Listener_cancel
 */
static char PYLIBSSH2_Listener_cancel_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Listener_cancel(PYLIBSSH2_LISTENER *self, PyObject *args)
{
    int rc;
    
    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_forward_cancel(self->listener);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Listener_methods[]
 *
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 *  { 'name', (PyCFunction)PYLIBSSH2_Listener_name, METHOD_VARARGS }
 *  for convenience
 */
#define ADD_METHOD(name) \
{ #name, (PyCFunction)PYLIBSSH2_Listener_##name, METH_VARARGS, PYLIBSSH2_Listener_##name##_doc }

struct PyMethodDef PYLIBSSH2_Listener_methods[] = {
    ADD_METHOD(accept),
    ADD_METHOD(cancel),
    { NULL, NULL }
};
#undef ADD_METHOD
/* }}} */

/* {{{ PYLIBSSH2_Listener_New
 */
PYLIBSSH2_LISTENER *
PYLIBSSH2_Listener_New(LIBSSH2_LISTENER *listener, int dealloc)
{
    PYLIBSSH2_LISTENER *self;

    self = PyObject_New(PYLIBSSH2_LISTENER, &PYLIBSSH2_Listener_Type);
    if (self == NULL) {
        return NULL;
    }

    self->listener = listener;
    self->dealloc = dealloc;
    self->tstate = NULL;

    return self;
}
/* }}} */

/* {{{ PYLIBSSH2_Listener_dealloc
 */
static void
PYLIBSSH2_Listener_dealloc(PYLIBSSH2_LISTENER *self)
{
    if (self) {
        PyObject_Del(self);
    }
}
/* }}} */

/* {{{ PYLIBSSH2_Listener_getattr
 */
static PyObject *
PYLIBSSH2_Listener_getattr(PYLIBSSH2_LISTENER *self, char *name)
{
    return Py_FindMethod(PYLIBSSH2_Listener_methods, (PyObject *) self, name);
}
/* }}} */

/* {{{ PYLIBSSH2_Listener_Type
 *
 * see /usr/include/python2.5/object.h line 261
 */
PyTypeObject PYLIBSSH2_Listener_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                       /* ob_size */
    "Listener",                              /* tp_name */
    sizeof(PYLIBSSH2_LISTENER),              /* tp_basicsize */
    0,                                       /* tp_itemsize */
    (destructor)PYLIBSSH2_Listener_dealloc,  /* tp_dealloc */
    0,                                       /* tp_print */
    (getattrfunc)PYLIBSSH2_Listener_getattr, /* tp_getattr */
    0,                                       /* tp_setattr */
    0,                                       /* tp_compare */
    0,                                       /* tp_repr */
    0,                                       /* tp_as_number */
    0,                                       /* tp_as_sequence */
    0,                                       /* tp_as_mapping */
    0,                                       /* tp_hash  */
    0,                                       /* tp_call */
    0,                                       /* tp_str */
    0,                                       /* tp_getattro */
    0,                                       /* tp_setattro */
    0,                                       /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                      /* tp_flags */
    "Listener objects",                      /* tp_doc */
};
/* }}} */

/* {{{ init_libssh2_Listener
 */
int
init_libssh2_Listener(PyObject *dict)
{
    PYLIBSSH2_Listener_Type.ob_type = &PyType_Type;
    Py_XINCREF(&PYLIBSSH2_Listener_Type);
    PyDict_SetItemString(dict, "ListenerType", (PyObject *)&PYLIBSSH2_Listener_Type);

    return 1;
}
/* }}} */
