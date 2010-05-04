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

/* {{{ PYLIBSSH2_Session_set_banner
 */
static char PYLIBSSH2_Session_set_banner_doc[] = "\
set_banner(banner) -> int\n\
\n\
Sets the banner that will be sent to remote host.\n\
This is optional, the banner libssh2.DEFAULT_BANNER will be sent by default.\n\
\n\
@param  banner: an user defined banner\n\
@type   banner: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_set_banner(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc;
    char *banner;

    if (!PyArg_ParseTuple(args, "s:set_banner", &banner)) {
        return NULL;
    }

    rc=libssh2_banner_set(self->session, banner);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_startup
 */
static char PYLIBSSH2_Session_startup_doc[] = "\
startup(socket) -> int\n\
\n\
Starts up the session from a socket created by socket.socket() call.\n\
\n\
@param  socket: a connected socket object\n\
@type   socket: socket._socketobject\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_startup(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc;
    int fd;
    char *last_error = "";
    PyObject *socket;

    if (!PyArg_ParseTuple(args, "O:startup", &socket)) {
        return NULL;
    }

    Py_XINCREF(socket);
    self->socket = socket;
    fd = PyObject_AsFileDescriptor(socket);
    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_session_startup(self->session, fd);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        libssh2_session_last_error(self->session, &last_error, NULL, 0);
        /* CLEAN: PYLIBSSH2_SESSION_STARTUP_MSG */
        PyErr_Format(PYLIBSSH2_Error, "SSH startup: %s", last_error);
        Py_INCREF(Py_None);
        return Py_None;
    }
    
    self->opened = 1;

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_close
 */
static char PYLIBSSH2_Session_close_doc[] = "\
close([reason]) -> int \n\
\n\
Closes the session.\n\
\n\
@param  reason: human readable reason for disconnection\n\
@type   reason: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_close(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc;
    char *reason = "end";

    if (!PyArg_ParseTuple(args, "|s:close", &reason)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_session_disconnect(self->session, reason);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_SESSION_CLOSE_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "SSH close error.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    self->opened = 0;

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_userauth_authenticated
 */
static char PYLIBSSH2_Session_userauth_authenticated_doc[] = "\
userauth_authenticated() -> int\n\
\n\
Returns authentification status for the given session.\n\
\n\
@return non-zero if authenticated or 0 if not\n\
@rtype  int";

static PyObject * 
PYLIBSSH2_Session_userauth_authenticated(PYLIBSSH2_SESSION *self, PyObject *args)
{
    return Py_BuildValue("i", libssh2_userauth_authenticated(self->session));
}
/* }}} */

/* {{{ PYLIBSSH2_Session_userauth_list
 */
static char PYLIBSSH2_Session_userauth_list_doc[] = "\
userauth_list(username) -> str\n\
\n\
Lists the authentification methods supported by a server.\n\
\n\
@param  username: username which will be used while authenticating\n\
@type   username: str\n\
\n\
@return a string containing a comma-separated list of authentication methods\n\
@rtype  str";

static PyObject *
PYLIBSSH2_Session_userauth_list(PYLIBSSH2_SESSION *self, PyObject *args)
{
    char *username;
    int username_len = 0;
    char *auth_list;

    if (!PyArg_ParseTuple(args, "s#:userauth_list", &username, &username_len)) {
        return NULL;
    }

    auth_list=libssh2_userauth_list(self->session, username, username_len);
    if (auth_list == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyString_FromString(auth_list);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_hostkey_hash
 */
static char PYLIBSSH2_Session_hostkey_hash_doc[] = "\n\
hostkey_hash([hashtype]) -> str\n\
\n\
Returns the computed digest of the remote host's key.\n\
\n\
@param  hashtype: values possible are 1 (HASH_MD5) or 2 (HASH_SHA1)\n\
@type   hashtype: str\n\
\n\
@return string representation of the computed hash value\n\
@rtype  str";

static PyObject *
PYLIBSSH2_Session_hostkey_hash(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int hashtype = LIBSSH2_HOSTKEY_HASH_MD5;
    const char *hash;

    if (!PyArg_ParseTuple(args, "|i:hostkey_hash", &hashtype)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    hash = libssh2_hostkey_hash(self->session, hashtype);
    MY_END_ALLOW_THREADS(self->tstate);

    if (hash == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyString_FromString(hash);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_userauth_password
 */
static char PYLIBSSH2_Session_userauth_password_doc[] = "\n\
userauth_password(username, password) -> int \n\
\n\
Authenticates a session with the given username and password.\n\
\n\
@param  username: user to authenticate\n\
@type   username: str\n\
@param  password: password to use for the authentication\n\
@type   password: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_userauth_password(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc;
    char *username;
    char *password;

    if (!PyArg_ParseTuple(args, "ss:userauth_password", &username, &password))
        return NULL;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_userauth_password(self->session, username, password);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_SESSION_USERAUTH_PASSWORD_FAILED_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Authentification by password failed.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_userauth_publickey_fromfile
 */
static char PYLIBSSH2_Session_userauth_publickey_fromfile_doc[] = "\n\
userauth_publickey_fromfile(username, publickey, privatekey, passphrase) -> int\n\
\n\
Authenticates a session as username using a key pair found in the pulickey and\n\
privatekey files, and passphrase if provided.\n\
\n\
@param  username: user to authenticate\n\
@type   username: str\n\
@param  publickey: path and name of public key file\n\
@type   publickey: str\n\
@param  privatekey: path and name of private key file\n\
@type   privatekey: str\n\
@param  passphrase: passphrase to use when decoding private file\n\
@type   passphrase: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_userauth_publickey_fromfile(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc;
    char *username;
    char *publickey;
    char *privatekey;
    char *passphrase;
    char *last_error;

    if (!PyArg_ParseTuple(args, "sss|s:userauth_publickey_fromfile", &username,
                          &publickey, &privatekey, &passphrase)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_userauth_publickey_fromfile(self->session, username, publickey,
                                             privatekey, passphrase);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        libssh2_session_last_error(self->session, &last_error, NULL, 0);
        PyErr_Format(PYLIBSSH2_Error, "Authentification by public key failed: %s",
                     last_error);
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_session_methods
 */
static char PYLIBSSH2_Session_session_methods_doc[] = "\n\
session_methods() -> dict\n\
\n\
Returns a dictionnary with the currently active algorithms. \n\
CS keys is Client to Server and SC keys is Server to Client.\n\
\n\
@return a dictionnary with actual method neogicated\n\
@rtype dict";

static PyObject *
PYLIBSSH2_Session_session_methods(PYLIBSSH2_SESSION *self, PyObject *args)
{
    /* cs = client-server, sc = server-client */
    const char *kex, *hostkey;
    const char *crypt_cs, *crypt_sc;
    const char *mac_cs, *mac_sc;
    const char *comp_cs, *comp_sc;
    const char *lang_cs, *lang_sc;

    PyObject *methods;

    kex = libssh2_session_methods(self->session, LIBSSH2_METHOD_KEX);
    if (kex == NULL) {
        /* session has not yet been started no methods negociated */
        Py_INCREF(Py_None);
        return Py_None;
    }
    hostkey = libssh2_session_methods(self->session, LIBSSH2_METHOD_HOSTKEY);
    crypt_cs = libssh2_session_methods(self->session, LIBSSH2_METHOD_CRYPT_CS);
    crypt_sc = libssh2_session_methods(self->session, LIBSSH2_METHOD_CRYPT_SC);
    mac_cs = libssh2_session_methods(self->session, LIBSSH2_METHOD_MAC_CS);
    mac_sc = libssh2_session_methods(self->session, LIBSSH2_METHOD_MAC_SC);
    comp_cs = libssh2_session_methods(self->session, LIBSSH2_METHOD_COMP_CS);
    comp_sc = libssh2_session_methods(self->session, LIBSSH2_METHOD_COMP_SC);
    lang_cs = libssh2_session_methods(self->session, LIBSSH2_METHOD_LANG_CS);
    lang_sc = libssh2_session_methods(self->session, LIBSSH2_METHOD_LANG_SC);

    /* create a python dictionnary to store cryptographic algorithms */
    methods = PyDict_New();
    PyDict_SetItemString(methods, "KEX", PyString_FromString(kex));
    PyDict_SetItemString(methods, "HOSTKEY", PyString_FromString(hostkey));
    PyDict_SetItemString(methods, "CRYPT_CS", PyString_FromString(crypt_cs));
    PyDict_SetItemString(methods, "CRYPT_SC", PyString_FromString(crypt_sc));
    PyDict_SetItemString(methods, "MAC_CS", PyString_FromString(mac_cs));
    PyDict_SetItemString(methods, "MAC_SC", PyString_FromString(mac_sc));
    PyDict_SetItemString(methods, "COMP_CS", PyString_FromString(comp_cs));
    PyDict_SetItemString(methods, "COMP_SC", PyString_FromString(comp_sc));
    PyDict_SetItemString(methods, "LANG_CS", PyString_FromString(lang_cs));
    PyDict_SetItemString(methods, "LANG_SC", PyString_FromString(lang_sc));

    return methods;
}
/* }}} */

/* {{{ PYLIBSSH2_Session_session_method_pref
 */
static char PYLIBSSH2_Session_session_method_pref_doc[] = "\n\
session_method_pref(method_type, pref) -> int\n\
\n\
Sets preferred methods to be negociated. Theses preferences must be\n\
prior to calling startup().\n\
\n\
@param  method_type: the method type constants\n\
@type   method_type: L{libssh2.METHOD}\n\
@param  pref: coma delimited list of preferred methods\n\
@type   pref: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Session_session_method_pref(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int method;
    char *pref;

    if (!PyArg_ParseTuple(args, "is:session_method_pref", &method, &pref)) {
        return NULL;
    }

    return PyInt_FromLong(libssh2_session_method_pref(self->session, method, pref)==0?1:0);   
}
/* }}} */

/* {{{ PYLIBSSH2_Session_open_session
 */
static char PYLIBSSH2_Session_open_session_doc[] = "\n\
open_session() -> libssh2.Channel\n\
\n\
Allocates a new channel for the current session.\n\
\n\
@return new channel opened\n\
@rtype  libssh2.Channel";

static PyObject *
PYLIBSSH2_Session_open_session(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int dealloc = 1;

    if (!PyArg_ParseTuple(args, "|i:open_session", &dealloc)) {
        return NULL;
    }

    return (PyObject *)PYLIBSSH2_Channel_New(libssh2_channel_open_session(self->session),
                                        dealloc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_scp_recv
 */
static char PYLIBSSH2_Session_scp_recv_doc[] = "\n\
scp_recv(remote_path) -> libssh2.Channel\n\
\n\
Requests a remote file via SCP protocol.\n\
\n\
@param  remote_path: absolute path of remote file to transfer\n\
@type   remote_path: str\n\
\n\
@return new channel opened\n\
@rtype  libssh2.Channel";

static PyObject *
PYLIBSSH2_Session_scp_recv(PYLIBSSH2_SESSION *self, PyObject *args)
{
    char *path;
    LIBSSH2_CHANNEL *channel;

    if (!PyArg_ParseTuple(args, "s:scp_recv", &path)) {
        return NULL;
    }

    channel = libssh2_scp_recv(self->session, path, NULL);
    if (channel == NULL) {
        /* CLEAN: PYLIBSSH2_CHANNEL_SCP_RECV_ERROR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "SCP receive error.");
        Py_INCREF(Py_None);
        return Py_None;
    }
    
    return (PyObject *)PYLIBSSH2_Channel_New(channel, 1);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_scp_send
 */
static char PYLIBSSH2_Session_scp_send_doc[] = "\n\
scp_send(path, mode, size) -> libssh2.Channel\n\
\n\
Sends a file to remote host via SCP protocol.\n\
\n\
@param path: absolute path of tile to transfer\n\
@type  path: str\n\
@param mode: file access mode to create file\n\
@type  mode: int\n\
@param size: size of file being transmitted\n\
@type  size: int\n\
\n\
@return new channel opened\n\
@rtype  libssh2.Channel";

static PyObject *
PYLIBSSH2_Session_scp_send(PYLIBSSH2_SESSION *self, PyObject *args)
{
    char *path;
    int mode;
    unsigned long filesize;
    LIBSSH2_CHANNEL *channel;

    if (!PyArg_ParseTuple(args, "sik:scp_send", &path, &mode, &filesize)) {
        return NULL;
    }

    channel = libssh2_scp_send(self->session, path, mode, filesize);
    if (channel == NULL) {
        /* CLEAN: PYLIBSSH2_CHANNEL_SCP_SEND_ERROR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "SCP send error.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)PYLIBSSH2_Channel_New(channel, 1);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_sftp_init
 */
static char PYLIBSSH2_Session_sftp_init_doc[] = "\n\
sftp_init() -> libssh2.Sftp\n\
\n\
Opens an SFTP Channel.\n\
\n\
@return new opened SFTP channel\n\
@rtype  libssh2.Sftp";

static PyObject *
PYLIBSSH2_Session_sftp_init(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int dealloc = 1;

    if (!PyArg_ParseTuple(args, "|i:sftp_init", &dealloc)) {
        return NULL;
    }

    return (PyObject *)PYLIBSSH2_Sftp_New(libssh2_sftp_init(self->session), dealloc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_direct_tcpip
 */
static char PYLIBSSH2_Session_direct_tcpip_doc[] = "\n\
direct_tcpip(host, port, shost, sport) -> libssh2.Channel\n\
\n\
Tunnels a TCP connection through an SSH Session.\n\
\n\
@param  host: remote host\n\
@type   host: str\n\
@param  port: remote port\n\
@type   port: int\n\
@param  shost: local host\n\
@type   shost: str\n\
@param  sport: local port\n\
@type   sport: int\n\
\n\
@return new opened channel\n\
@rtype  libssh2.Channel";

static PyObject *
PYLIBSSH2_Session_direct_tcpip(PYLIBSSH2_SESSION *self, PyObject *args)
{
    /* remote host */
    char *host;
    /* local host */
    char *shost = "127.0.0.1";
    /* local port */
    int sport = 22;
    /* remote port */
    int port;
    LIBSSH2_CHANNEL *channel;
    char *last_error = "";

    if (!PyArg_ParseTuple(args, "si|si:direct_tcpip", &host, &port, &shost, &sport)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    channel = libssh2_channel_direct_tcpip_ex(self->session, host, port, shost, sport);
    MY_END_ALLOW_THREADS(self->tstate);

    if (channel == NULL) {
        libssh2_session_last_error(self->session, &last_error, NULL, 0);
        /* CLEAN: PYLIBSSH2_SESSION_TCP_CONNECT_ERROR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to create TCP connection.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)channel;
}
/* }}} */

/* {{{ PYLIBSSH2_Session_forward_listen
 */
static char PYLIBSSH2_Session_forward_listen_doc[] = "\n\
forward_listen(host, port, bound_port, queue_maxsize) -> libssh2.Listener\n\
\n\
Forwards a TCP connection through an SSH Session.\n\
\n\
@param  host: remote host\n\
@type   host: str\n\
@param  port: remote port\n\
@type   port: int\n\
@param  bound_port: populated with the actual port on the remote host\n\
@type   bound_port: int\n\
@param  queue_maxsize: maximum number of pending connections\n\
@type   int\n\
\n\
@return new libssh2.Listener instance on succes or None on failure\n\
@rtype  libssh2.Listener";

static PyObject *
PYLIBSSH2_Session_forward_listen(PYLIBSSH2_SESSION *self, PyObject *args)
{
    char *host;
    int port;
    int queue_maxsize;
    int *bound_port;
    LIBSSH2_LISTENER *listener;

    if (!PyArg_ParseTuple(args, "siii:forward_listen", &host, &port,
                          &bound_port, &queue_maxsize)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    listener = libssh2_channel_forward_listen_ex(self->session, host, port,
                                                 bound_port, queue_maxsize);
    MY_END_ALLOW_THREADS(self->tstate);

    if (listener == NULL) {
        /* CLEAN: PYLIBSSH2_SESSION_TCP_CONNECT_ERROR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to forward listen connection.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)PYLIBSSH2_Listener_New(listener, 0);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_last_error
 */
static char PYLIBSSH2_Session_last_error_doc[] = "\n\
last_error() -> (int, str)\n\
\n\
Returns the last error in tuple format (code, message).\n\
\n\
@return error tuple (int, str)\n\
@rtype  tuple";

static PyObject *
PYLIBSSH2_Session_last_error(PYLIBSSH2_SESSION *self, PyObject *args)
{
    char *errmsg;
    int rc,want_buf=0;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc=libssh2_session_last_error(self->session, &errmsg, NULL, want_buf);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("(i,s)", rc, errmsg);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_callback_set
 */
static char PYLIBSSH2_Session_callback_set_doc[] = "\n\
callback_set(cbtype, callback)\n\
\n\
Set (or reset) a callback function\n\
\n\
@param cbtype\n\
@param callback\n\
@return\n\
@rtype ";

/*
void
x11_callback(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *channel, 
             const char *shost, int sport, void **abstract)
*/

static PyObject *py_callback_func = NULL;

static void
stub_callback_func(LIBSSH2_SESSION *session,
                   LIBSSH2_CHANNEL *channel,
                   const char *shost,
                   int sport,
                   void **abstract)
{
    int rc=0;

    PYLIBSSH2_SESSION *pysession;
    PYLIBSSH2_CHANNEL *pychannel; 
    PyObject *pyabstract;

    /* Ensure current thread is ready to call Python C API */
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    PyObject *arglist;
    PyObject *result = NULL;

    pysession = PyObject_New(PYLIBSSH2_SESSION, &PYLIBSSH2_Session_Type);
    pysession->session = session;
    pysession->opened = 1;
    pysession->dealloc = 0;
    pysession->tstate = NULL;
    Py_INCREF(pysession);

    pychannel = PyObject_New(PYLIBSSH2_CHANNEL, &PYLIBSSH2_Channel_Type);
    pychannel->channel = channel;
    pychannel->dealloc = 0;
    pychannel->tstate = NULL;
    Py_INCREF(pychannel);

    pyabstract = Py_None;
    Py_INCREF(pyabstract);

    arglist = Py_BuildValue("(OOsiO)",
        pysession, pychannel, shost, sport, pyabstract
    );
    Py_INCREF(arglist);

    /* Performing Python callback with C API */
    result = PyEval_CallObject(py_callback_func, arglist);
    if (result && PyInt_Check(result)) {
        rc = PyInt_AsLong(result);
    }

    /* Restore previous thread state and release acquired resources */
    PyGILState_Release(gstate);

    Py_DECREF(pysession);
    Py_DECREF(pychannel);
    Py_DECREF(pyabstract);
    Py_DECREF(arglist);
}

static PyObject *
PYLIBSSH2_Session_callback_set(PYLIBSSH2_SESSION *self, PyObject *args)
{
    PyObject *result = NULL;

    /* type of callback to register see libssh2.h LIBSSH2_CALLBACK_* */
    int cbtype;

    /* callback is callable Python Object */
    PyObject *cb;

    if (PyArg_ParseTuple(args, "iO:callback_set", &cbtype, &cb)) {
        if (!PyCallable_Check(cb)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(cb);
        Py_XINCREF(py_callback_func);
        py_callback_func = cb;

        MY_BEGIN_ALLOW_THREADS(self->tstate);
        libssh2_session_callback_set(self->session, cbtype, stub_callback_func);
        MY_END_ALLOW_THREADS(self->tstate);

        Py_INCREF(Py_None);
        result = Py_None;
    }

    return result;
}
/* }}} */

/* {{{ PYLIBSSH2_Session_set_trace
 */
static char PYLIBSSH2_Session_set_trace_doc[] = "\n\
set_trace(bitmask)\n\
\n\
Set the trace level\n\
\n\
@param bitmask\n\
@return\n\
@rtype ";

static PyObject *
PYLIBSSH2_Session_set_trace(PYLIBSSH2_SESSION *self, PyObject *args)
{
    int rc=0;
    int bitmask;

    if (!PyArg_ParseTuple(args, "i:set_trace", &bitmask)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    libssh2_trace(self->session, bitmask);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_methods[]
 *
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 * { 'name', (PyCFunction)PYLIBSSH2_Session_name, METHOD_VARARGS }
 * for convenience
 * 
 */
#define ADD_METHOD(name) \
{ #name, (PyCFunction)PYLIBSSH2_Session_##name, METH_VARARGS, PYLIBSSH2_Session_##name##_doc }

static PyMethodDef PYLIBSSH2_Session_methods[] =
{
    ADD_METHOD(set_banner),
    ADD_METHOD(startup),
    ADD_METHOD(close),
    ADD_METHOD(userauth_authenticated),
    ADD_METHOD(hostkey_hash),
    ADD_METHOD(userauth_list),
    ADD_METHOD(session_methods),
    ADD_METHOD(userauth_password),
    ADD_METHOD(userauth_publickey_fromfile),
    ADD_METHOD(session_method_pref),
    ADD_METHOD(open_session),
    ADD_METHOD(scp_recv),
    ADD_METHOD(scp_send),
    ADD_METHOD(sftp_init),
    ADD_METHOD(direct_tcpip),
    ADD_METHOD(forward_listen),
    ADD_METHOD(last_error),
    ADD_METHOD(callback_set),
    ADD_METHOD(set_trace),
    { NULL, NULL }
};
#undef ADD_METHOD
/* }}} */

/* {{{ PYLIBSSH2_Session_New
 */
PYLIBSSH2_SESSION *
PYLIBSSH2_Session_New(LIBSSH2_SESSION *session, int dealloc)
{
    PYLIBSSH2_SESSION *self;

    self = PyObject_New(PYLIBSSH2_SESSION, &PYLIBSSH2_Session_Type);

    if (self == NULL) {
        return NULL;
    }

    self->session = session;
    self->dealloc = dealloc;
    self->opened = 0;
    self->socket = NULL;
    self->tstate = NULL;

    libssh2_banner_set(session, LIBSSH2_SSH_DEFAULT_BANNER " Python");

    return self;
}
/* }}} */

/* {{{ PYLIBSSH2_Session_dealloc
 */
static void
PYLIBSSH2_Session_dealloc(PYLIBSSH2_SESSION *self)
{
    if (self->opened) {
        libssh2_session_disconnect(self->session, "end");
    }

    if (self->dealloc) {
        libssh2_session_free(self->session);
    }

    Py_XDECREF(self->socket);
    self->socket = NULL;

    if (self) {
        PyObject_Del(self);
    }
}
/* }}} */

/* {{{ PYLIBSSH2_Session_getattr
 */
static PyObject *
PYLIBSSH2_Session_getattr(PYLIBSSH2_SESSION *self, char *name)
{
    return Py_FindMethod(PYLIBSSH2_Session_methods, (PyObject *)self, name);
}
/* }}} */

/* {{{ PYLIBSSH2_Session_Type
 *
 * see /usr/include/python2.5/object.h line 261
 */
PyTypeObject PYLIBSSH2_Session_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                       /* ob_size */
    "Session",                               /* tp_name */
    sizeof(PYLIBSSH2_SESSION),               /* tp_basicsize */
    0,                                       /* tp_itemsize */
    (destructor)PYLIBSSH2_Session_dealloc,   /* tp_dealloc */
    0,                                       /* tp_print */
    (getattrfunc)PYLIBSSH2_Session_getattr,  /* tp_getattr */
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
    "Sesssion  objects",                     /* tp_doc */
};
/* }}} */

/* {{{ init_libssh2_Session
 */
int
init_libssh2_Session(PyObject *dict)
{
    PYLIBSSH2_Session_Type.ob_type = &PyType_Type;
    Py_XINCREF(&PYLIBSSH2_Session_Type);
    PyDict_SetItemString(dict, "SessionType", (PyObject *)&PYLIBSSH2_Session_Type);
    
    return 1;
}
