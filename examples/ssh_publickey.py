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
import os

import libssh2

DEBUG=True

usage = """Do a SSH connection with username@hostname using a public/private key
Usage: %s <hostname> <username> <path_to_private_key> [<password> [<path_to_public_key>]]""" % __file__[__file__.rfind('/')+1:]

class MySSHClient:
    def __init__(self, hostname, username, private_key, password=None, public_key=None, port=22):
        self.hostname = hostname
        self.username = username
        self.private_key = os.path.expanduser(private_key)
        self.password = password
        if not public_key is None:
            self.public_key = os.path.expanduser(public_key)
        else:
            self.public_key = public_key
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
            # To activable full debug, uncomment the following line
            # self.session.set_trace(0xff)
            self.session.set_banner()

            self.session.startup(self.sock)
            # authentication
            self.session.userauth_publickey_fromfile(self.username, self.public_key, self.private_key, self.password)

        except Exception, e:
            print "SSHError: Can't startup session"
            raise e

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
                data_to_disp = channel.poll_read(1)
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

        except (EOFError, TypeError):
            # Print a newline (in case user was sitting at prompt)
            print('')
        except Exception as e:
            print e
        finally:
            channel.close()


    def __del__(self):
        self.session.close()
        self.sock.close()

if __name__ == '__main__' :

    def argv_or(position, default):
        if len(sys.argv) > position:
            return sys.argv[position]
        else:
            return default

    if len(sys.argv) == 1:
        print usage
        sys.exit(1)

    # save terminal settings
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    
    # enable raw mode

    try: 
        myssh = MySSHClient(argv_or(1, "localhost"), argv_or(2, "root"), argv_or(3, "~/.ssh/id_dsa"),  argv_or(4, None),  argv_or(5, None))
        tty.setraw(fd)
        myssh.run()

    finally:
        print ''
        # restore terminal settings
        atexit.register(
            termios.tcsetattr,
            sys.stdin.fileno(), termios.TCSADRAIN, old_settings
        )
