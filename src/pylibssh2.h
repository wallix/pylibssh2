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
#ifndef _PYLIBSSH2_SSH2_H_
#define _PYLIBSSH2_SSH2_H_

#include <Python.h>

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <libssh2_publickey.h>

#include "channel.h"
#include "listener.h"
#include "sftp.h"
#include "sftphandle.h"
#include "session.h"
#include "util.h"

/* pylibssh2 module version */
#define PYLIBSSH2_VERSION MAJOR_VERSION"."MINOR_VERSION"."PATCH_VERSION

/* Python module name */
#define PYLIBSSH2_MODULE_NAME "_libssh2"

/* Python module's Error */
extern PyObject *PYLIBSSH2_Error;
/* Thread support
 *
 * WITH_THREAD is defined in /usr/include/python2.{4,5,6}/pyconfig.h
 *
 * */
#ifdef WITH_THREAD
#   define MY_BEGIN_ALLOW_THREADS(st) \
    { st = PyEval_SaveThread(); }
#   define MY_END_ALLOW_THREADS(st) \
    { PyEval_RestoreThread(st);}
#else
#   define MY_BEGIN_ALLOW_THREADS(st)
#   define MY_END_ALLOW_THREADS(st) { st = NULL; }
#endif

#ifdef exception_from_error_queue
#   undef exception_from_error_queue
#endif
#define exception_from_error_queue() do { \
    PyObject *errlist = error_queue_to_list(); \
    PyErr_SetObject(PYLIBSSH2_Error, errlist); \
    Py_DECREF(errlist); \
} while (0)

#define PYLIBSSH2_Session_New_NUM        0
#define PYLIBSSH2_Session_New_RETURN     PYLIBSSH2_SESSION *
#define PYLIBSSH2_Session_New_PROTO      (LIBSSH2_SESSION *, int)

#define PYLIBSSH2_Channel_New_NUM        1
#define PYLIBSSH2_Channel_New_RETURN     PYLIBSSH2_CHANNEL *
#define PYLIBSSH2_Channel_New_PROTO      (LIBSSH2_CHANNEL *, int)

#define PYLIBSSH2_Sftp_New_NUM           2
#define PYLIBSSH2_Sftp_New_RETURN        PYLIBSSH2_SFTP *
#define PYLIBSSH2_Sftp_New_PROTO         (LIBSSH2_SFTP *, int)

#define PYLIBSSH2_Sftphandle_New_NUM     3
#define PYLIBSSH2_Sftphandle_New_RETURN  PYLIBSSH2_SFTPHANDLE *
#define PYLIBSSH2_Sftphandle_New_PROTO   (LIBSSH2_SFTP_HANDLE *, int)

#define PYLIBSSH2_Listener_New_NUM       4
#define PYLIBSSH2_Listener_New_RETURN    PYLIBSSH2_LISTENER *
#define PYLIBSSH2_Listener_New_PROTO     (LIBSSH2_LISTENER *, int)

#define PYLIBSSH2_API_pointers           5

#ifdef PYLIBSSH2_MODULE

extern PYLIBSSH2_Session_New_RETURN     PYLIBSSH2_Session_New   PYLIBSSH2_Session_New_PROTO;
extern PYLIBSSH2_Channel_New_RETURN     PYLIBSSH2_Channel_New   PYLIBSSH2_Channel_New_PROTO;
extern PYLIBSSH2_Sftp_New_RETURN        PYLIBSSH2_Sftp_New      PYLIBSSH2_Sftp_New_PROTO;
extern PYLIBSSH2_Sftphandle_New_RETURN  PYLIBSSH2_Sftphandle_New   PYLIBSSH2_Sftphandle_New_PROTO;
extern PYLIBSSH2_Listener_New_RETURN    PYLIBSSH2_Listener_New  PYLIBSSH2_Listener_New_PROTO;

#else

extern void **PYLIBSSH2_API;

/*#define PYLIBSSH2_Session_New  (*(PYLIBSSH2_Session_New_RETURN (*)PYLIBSSH2_Session_New_PROTO) PYLIBSSH2_API[PYLIBSSH2_Session_New_NUM])
#define PYLIBSSH2_Channel_New (*(PYLIBSSH2_Channel_New_RETURN (*)PYLIBSSH2_Channel_New_PROTO) PYLIBSSH2_API[PYLIBSSH2_Channel_New_NUM])
#define PYLIBSSH2_Sftp_New (*(PYLIBSSH2_Sftp_New_RETURN (*)PYLIBSSH2_Sftp_New_PROTO) PYLIBSSH2_API[PYLIBSSH2_Sftp_New_NUM])
#define PYLIBSSH2_Sftphandle_New (*(PYLIBSSH2_Sftphandle_New_RETURN (*)PYLIBSSH2_Sftphandle_New_PROTO) PYLIBSSH2_API[PYLIBSSH2_Sftphandle_New_NUM])
#define PYLIBSSH2_Listener_New (*(PYLIBSSH2_Listener_New_RETURN (*)PYLIBSSH2_Listener_New_PROTO) PYLIBSSH2_API[PYLIBSSH2_Listener_New_NUM])*/

#define import_PYLIBSSH2() \
{ \
  PyObject *PYLIBSSH2_module = PyImport_ImportModule(PYLIBSSH2_MODULE_NAME); \
  if (PYLIBSSH2_module != NULL) { \
    PyObject *PYLIBSSH2_dict, *PYLIBSSH2_api_object; \
    PYLIBSSH2_dict = PyModule_GetDict(PYLIBSSH2_module); \
    PYLIBSSH2_api_object = PyDict_GetItemString(PYLIBSSH2_dict, "_C_API"); \
    if (PyCObject_Check(PYLIBSSH2_api_object)) { \
      PYLIBSSH2_API = (void **)PyCObject_AsVoidPtr(PYLIBSSH2_api_object); \
    } \
  } \
}

#endif /* PYLIBSSH2_MODULE */

#endif /* _PYLIBSSH2_SSH2_H_ */
