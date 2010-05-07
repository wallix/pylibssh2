#!/usr/bin/env python
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
