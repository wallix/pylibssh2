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

usage = """Do a SFTP file listing of <directory> with username@hostname
Usage: sftp.py <hostname> <username> <password> <directory>"""

class MySFTPClient:
    def __init__(self, hostname, username, password, port=22):
        self.hostname = hostname
        self.username = username
        self.password = password
        self.port = port
        self._prepare_sock()

    def _prepare_sock(self):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.hostname, self.port))
            self.sock.setblocking(1)
        except Exception, e:
            print "SockError: Can't connect socket to %s:%d" % (self.hostname, self.port)
            print e

        try:
            self.session = libssh2.Session()
            self.session.set_banner()
            self.session.startup(self.sock)
            self.session.userauth_password(self.username, self.password)
        except Exception, e:
            print "SSHError: Can't startup session"
            print e

        # use low level layer because we don't yet provide High layer for sftp
        self.sftp = self.session._session.sftp_init()

    def listdir(self, remote_path='/tmp'):
        handle = self.sftp.opendir(remote_path)
        if handle:
            while True:
                data = self.sftp.readdir(handle)
                if not data: break
                print data

            for file, attribute in self.sftp.listdir(handle):
                print file, attribute

        self.sftp.close(handle)

    def __del__(self):
        self.session.close()
        self.sock.close()

if __name__ == '__main__' :
    if len(sys.argv) == 1:
        print usage
        sys.exit(1)
    mysftp = MySFTPClient(
        hostname=sys.argv[1],
        username=sys.argv[2], 
        password=sys.argv[3]
    )
    mysftp.listdir(sys.argv[4])
