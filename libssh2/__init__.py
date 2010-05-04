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
