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
            
            hash = self.session.hostkey_hash(2)

            #print "----"
            #import base64
            #print base64.encodestring(hash)
            #print "----"

            # authentication
            self.session.userauth_password(self.username, self.password)

        except Exception, e:
            print "SSHError: Can't startup session"
            print e

    def run(self):

        try:
            # open channel
            channel = self.session.open_session()

            # request X11 Forwarding on display 0
            channel.x11_req(0)

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
