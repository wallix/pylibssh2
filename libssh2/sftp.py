#
# pylibssh2 - python bindings for libssh2 library
#
# Copyright (C) 2010 Wallix Inc.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
"""
Abstraction for libssh2 L{Sftp} object
"""

class SftpException(Exception):
    """
    Exception raised when L{Sftp} actions fails.
    """
    pass

class Sftp(object):
    """
    Sftp object
    """
    def __init__(self):
        """
        Create a new Sftp object.
        """
        pass

    def close(self):
        """
        """
        raise NotImplementedError()

    def opendir(self):
        """
        """
        raise NotImplementedError()

    def readdir(self):
        """
        """
        raise NotImplementedError()

    def listdir(self):
        """
        """
        raise NotImplementedError()

    def open(self):
        """
        """
        raise NotImplementedError()


    def shutdown(self):
        """
        """
        raise NotImplementedError()

    def read(self):
        """
        """
        raise NotImplementedError()

    def write(self):
        """
        """
        raise NotImplementedError()

    def tell(self):
        """
        """
        raise NotImplementedError()

    def seek(self):
        """
        """
        raise NotImplementedError()

    def unlink(self):
        """
        """
        raise NotImplementedError()

    def rename(self):
        """
        """
        raise NotImplementedError()

    def mkdir(self):
        """
        """
        raise NotImplementedError()

    def rmdir(self):
        """
        """
        raise NotImplementedError()

    def realpath(self):
        """
        """
        raise NotImplementedError()

    def symlink(self):
        """
        """
        raise NotImplementedError()

    def getstat(self):
        """
        """
        raise NotImplementedError()

    def setstat(self):
        """
        """
        raise NotImplementedError()
