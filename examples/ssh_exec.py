#!/usr/bin/env python
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
import socket, sys

import libssh2

DEBUG=False

usage = """Do a SSH remote command with username@hostname
Usage: sshcmd.py <hostname> <username> <password> <command>"""

def my_print(args):
    if DEBUG: print(args)

class SSHRemoteClient(object):
    def __init__(self, hostname, username, password, port=22):
        self.username = username
        self.password = password
        self.hostname = hostname
        self.port = port

        self.session = libssh2.Session()
        self.session.set_banner()

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((self.hostname,self.port))
            self.session.startup(sock)
            my_print(self.session.last_error())
            self.session.userauth_password(self.username,self.password)
            my_print(self.session.last_error())
        except Exception, e:
            print str(e)
            raise Exception, self.session.last_error()

        self.channel = self.session.open_session()
        my_print(self.session.last_error())

    def execute(self, command="uname -a"):
        datas = []
        buffer = 4096
        rc = self.channel.execute(command)
        my_print(rc)
        while True:
            data = self.channel.read(buffer)
            if data == '' or data is None: break
            my_print(type(data))
            print data.strip()

        self.channel.close()

    def __del__(self):
        self.session.close()
        my_print(self.session.last_error())

if __name__ == '__main__':
    try:
        if len(sys.argv) == 1:
            print usage
            sys.exit(1)
        src = SSHRemoteClient(sys.argv[1], sys.argv[2], sys.argv[3])
        src.execute(sys.argv[4])
    except Exception, e:
        print str(e)
    except KeyboardInterrupt, e:
        sys.exit(1)
