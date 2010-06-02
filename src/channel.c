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


/* {{{ PYLIBSSH2_Channel_close
 */
static char PYLIBSSH2_Channel_close_doc[] = "\n\
close() -> int\n\
\n\
Closes the active channel.\n\
\n\
@param channel\n\
@type libssh2.Channel\n\
\n\
@return 0 on success or negative on failure\n\
@rtype int";

static PyObject *
PYLIBSSH2_Channel_close(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_close(self->channel);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_CHANNEL_CANT_CLOSE_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to close the channel.");
        return NULL; 
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_pty
 */
static char PYLIBSSH2_Channel_pty_doc[] = "\n\
pty(term) -> int\n\
\n\
Requests a pty with term type on a channel.\n\
\n\
@param  term: terminal emulation type (vt100, ansi, etc...)\n\
@type   term: str\n\
\n\
@return 0 on succes or negative on failure\n\
@rtype  int";

/*
 * refer libssh2_channel_request_pty_ex()
 */
static PyObject *
PYLIBSSH2_Channel_pty(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    /* terminal type */
    char *term;
    /* terminal type length */
    int term_len;
    /* terminal modes capabilities */
    char *modes = NULL;
    /* modes length */
    int modes_len = 0;
    /* terminal dimensions in characters */
    int width = 80, height = 24;
    /* default dimensions */
    int width_px = 0, height_px = 0;

    if (!PyArg_ParseTuple(args, "s#|s#iiii:pty", &term, &term_len, &modes,
                          &modes_len, &width, &height, &width_px, &height_px)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_request_pty_ex(self->channel, term, term_len, modes, modes_len,
                                        width, height, width_px, height_px);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_CHANNEL_PTY_FAILED_MSG */ 
        PyErr_SetString(PYLIBSSH2_Error, "Failed to request pty.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
} 
/* }}} */

/* {{{ PYLIBSSH2_Channel_pty_resize
 */
static char PYLIBSSH2_Channel_pty_resize_doc[] = "\n\
pty_resize(width, height) -> int\n\
\n\
Requests a pty resize on a channel with the given width and height.\n\
\n\
@param  width: terminal width\n\
@type   width: int\n\
@param  height: terminal height\n\
@type   height: int\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

/*
 * refer libssh2_channel_request_pty_size_ex()
 */
static PyObject *
PYLIBSSH2_Channel_pty_resize(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc = -1;

    /* terminal dimensions in characters */
    int width  = 80;
    int height = 24;

    /* terminal dimensions in pixels */
    int width_px  = 0;
    int height_px = 0;

    if (!PyArg_ParseTuple(args,"ii|ii:pty_resize", &width,&height,
                                                    &width_px,&height_px)){
        return NULL;
    }
    /* FIXME: For unknown reason, the activation of the two following methods
     * provokes a core dump of the lib with the following error:
     * PyEval_RestoreThread: NULL tstate
     */
    /*MY_BEGIN_ALLOW_THREADS(self->tstate);*/
    rc = libssh2_channel_request_pty_size_ex(self->channel, width, height,
                                                width_px, height_px);
    /*MY_END_ALLOW_THREADS(self->tstate);*/

    if (rc) {
        PyErr_SetString(PYLIBSSH2_Error, "Failed to resize pty");
        return NULL;
    }

    return Py_BuildValue("i",rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_shell
 */
static char PYLIBSSH2_Channel_shell_doc[] = "\n\
shell() -> int\n\
\n\
Requests a shell on the channel.\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_shell(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_shell(self->channel);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_CHANNEL_CANT_REQUEST_SHELL_MSG */
        PyErr_SetString(PYLIBSSH2_Error,"Unable to request shell on allocated pty.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_execute
 */
static char PYLIBSSH2_Channel_execute_doc[] = "\n\
execute(command) -> int\n\
\n\
Executes command on the channel.\n\
\n\
@param  command: message data\n\
@type   command: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_execute(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    /* command to execute */
    char *command;

    if (!PyArg_ParseTuple(args, "s:execute", &command))
        return NULL;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_exec(self->channel, command);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_CANT_REQUEST_EXEC_COMMAND_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to request exec command.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_setenv
 */
static char PYLIBSSH2_Channel_setenv_doc[] = "\n\
setenv(name, value) -> int\n\
\n\
Sets envrionment variable on the channel.\n\
\n\
@param name: evironment variable name\n\
@type  name: str\n\
@param value: evironment variable value\n\
@type  value: str\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_setenv(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    /* variable envrionnement name */
    char *env_key;
    /* variable envrionnement value */
    char *env_val;

    if (!PyArg_ParseTuple(args, "ss:setenv", &env_key, &env_val))
        return NULL;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_setenv(self->channel, env_key, env_val);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_CANT_SET_ENVRIONNEMENT_VARIABLE_MSG */
        PyErr_SetString(PYLIBSSH2_Error,"Unable to set envrionnement variable.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_setblocking
 */
static char PYLIBSSH2_Channel_setblocking_doc[] = "\n\
setblocking([mode])\n\
\n\
Sets blocking mode on the channel. Default mode is blocking.\n\
\n\
@param  mode: blocking (1) or non blocking (0) mode\n\
@type   mode: int";

static PyObject *
PYLIBSSH2_Channel_setblocking(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    /* 1 blocking, 0 non blocking */
    int block = 1;

    if (!PyArg_ParseTuple(args, "i:setblocking", &block))
        return NULL;

    libssh2_channel_set_blocking(self->channel, block);
    Py_INCREF(Py_None);

    return Py_None;
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_read
 */
static char PYLIBSSH2_Channel_read_doc[] = "\n\
read(size) -> str\n\
\n\
Reads size bytes on the channel.\n\
\n\
@param size: size of the buffer storage\n\
@type  size: int\n\
\n\
@return bytes read or negative on failure\n\
@rtype  str";

static PyObject *
PYLIBSSH2_Channel_read(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    int buffer_size;
    /* buffer to read as a python object */
    PyObject *buffer;

    if (!PyArg_ParseTuple(args, "i|i:read", &buffer_size))
        return NULL;

    buffer = PyString_FromStringAndSize(NULL, buffer_size);
    if (buffer == NULL) {
        return NULL;
    }

    if (libssh2_channel_eof(self->channel) != 1) {
        MY_BEGIN_ALLOW_THREADS(self->tstate);
        rc = libssh2_channel_read(self->channel, PyString_AsString(buffer),
                                  buffer_size);
        MY_END_ALLOW_THREADS(self->tstate);

        if (rc > 0) {
            if (rc != buffer_size && _PyString_Resize(&buffer, rc) < 0)
                return NULL;
            return buffer;
        }
    }

    Py_XDECREF(buffer);
    Py_INCREF(Py_None);

    return Py_None;
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_write
 */
static char PYLIBSSH2_Channel_write_doc[] = "\n\
write(message) -> int\n\
\n\
Writes data on a channel.\n\
\n\
@param  message: data to write\n\
@type   message: str\n\
\n\
@return 0 on success or failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_write(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    char *message;
    int message_len;

    if (!PyArg_ParseTuple(args, "s#:write", &message, &message_len))
        return NULL;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_write(self->channel, message, message_len);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_CANT_WRITE_CHANNEL_MSG */
        PyErr_SetString(PYLIBSSH2_Error,"Unable to write channel.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_flush
 */
static char PYLIBSSH2_Channel_flush_doc[] = "\n\
flush() -> int\n\
\n\
Flushs the read buffer for a given channel.\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_flush(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_flush(self->channel);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_CANT_FLUSH_CHANNEL_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to flush channel.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_exit_status
 */
static char PYLIBSSH2_Channel_exit_status_doc[] = "\n\
exit_status() -> int \n\
\n\
Gets the exit code raised by the process running on the remote host.\n\
\n\
@return the exit status reported by remote host or 0 on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_exit_status(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    return Py_BuildValue("i", libssh2_channel_get_exit_status(self->channel));
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_eof
 */
static char PYLIBSSH2_Channel_eof_doc[] = "\n\
eof() -> int\n\
\n\
Checks if the remote host has sent an EOF status.\n\
\n\
@return 1 if the remote host has sent EOF otherwise 0\n\
@rtype   int";

static PyObject *
PYLIBSSH2_Channel_eof(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    return Py_BuildValue("i", libssh2_channel_eof(self->channel));
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_send_eof
 */
static char PYLIBSSH2_Channel_send_eof_doc[] = "\n\
send_eof() -> int\n\
\n\
Sends EOF status on the channel to remote server.\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_send_eof(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_send_eof(self->channel);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_CANT_SEND_EOF_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to send a EOF on channel.");
        return NULL;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_window_read
 */
static char PYLIBSSH2_Channel_window_read_doc[] = "\n\
window_read(read_avail, window_size_initial) -> int\n\
\n\
Checks the status of the read window.\n\
\n\
@param  read_avail: \n\
@type   read_avail: int\n\
@param  window_size_initial: \n\
@type   window_size_initial: int\n\
\n\
@return the number of bytes which the remote end may send without overflowing\n\
the window limit\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_window_read(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    unsigned long rc;
    unsigned long read_avail;
    unsigned long window_size_initial;

    if (!PyArg_ParseTuple(args, "|ii:window_read", &read_avail, &window_size_initial)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_window_read_ex(self->channel, &read_avail, &window_size_initial);
    MY_END_ALLOW_THREADS(self->tstate);

    return PyInt_FromLong(rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_poll
 */
static char PYLIBSSH2_Channel_poll_doc[] = "\n\
poll(timeout, [nfds]) -> int\n\
\n\
Polls for activity on the channel.\n\
\n\
@param  timeout: remaining timeout\n\
@type   timeout: int\n\
@param  nfds: number of fds to poll\n\
@type   nfds: int\n\
\n\
@return number of fds with interesting events or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_poll(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    int nfds = 1;
    long timeout = 0;
    LIBSSH2_POLLFD fds;

    if (!PyArg_ParseTuple(args, "i|i:poll", &timeout, &nfds)) {
        return NULL;
    }

    fds.type = LIBSSH2_POLLFD_CHANNEL;
    fds.fd.channel = self->channel; /*see PYLIBSSH2_CHANNEL struct*/
    fds.events  = LIBSSH2_POLLFD_POLLIN;

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_poll(&fds, nfds, timeout);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_x11_req
 */
static char PYLIBSSH2_Channel_x11_req_doc[] = "\n\
x11_req([single_connection, auth_proto, auth_cookie, display]) -> int\n\
\n\
Requests an X11 Forwarding on the channel.\n\
\n\
@param  single_connection: forward a single connection\n\
@type  single_connection: int\n\
@param  auth_proto: X11 authentication protocol to use\n\
@type  auth_proto: str\n\
@param  auth_cookie: the cookie\n\
@type  auth_cookie: str\n\
@param  display: screen number\n\
@type  display: int\n\
\n\
@return 0 on success or negative on failure\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_x11_req(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    int single_connection = 0;
    char *auth_proto = NULL;
    char *auth_cookie = NULL;
    int display = 0;

    if (!PyArg_ParseTuple(args, "|issi:poll_x11_req", &single_connection, &auth_proto, &auth_cookie, &display)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_channel_x11_req_ex(self->channel, single_connection, auth_proto, auth_cookie, display);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_poll_read
 */
static char PYLIBSSH2_Channel_poll_read_doc[] = "\n\
poll_read([extended]) -> int\n\
\n\
Checks if data is available on the channel.\n\
\n\
@param  extended: if message channel datas is extended\n\
@type   extended: int\n\
\n\
@return 1 when data is available or 0 otherwise\n\
@rtype  int";

static PyObject *
PYLIBSSH2_Channel_poll_read(PYLIBSSH2_CHANNEL *self, PyObject *args)
{
    int rc;
    int extended = 0;

    if (!PyArg_ParseTuple(args, "|i:poll_read", &extended)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_poll_channel_read(self->channel, extended);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_methods[]
 *
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 *  { 'name', (PyCFunction)PYLIBSSH2_Channel_name, METHOD_VARARGS }
 *  for convenience
 */

#define ADD_METHOD(name) \
{ #name, (PyCFunction)PYLIBSSH2_Channel_##name, METH_VARARGS, PYLIBSSH2_Channel_##name##_doc }
static PyMethodDef PYLIBSSH2_Channel_methods[] = 
{
    ADD_METHOD(close),
    ADD_METHOD(pty),
    ADD_METHOD(pty_resize),
    ADD_METHOD(shell),
    ADD_METHOD(execute),
    ADD_METHOD(setenv),
    ADD_METHOD(setblocking),
    ADD_METHOD(read),
    ADD_METHOD(write),
    ADD_METHOD(flush),
    ADD_METHOD(eof),
    ADD_METHOD(send_eof),
    ADD_METHOD(window_read),
    ADD_METHOD(poll_read),
    ADD_METHOD(poll),
    ADD_METHOD(x11_req),
    ADD_METHOD(exit_status),
    { NULL, NULL }
};
#undef ADD_METHOD
/* }}} */

/* {{{ PYLIBSSH2_Channel_New
 */
PYLIBSSH2_CHANNEL *
PYLIBSSH2_Channel_New(LIBSSH2_CHANNEL *channel, int dealloc)
{
    PYLIBSSH2_CHANNEL *self;

    self = PyObject_New(PYLIBSSH2_CHANNEL, &PYLIBSSH2_Channel_Type);
    if (self == NULL) {
        return NULL;
    }

    self->channel = channel;
    self->tstate = NULL;
    self->dealloc = dealloc;

    return self;
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_dealloc
 */
static void
PYLIBSSH2_Channel_dealloc(PYLIBSSH2_CHANNEL *self)
{
    PyObject_Del(self);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_getattr
 */
static PyObject *
PYLIBSSH2_Channel_getattr(PYLIBSSH2_CHANNEL *self, char *name)
{
    return Py_FindMethod(PYLIBSSH2_Channel_methods, (PyObject *)self, name);
}
/* }}} */

/* {{{ PYLIBSSH2_Channel_Type
 * see /usr/include/python2.5/object.h line 261
 */
PyTypeObject PYLIBSSH2_Channel_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                      /* ob_size */
    "Channel",                              /* tp_name */
    sizeof(PYLIBSSH2_CHANNEL),              /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor)PYLIBSSH2_Channel_dealloc,  /* tp_dealloc */
    0,                                      /* tp_print */
    (getattrfunc)PYLIBSSH2_Channel_getattr, /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash  */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    "Channel objects",                      /* tp_doc */
};
/* }}} */

 /* {{{ init_libssh2_Channel
 */
int
init_libssh2_Channel(PyObject *dict)
{
    PYLIBSSH2_Channel_Type.ob_type = &PyType_Type;
    Py_XINCREF(&PYLIBSSH2_Channel_Type);
    PyDict_SetItemString(dict, "ChannelType", (PyObject *)&PYLIBSSH2_Channel_Type);
     
    return 1;
}
/* }}} */
