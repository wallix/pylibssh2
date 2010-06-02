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
#import fcntl, signal, struct
import os
import select, sys
import subprocess
from socket import socket, AF_INET, AF_UNIX, SOCK_STREAM, SHUT_RDWR
import tty, termios

import libssh2
from libssh2 import SessionException, ChannelException

usage = """Do a X11 SSH connection with username@hostname
Usage: ssh_x11.py <hostname> <username> <password> <port>"""

def remove_node(elem):
    x11_channels.remove(elem)

def session_shutdown(session):
    session.close()
    del session

def raw_mode(fd):
    tty.setraw(fd)

def normal_mode(fd):
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

def x11_callback(session, channel, shost, sport, abstract):
    display = os.environ["DISPLAY"]
    display_port = display[display.index(":")+1]
    _path_unix_x = "/tmp/.X11-unix/X%s" % display_port
    if display[:5] == "unix:" or display[0] == ':':
        x11_sock = socket(AF_UNIX, SOCK_STREAM)
        x11_sock.connect(_path_unix_x)
    x11_channels.append(X11Channel(channel, x11_sock))

def x11_send_recieve(channel, sock):
    rc=0
    rc = channel.poll(0, 1)
    if rc > 0:
        data = channel.read(buffsize)
        sock.sendall(data)

    r, w, x = select.select([sock], [], [], 0.01)
    if r:
        data = sock.recv(buffsize)
        if data is None:
            return -1
        channel.write(data)

    if channel.eof():
        return -1
    else:
        return 0

def trace(session):
    if DEBUG and session:
        session.set_trace(
            libssh2.LIBSSH2_TRACE_TRANS|
            libssh2.LIBSSH2_TRACE_CONN|
            libssh2.LIBSSH2_TRACE_AUTH|
            libssh2.LIBSSH2_TRACE_ERROR
        )

class X11Channel(object):
    def __init__(self, chan, sock):
        self.chan = chan
        self.sock = sock

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print usage
        sys.exit(1)
    
    DEBUG=False
    x11_channels = []
    buffsize = 8192

    # save terminal settings
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    hostname = sys.argv[1]
    username = sys.argv[2]
    password = sys.argv[3]
    port = int(sys.argv[4])

    sock = socket(AF_INET, SOCK_STREAM)
    try:
        sock.connect((hostname, port))
        sock.setblocking(1)
    except Exception, e:
        print "Can't connect socket to (%s:%d): %s" % (
           hostname, port, e
        )
        sys.exit(1)

    # start session
    session = libssh2.Session()
    try:
        session.set_banner()
        # trace session on stderr if DEBUG=True
        trace(session)
        session.startup(sock)
    except SessionException, e:
        print "Can't startup session: %s" % e
        sys.exit(1)

    # register X11 callback
    session.callback_set(
        libssh2.LIBSSH2_CALLBACK_X11,
        x11_callback
    )

    try:
        session.userauth_password(username, password)
    except SessionException, e:
        print "Failed to authenticate user with %s %s" % (
            username, password
        )
        sys.exit(1)

    try:
        # open channel
        channel = session.open_session()

        # request pty
        channel.pty('xterm')

        # parse xauth data
        p = subprocess.Popen(
            ['xauth','list'], shell=False, stdout=subprocess.PIPE
        )
        xauth_data = p.communicate()[0]
        auth_protocol, auth_cookie = xauth_data.split()[1:]


        # request X11 forwarding on display 0
        channel.x11_req(0, auth_protocol, auth_cookie, 0)

        # request shell
        channel.shell()

        # enable raw mode
        raw_mode(fd)

        while True:
            # XXX tty resize
            #win = struct.unpack(
            #    'hh',
            #    fcntl.ioctl(fd, termios.TIOCGWINSZ, 4*' ')
            #)
            # width, height
            #channel.pty_resize(win[1], win[0])

            # polling on tty channel
            rc = channel.poll(0, 1)
            if rc > 0:
                data = channel.read(buffsize)
                sys.stdout.write(data)
            else:
                sys.stdout.flush()

            # polling on x11 channels
            if len(x11_channels) != 0:
                for x11_channel in x11_channels:
                    rc = x11_send_recieve(x11_channel.chan, x11_channel.sock)
                    if rc == -1:
                        x11_channel.sock.shutdown(SHUT_RDWR)
                        x11_channel.sock.close()
                        remove_node(x11_channel)

            r, w, x = select.select([fd], [], [], 0.01)
            if sys.stdin.fileno() in r:
                data = sys.stdin.read(1).replace('\n','\r\n')
                channel.write(data)

            if channel.eof():
                break

    except ChannelException, e:
        print "Channel exception: %s" % e
    finally:
        channel.close()

    session_shutdown(session)
    sock.close()

    # restore terminal settings
    atexit.register(
        normal_mode, fd
    )
