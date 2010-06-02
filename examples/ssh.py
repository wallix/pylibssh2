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
import atexit

import select, socket, sys
import tty, termios

import libssh2

usage = """Do a SSH connection with username@hostname
Usage: ssh.py <hostname> <username> <password>"""

class MySSHClient:
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
            
            # authentication
            self.session.userauth_password(self.username, self.password)

        except Exception, e:
            print "SSHError: Can't startup session"
            print e

    def run(self):

        try:
            # open channel
            channel = self.session.open_session()

            # request pty
            channel.pty('vt100')

            # request shell
            channel.shell()
            channel.setblocking(0)

            # loop
            while True:
                data_to_disp = channel.poll(0, 1)
                if data_to_disp > 0:
                    data = channel.read(1024)
                    if data is not None:
                        sys.stdout.write(data)
                    else:
                        break
                    sys.stdout.flush()
                    
                r,w,x = select.select([fd],[],[],0.01)
                if sys.stdin.fileno() in r:
                    data = sys.stdin.read(1).replace('\n','\r\n')
                    channel.write(data)

        except Exception,e:
            print e
        finally:
            channel.close()


    def __del__(self):
        self.session.close()
        self.sock.close()

if __name__ == '__main__' :
    if len(sys.argv) == 1:
        print usage
        sys.exit(1)

    # save terminal settings
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    
    # enable raw mode
    tty.setraw(fd)

    myssh = MySSHClient(sys.argv[1],sys.argv[2], sys.argv[3])
    myssh.run()

    # restore terminal settings
    atexit.register(
        termios.tcsetattr,
        sys.stdin.fileno(), termios.TCSADRAIN, old_settings
    )
