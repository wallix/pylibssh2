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
__doc__ = """Python binding for libssh2 library"""

from version import *

from channel import ChannelException, Channel
from session import SessionException, Session
from sftp import SftpException, Sftp

__all__ = [
    'Channel',
    'ChannelException',
    'Session',
    'SessionException',
    'Sftp',
    'SftpException'
]

LIBSSH2_TRACE_TRANS = 1<<1
LIBSSH2_TRACE_KEX = 1<<2
LIBSSH2_TRACE_AUTH = 1<<3
LIBSSH2_TRACE_CONN = 1<<4
LIBSSH2_TRACE_SCP = 1<<5
LIBSSH2_TRACE_SFTP = 1<<6
LIBSSH2_TRACE_ERROR = 1<<7
LIBSSH2_TRACE_PUBLICKEY = 1<<8
LIBSSH2_TRACE_SOCKET = 1<<9

LIBSSH2_CALLBACK_X11 = 4
