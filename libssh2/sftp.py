#
# Copyright (c) 2010 Wallix Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the author nor the names of any co-contributors
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
