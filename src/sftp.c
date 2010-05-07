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

/* {{{ get_attrs
 */
PyObject *
get_attrs(LIBSSH2_SFTP_ATTRIBUTES *attr)
{
    PyObject *attrs=NULL;

    attrs = PyList_New(0);
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->filesize));
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->uid));
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->gid));
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->permissions));
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->atime));
    PyList_Append(attrs, PyLong_FromUnsignedLong(
        (unsigned long)attr->mtime));

    return attrs; 
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_close
 */
static char PYLIBSSH2_Sftp_close_doc[] = "\n\
myfunction(name, value) -> returnType \n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_close(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    PYLIBSSH2_SFTPHANDLE *handle;

    if (!PyArg_ParseTuple(args, "O:close", &handle)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_close_handle(handle->sftphandle);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc) {
        /* CLEAN: PYLIBSSH2_SFTPHANDLE_CANT_CLOSE_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to close sftp handle.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_opendir
 */
static char PYLIBSSH2_Sftp_opendir_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_opendir(PYLIBSSH2_SFTP *self, PyObject *args)
{
    LIBSSH2_SFTP_HANDLE *handle;
    char *path;

    if (!PyArg_ParseTuple(args, "s:opendir", &path)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    handle = libssh2_sftp_opendir(self->sftp, path);
    MY_END_ALLOW_THREADS(self->tstate);

    if (handle == NULL) {
        /* CLEAN: PYLIBSSH2_SFTPHANDLE_CANT_OPENDIR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to open sftp directory.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)PYLIBSSH2_Sftphandle_New(handle, 1);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_readdir
 */
static char PYLIBSSH2_Sftp_readdir_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_readdir(PYLIBSSH2_SFTP *self, PyObject *args)
{
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    PYLIBSSH2_SFTPHANDLE *handle;
    int buffer_maxlen = 0;
    int longentry_maxlen = 255;
    PyObject *buffer;
    PyObject *list;

    if (!PyArg_ParseTuple(args, "O:readdir", &handle)) {
        return NULL;
    }

    buffer = PyString_FromStringAndSize(NULL, longentry_maxlen);
    if (buffer == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    buffer_maxlen = libssh2_sftp_readdir(handle->sftphandle, PyString_AsString(buffer),
                                         longentry_maxlen, &attrs);
    MY_END_ALLOW_THREADS(self->tstate);

    if (buffer_maxlen == 0) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if ( buffer_maxlen == -1) {
        /* CLEAN: PYLIBSSH2_SFTPHANDLE_CANT_READDIR_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to readdir.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    if (buffer_maxlen != longentry_maxlen && 
        _PyString_Resize(&buffer, buffer_maxlen) < 0) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    list = PyList_New(0);
    PyList_Append(list, buffer);
    PyList_Append(list, get_attrs(&attrs));

    return list;
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_listdir
 */
static char PYLIBSSH2_Sftp_listdir_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_listdir(PYLIBSSH2_SFTP *self, PyObject *args)
{
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    PYLIBSSH2_SFTPHANDLE *handle;
    int buffer_maxlen = 0;
    int longentry_maxlen = 255;
    PyObject *buffer;
    PyObject *all = NULL;
    PyObject *list = NULL;

    if (!PyArg_ParseTuple(args, "O:listdir", &handle)) {
        return NULL;
    }

    all = PyList_New(0);
    while (1) {
        buffer = PyString_FromStringAndSize(NULL, longentry_maxlen);
        if (buffer == NULL) {
            Py_INCREF(Py_None);
            return Py_None;
        }

        MY_BEGIN_ALLOW_THREADS(self->tstate);
        buffer_maxlen = libssh2_sftp_readdir(handle->sftphandle, 
            PyString_AsString(buffer), longentry_maxlen, &attrs);
        MY_END_ALLOW_THREADS(self->tstate);

        if (buffer_maxlen == 0) { 
            break; 
        } else if (buffer_maxlen == -1) {
            PyErr_SetString(PYLIBSSH2_Error, "Unable to listdir.");
            Py_INCREF(Py_None);
            return Py_None;
        }

        if ( buffer_maxlen != longentry_maxlen && 
             _PyString_Resize(&buffer, buffer_maxlen) < 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }

        list = PyList_New(0);
        PyList_Append(list, buffer);
        PyList_Append(list, get_attrs(&attrs));
        PyList_Append(all, list);
    }

    return all;
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_open
 */
static char PYLIBSSH2_Sftp_open_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_open(PYLIBSSH2_SFTP *self, PyObject *args)
{
    LIBSSH2_SFTP_HANDLE *handle;
    char *path;
    char *flags = "r";
    long mode = 0755;

    if (!PyArg_ParseTuple(args, "s|si:open", &path, &flags, &mode)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    handle = libssh2_sftp_open(self->sftp, path, get_flags(flags), mode);
    MY_END_ALLOW_THREADS(self->tstate);

    if (handle == NULL) {
        /* CLEAN: PYLIBSSH2_SFTP_CANT_OPEN_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to sftp open.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return (PyObject *)PYLIBSSH2_Sftphandle_New(handle, 1);
    
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_shutdown
 */
static char PYLIBSSH2_Sftp_shutdown_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_shutdown(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;

    rc=libssh2_sftp_shutdown(self->sftp);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_SFTP_CANT_SHUTDOWN_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to shutdown sftp.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_read
 */
static char PYLIBSSH2_Sftp_read_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_read(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    int buffer_maxlen;
    PyObject *buffer;
    PYLIBSSH2_SFTPHANDLE *handle;

    if (!PyArg_ParseTuple(args, "Oi:read", &handle, &buffer_maxlen)) {
        return NULL;
    }

    buffer = PyString_FromStringAndSize(NULL, buffer_maxlen);
    if (buffer == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_read(handle->sftphandle, PyString_AsString(buffer),
                           buffer_maxlen);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc > 0) {
        if ( rc != buffer_maxlen && _PyString_Resize(&buffer, rc) < 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        return buffer;
    }

    Py_XDECREF(buffer);
    Py_INCREF(Py_None);

    return Py_None;
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_write
 */
static char PYLIBSSH2_Sftp_write_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_write(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc, buffer_len;
    char *buffer;
    PYLIBSSH2_SFTPHANDLE *handle;

    if (!PyArg_ParseTuple(args, "Os#:write", &handle, &buffer, &buffer_len)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_write(handle->sftphandle, buffer, buffer_len);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc < 0) {
        /* CLEAN: PYLIBSSH2_Sftp_CANT_WRITE_MSG */
        PyErr_Format(PYLIBSSH2_Error, "Unable to write sftp.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
    
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_tell
 */
static char PYLIBSSH2_Sftp_tell_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_tell(PYLIBSSH2_SFTP *self, PyObject *args)
{
    PYLIBSSH2_SFTPHANDLE *handle;

    if (!PyArg_ParseTuple(args, "O:tell", &handle)) {
        return NULL;
    }

    return PyInt_FromLong(libssh2_sftp_tell(handle->sftphandle));
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_seek
 */
static char PYLIBSSH2_Sftp_seek_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_seek(PYLIBSSH2_SFTP *self, PyObject *args)
{
    PYLIBSSH2_SFTPHANDLE *handle;
    unsigned long offset=0;

    if (!PyArg_ParseTuple(args, "Ok:seek", &handle, &offset)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    libssh2_sftp_seek(handle->sftphandle, offset);
    MY_END_ALLOW_THREADS(self->tstate);

    return PyInt_FromLong(1);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_unlink
 */
static char PYLIBSSH2_Sftp_unlink_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_unlink(PYLIBSSH2_SFTP *self, PyObject *args)
{

    int rc;
    char *path;

    if (!PyArg_ParseTuple(args, "s:unlink", &path)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_unlink(self->sftp, path);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_rename
 */
static char PYLIBSSH2_Sftp_rename_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_rename(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *src, *dst;
    
    if (!PyArg_ParseTuple(args, "ss:rename", &src, &dst)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_rename(self->sftp, src, dst);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_mkdir
 */
static char PYLIBSSH2_Sftp_mkdir_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_mkdir(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *path;
    long mode = 0755;
    
    if (!PyArg_ParseTuple(args, "s|i:mkdir", &path, &mode)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_mkdir(self->sftp, path, mode);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_rmdir
 */
static char PYLIBSSH2_Sftp_rmdir_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_rmdir(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *path;

    if (!PyArg_ParseTuple(args, "s:rmdir", &path)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_rmdir(self->sftp, path);
    MY_END_ALLOW_THREADS(self->tstate);

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_realpath
 */
static char PYLIBSSH2_Sftp_realpath_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_realpath(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc, path_len = 0, target_len = 1024;
    int type = LIBSSH2_SFTP_REALPATH;
    char *path;
    PyObject *target;

    if (!PyArg_ParseTuple(args, "s#|i:realpath", &path, &path_len, &type)) {
        return NULL;
    }

    target = PyString_FromStringAndSize(NULL, target_len);
    if (target == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_symlink_ex(self->sftp, path, path_len, 
            PyString_AsString(target), target_len, type);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc > 0) {
        if (rc != target_len && _PyString_Resize(&target, rc) < 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        return target;
    }

    Py_XDECREF(target);
    Py_INCREF(Py_None);

    return Py_None;
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_symlink
 */
static char PYLIBSSH2_Sftp_symlink_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_symlink(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *path, *target;

    if (!PyArg_ParseTuple(args, "ss:symlink", &path, &target)) {
        return NULL;
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_symlink(self->sftp, path, target);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_SFTP_CANT_SYMLINK_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to sftp symlink.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_get_stat
 */
static char PYLIBSSH2_Sftp_get_stat_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_get_stat(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *path;
    int path_len = 0;
    int type = LIBSSH2_SFTP_STAT;
    LIBSSH2_SFTP_ATTRIBUTES attr;
    
    if (!PyArg_ParseTuple(args, "s#|i:get_stat", &path, &path_len, &type)) {
        return NULL; 
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_stat_ex(self->sftp, path, path_len, type, &attr);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        /* CLEAN: PYLIBSSH2_SFTP_CANT_GETSTAT_MSG */
        PyErr_SetString(PYLIBSSH2_Error, "Unable to get stat.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return get_attrs(&attr);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_set_stat
 */
static char PYLIBSSH2_Sftp_set_stat_doc[] = "\n\
\n\
Arguments:\n\
\n\
Returns:\n\
";

static PyObject *
PYLIBSSH2_Sftp_set_stat(PYLIBSSH2_SFTP *self, PyObject *args)
{
    int rc;
    char *path;
    LIBSSH2_SFTP_ATTRIBUTES attr;
    PyObject *attrs;

    if (!PyArg_ParseTuple(args, "sO:set_stat", &path, &attrs)) {
        return NULL;
    }

    attr.flags = 0;
    if (PyMapping_HasKeyString(attrs, "perms")) {
        attr.flags |= LIBSSH2_SFTP_ATTR_PERMISSIONS;
        attr.permissions = PyLong_AsLong(PyDict_GetItemString(attrs, "perms"));
    }

    if (PyMapping_HasKeyString(attrs, "uid") && 
        PyMapping_HasKeyString(attrs, "gid")) {
        if (PyMapping_HasKeyString(attrs, "uid")) {
            attr.flags |= LIBSSH2_SFTP_ATTR_UIDGID;
            attr.uid = PyLong_AsLong(PyDict_GetItemString(attrs,"uid"));
        }
        if (PyMapping_HasKeyString(attrs, "gid")) {
            attr.flags |= LIBSSH2_SFTP_ATTR_UIDGID;
            attr.uid = PyLong_AsLong(PyDict_GetItemString(attrs,"gid"));
        }
    }

    if (PyMapping_HasKeyString(attrs, "atime") &&
        PyMapping_HasKeyString(attrs, "mtime")) {
        if (PyMapping_HasKeyString(attrs, "atime")) {
            attr.flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
            attr.uid = PyLong_AsLong(PyDict_GetItemString(attrs,"atime"));
        }
        if (PyMapping_HasKeyString(attrs, "mtime")) {
            attr.flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
            attr.uid = PyLong_AsLong(PyDict_GetItemString(attrs,"mtime"));
        }
    }

    MY_BEGIN_ALLOW_THREADS(self->tstate);
    rc = libssh2_sftp_setstat(self->sftp, path, &attr);
    MY_END_ALLOW_THREADS(self->tstate);

    if (rc == -1) {
        PyErr_SetString(PYLIBSSH2_Error, "Unable to stat.");
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("i", rc);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_methods[]
 *
 * ADD_METHOD(name) expands to a correct PyMethodDef declaration
 *   {  'name', (PyCFunction)PYLIBSSH2_Sftp_name, METH_VARARGS }
 * for convenience
 */
#define ADD_METHOD(name) \
{ #name, (PyCFunction)PYLIBSSH2_Sftp_##name, METH_VARARGS, PYLIBSSH2_Sftp_##name##_doc }

static PyMethodDef PYLIBSSH2_Sftp_methods[] =
{
    ADD_METHOD(close),
    ADD_METHOD(opendir),
    ADD_METHOD(readdir),
    ADD_METHOD(listdir),
    ADD_METHOD(open),
    ADD_METHOD(shutdown),
    ADD_METHOD(read),
    ADD_METHOD(write),
    ADD_METHOD(tell),
    ADD_METHOD(seek),
    ADD_METHOD(unlink),
    ADD_METHOD(rename),
    ADD_METHOD(mkdir),
    ADD_METHOD(rmdir),
    ADD_METHOD(realpath),
    ADD_METHOD(symlink),
    ADD_METHOD(get_stat),
    ADD_METHOD(set_stat),
    { NULL, NULL }
};
#undef ADD_METHOD
/* }}} */


/* {{{ PYLIBSSH2_Sftp_New
 */
PYLIBSSH2_SFTP *
PYLIBSSH2_Sftp_New(LIBSSH2_SFTP *sftp, int dealloc)
{
    PYLIBSSH2_SFTP *self;

    self = PyObject_New(PYLIBSSH2_SFTP, &PYLIBSSH2_Sftp_Type);
    if (self == NULL) {
        return NULL;
    }

    self->sftp = sftp;
    self->dealloc = dealloc;
    self->tstate = NULL;

    return self;
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_dealloc
 */
static void
PYLIBSSH2_Sftp_dealloc(PYLIBSSH2_SFTP *self)
{
    PyObject_Del(self);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_getattr
 */
static PyObject *
PYLIBSSH2_Sftp_getattr(PYLIBSSH2_SFTP *self, char *name)
{
    return Py_FindMethod(PYLIBSSH2_Sftp_methods, (PyObject *)self, name);
}
/* }}} */

/* {{{ PYLIBSSH2_Sftp_Type
 *
 * see /usr/include/python2.5/object.h line 261
 */
PyTypeObject PYLIBSSH2_Sftp_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                       /* ob_size */
    "Sftp",                                  /* tp_name */
    sizeof(PYLIBSSH2_SFTP),                  /* tp_basicsize */
    0,                                       /* tp_itemsize */
    (destructor)PYLIBSSH2_Sftp_dealloc,      /* tp_dealloc */
    0,                                       /* tp_print */
    (getattrfunc)PYLIBSSH2_Sftp_getattr,     /* tp_getattr */
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
    "Sftp objects",                          /* tp_doc */
};
/* }}} */

/* {{{ init_libssh2_Sftp
 */
int
init_libssh2_Sftp(PyObject *dict)
{
    PYLIBSSH2_Sftp_Type.ob_type = &PyType_Type;
    Py_XINCREF(&PYLIBSSH2_Sftp_Type);
    PyDict_SetItemString(dict, "SFTPType", (PyObject *) &PYLIBSSH2_Sftp_Type);

    return 1;
}
/* }}} */
