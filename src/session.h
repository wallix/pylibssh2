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
#ifndef _PYLIBSSH2_SESSION_H_
#define _PYLIBSSH2_SESSION_H_

#include <Python.h>
#include <libssh2.h>

extern int init_libssh2_Session(PyObject *);

extern PyTypeObject PYLIBSSH2_Session_Type;

#define PYLIBSSH2_Session_Check(v) ((v)->ob_type == &PYLIBSSH2_Session_Type)

typedef struct {
    PyObject_HEAD
    LIBSSH2_SESSION *session;
    PyObject        *socket;
    PyThreadState   *tstate;
    int             dealloc;
    int             opened;
} PYLIBSSH2_SESSION;

#endif /* _PYLIBSSH2_SESSION_H_ */
