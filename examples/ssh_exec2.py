#!/usr/bin/env python
import sys

import socket
from select import select

import libssh2


DEBUG = False

usage = """Do a SSH remote command with username@hostname
Usage: %s <hostname> <username> <password> <command>""" % __file__[__file__.rfind('/')+1:]


def my_print(args):
    if DEBUG: print(args)

class SSHRemoteClientNonBlocking(object):
    LIBSSH2_ERROR_EAGAIN= -37

    def __init__(self, hostname, username, password, port=22):
        self.username = username
        self.password = password
        self.hostname = hostname
        self.port = port
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setblocking(1)
        self.sock.connect_ex((self.hostname, self.port))
        self.sock.setblocking(0)        

        self.session = libssh2.Session()
        self.session.setblocking(0)

    def _wait_select(self):
        '''
        Find out from libssh2 if its blocked on read or write and wait accordingly
        Return immediately if libssh2 is not blocked
        '''
        blockdir = self.session.blockdirections()
        if blockdir == 0:
            # return if not blocked
            return

        readfds = [self.sock] if (blockdir & 01) else []
        writefds = [self.sock] if (blockdir & 02) else []
        select(readfds, writefds, [])        
        return


    def startup(self):
        count = 1
        ret = self.session.startup(self.sock)
        while ret==self.LIBSSH2_ERROR_EAGAIN:
            self._wait_select()
            count += 1
            ret = self.session.startup(self.sock)

        my_print("startup count is %s" %count)
        my_print("sess startup ret is %s" %ret)


    def auth(self):
        count = 1
        ret = self.session.userauth_password(self.username, self.password)
        while ret == self.LIBSSH2_ERROR_EAGAIN:
            self._wait_select()
            count += 1
            ret = self.session.userauth_password(self.username, self.password)
            
        my_print("userauth count is %s" %count)
        my_print("userauth pass ret is %s" %ret)

    def open_channel(self):
        self.chan = self.session.open_session()
        count = 1
        while self.chan == None:
            count += 1
            self._wait_select()
            self.chan = self.session.open_session()

        my_print("open channel count is %s" %count)

        count = 1
        ret = self.chan.pty()
        while ret == self.LIBSSH2_ERROR_EAGAIN:
            count += 1
            self._wait_select()
            ret = self.chan.pty()

        my_print("channel pty count is %s" %count)
        my_print("channel pty ret is %s" %ret)
        
    def execute(self, cmd):
        count = 1
        ret = self.chan.execute(cmd)
        while ret == self.LIBSSH2_ERROR_EAGAIN:
            count += 1
            self._wait_select()
            ret = self.chan.execute(cmd)

        my_print("execute count is %s" %count)
        my_print("execute ret is %s" %ret)

        while not self.chan.eof():
            self._wait_select()
            data1 = self.chan.read_ex()
            while data1[0] > 0:
                my_print('Received data')
                print data1[1]
                data1 = self.chan.read_ex()        


    def __del__(self):
        count = 1
        ret = self.session.close()
        while ret == self.LIBSSH2_ERROR_EAGAIN:
            count += 1
            self._wait_select()
            ret = self.session.close(cmd)

        my_print('session close count is %s' %count)


if __name__ == '__main__':
    try:
        if len(sys.argv) == 1:
            print usage
            sys.exit(1)
        src = SSHRemoteClientNonBlocking(sys.argv[1], sys.argv[2], sys.argv[3])
        src.startup()
        src.auth()
        src.open_channel()
        src.execute(sys.argv[4])
    except Exception, e:
        print str(e)
    except KeyboardInterrupt, e:
        sys.exit(1)
