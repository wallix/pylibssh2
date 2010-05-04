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

/* {{{ PYLIBSSH2_doc
 */
PyDoc_STRVAR(PYLIBSSH2_doc,
"Python binding for libssh2 library\n\
\n\
pylibssh2 is a C extension module around C library libssh2. It provide an\n\
easy way to manage SSH connections in Python.\n\
\n\
The high-level API start with creation of an L{Session} object with a\n\
socket-like object connected. Then create a L{Channel} with L{open_session}\n\
method on the session instance.\n\
\n");

/* }}} */

PyObject *PYLIBSSH2_Error;

/* {{{ PYLIBSSH2_Session
 */
PyDoc_STRVAR(PYLIBSSH2_Session_doc,
"\n\
This class provide SSH Session operations.\n\
\n\
close() -- closes the session\n\
direct_tcpip() -- tunnels a TCP connection\n\
forward_listen() -- forwards a TCP connection\n\
hostkey_hash() -- returns the computed digest of the remote host key\n\
last_error() -- returns the last error in tuple format\n\
open_session() -- allocates a new channel\n\
scp_recv() -- requests a remote file via SCP protocol\n\
scp_send() -- sends a remote file via SCP protocol\n\
session_method_pref() -- sets preferred methods to be negociated\n\
session_methods() -- returns a dictionnary with the currently active algorithms\n\
set_banner() -- sets the banner that will be sent to remote host\n\
sftp_init() -- opens an SFTP Channel\n\
startup() -- starts up the session from a socket\n\
userauth_authenticated() -- returns authentification status\n\
userauth_list() -- lists the authentification methods\n\
userauth_password() -- authenticates a session with credentials\n\
userauth_publickey_fromfile() -- authenticates a session with publickey\n\
");

static PyObject *
PYLIBSSH2_Session(PyObject *self, PyObject *args)
{
    int dealloc = 1;

    if (!PyArg_ParseTuple(args, "|i:Session", &dealloc)) {
        return NULL;
    }

    return (PyObject *)PYLIBSSH2_Session_New(libssh2_session_init(), dealloc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel
 */
PyDoc_STRVAR(PYLIBSSH2_Channel_doc,
"\n\
This class provide SSH Channel operations.\n\
\n\
close() -- closes the active channel\n\
eof() -- checks if the remote host has sent an EOF status\n\
execute() -- executes command of the channel\n\
exit_status() -- gets the exit code\n\
flush() -- flushs the read buffer\n\
poll() -- polls for activity on the channel\n\
poll_read() -- checks if data is available on the channel\n\
pty() -- requests a pty\n\
pty_resize() -- requests a pty resize\n\
read() -- reads bytes on the channel\n\
send_eof() -- sends EOF status\n\
setblocking() -- sets blocking mode\n\
setenv() -- sets envrionment variable\n\
shell() -- requests a shell\n\
window_read() -- checks the status of the read window\n\
write() -- writes data on a channel\n\
x11_req() -- requests an X11 Forwarding channel\n\
");

static PyObject *
PYLIBSSH2_Channel(PyObject *self, PyObject *args)
{
    PYLIBSSH2_SESSION *session;
    int dealloc = 1;

    if (!PyArg_ParseTuple(args, "O|i:Channel", &session, &dealloc)) {
        return NULL;
    }

    return (PyObject *)PYLIBSSH2_Channel_New(libssh2_channel_open_session(
                        session->session), dealloc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp
 */
static char PYLIBSSH2_Sftp_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp(PyObject *self, PyObject *args)
{
    PYLIBSSH2_SESSION *session;
    int dealloc = 1;

    if (!PyArg_ParseTuple(args, "O|i:Sftp", &session, &dealloc)) {
        return NULL;
    }

    return (PyObject *)PYLIBSSH2_Sftp_New(libssh2_sftp_init(
                        session->session), dealloc);
}
/* }}} */

/* {{{ PYLIBSSH2_methods[]
 */
static PyMethodDef PYLIBSSH2_methods[] = {
    { "Session", (PyCFunction)PYLIBSSH2_Session, METH_VARARGS, PYLIBSSH2_Session_doc },
    { "Channel", (PyCFunction)PYLIBSSH2_Channel, METH_VARARGS, PYLIBSSH2_Channel_doc },
    { "Sftp", (PyCFunction)PYLIBSSH2_Sftp, METH_VARARGS, PYLIBSSH2_Sftp_doc },
    { NULL, NULL }
};
/* }}} */

/* {{{ init_libssh2
 */
PyMODINIT_FUNC
init_libssh2(void)
{
    static void *PYLIBSSH2_API[PYLIBSSH2_API_pointers];
    PyObject *c_api_object;
    PyObject *module, *dict;

    module = Py_InitModule3(
        PYLIBSSH2_MODULE_NAME, 
        PYLIBSSH2_methods, 
        PYLIBSSH2_doc
    );
    if (module == NULL) {
        return;
    }

    PYLIBSSH2_API[PYLIBSSH2_Session_New_NUM] = (void *) PYLIBSSH2_Session_New;
    PYLIBSSH2_API[PYLIBSSH2_Channel_New_NUM] = (void *) PYLIBSSH2_Channel_New;
    PYLIBSSH2_API[PYLIBSSH2_Sftp_New_NUM] = (void *) PYLIBSSH2_Sftp_New;
    PYLIBSSH2_API[PYLIBSSH2_Sftphandle_New_NUM] = (void *) PYLIBSSH2_Sftphandle_New;

    c_api_object = PyCObject_FromVoidPtr((void *)PYLIBSSH2_API, NULL);
    if (c_api_object != NULL) {
        PyModule_AddObject(module, "_C_API", c_api_object);
    }

    PYLIBSSH2_Error = PyErr_NewException(
        PYLIBSSH2_MODULE_NAME".Error", 
        NULL, 
        NULL
    );
    if (PYLIBSSH2_Error == NULL) {
        goto error;
    }
    if (PyModule_AddObject(module, "Error", PYLIBSSH2_Error) != 0) {
        goto error;
    }

    PyModule_AddIntConstant(module, "FINGERPRINT_MD5", 0x0000);
    PyModule_AddIntConstant(module, "FINGERPRINT_SHA1", 0x0001);
    PyModule_AddIntConstant(module, "FINGERPRINT_HEX", 0x0000);
    PyModule_AddIntConstant(module, "FINGERPRINT_RAW", 0x0002);

    PyModule_AddIntConstant(module, "METHOD_KEX",  LIBSSH2_METHOD_KEX);
    PyModule_AddIntConstant(module, "METHOD_HOSTKEY",  LIBSSH2_METHOD_HOSTKEY);
    PyModule_AddIntConstant(module, "METHOD_CRYPT_CS",  LIBSSH2_METHOD_CRYPT_CS);
    PyModule_AddIntConstant(module, "METHOD_CRYPT_SC",  LIBSSH2_METHOD_CRYPT_SC);
    PyModule_AddIntConstant(module, "METHOD_MAC_CS",  LIBSSH2_METHOD_MAC_CS);
    PyModule_AddIntConstant(module, "METHOD_MAC_SC",  LIBSSH2_METHOD_MAC_SC);
    PyModule_AddIntConstant(module, "METHOD_COMP_CS",  LIBSSH2_METHOD_COMP_CS);
    PyModule_AddIntConstant(module, "METHOD_COMP_SC",  LIBSSH2_METHOD_COMP_SC);

    PyModule_AddIntConstant(module, "SFTP_SYMLINK", LIBSSH2_SFTP_SYMLINK);
    PyModule_AddIntConstant(module, "SFTP_READLINK", LIBSSH2_SFTP_READLINK);
    PyModule_AddIntConstant(module, "SFTP_REALPATH", LIBSSH2_SFTP_REALPATH);

    PyModule_AddIntConstant(module, "SFTP_STAT", LIBSSH2_SFTP_STAT);
    PyModule_AddIntConstant(module, "SFTP_LSTAT", LIBSSH2_SFTP_LSTAT);
    
    PyModule_AddStringConstant(module, "DEFAULT_BANNER", LIBSSH2_SSH_DEFAULT_BANNER);
    PyModule_AddStringConstant(module, "LIBSSH2_VERSION", LIBSSH2_VERSION);

    dict = PyModule_GetDict(module);
    if (!init_libssh2_Session(dict)) {
        goto error;
    }
    if (!init_libssh2_Channel(dict)) {
        goto error;
    }
    if (!init_libssh2_Sftp(dict)) {
        goto error;
    }
    if (!init_libssh2_Sftphandle(dict)) {
        goto error;
    }

    error:
    ;
}
/* }}} */
