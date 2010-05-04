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
"""
Abstraction for libssh2 L{Channel} object
"""

class ChannelException(Exception):
    """
    Exception raised when L{Channel} actions fails.
    """
    pass

class Channel(object):
    """
    Channel object
    """
    def __init__(self, _channel):
        """
        Creates a new channel object with the given _channel.

        @param _channel: low level channel object
        @type _channel: L{_libssh2.Channel}
        """
        self._channel = _channel
        self.closed = False
        self.flushed = False

    def close(self):
        """
        Closes the active channel.

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self.closed = True
        return self._channel.close()

    def eof(self):
        """
        Checks if the remote host has sent a EOF status.

        @return: 1 if the remote host has sent EOF otherwise 0
        @rtype: int
        """
        return self._channel.eof()

    def execute(self, command):
        """
        Executes command on the channel.

        @param command: message data
        @type command: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self.closed = True
        return self._channel.execute(command)

    def exit_status(self):
        """
        Gets the exit code raised by the process running on the remote host.

        @return: the exit status reported by remote host or 0 on failure
        @rtype: int
        """
        return self._channel.exit_status()

    def flush(self):
        """
        Flushs the read buffer on the channel.

        @return: 0 on sucess or negative on failure
        @rtype: int
        """
        self.flushed = True
        return self._channel.flush()

    def poll(self, timeout, nfds):
        """
        Polls for activity on the channel.

        @param timeout: remaining timeout
        @type timeout: int
        @param nfds: number of fds to poll
        @type nfds: int

        @return: number of fds with interesting events or negative on failure
        @rtype: int
        """
        return self._channel.poll(timeout, nfds)

    def poll_read(self, extended):
        """
        Checks if data is available on the channel.

        @param extended: if message channel datas is extended
        @type extended: int
        
        @return: 1 when data is available or 0 otherwise
        @rtype: int
        """
        return self._channel.poll_read(extended)

    def pty(self, term="vt100"):
        """
        Requests a pty with term type on the channel.

        @param term: terminal emulation type (vt100, ansi, etc...)
        @type term: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        return self._channel.pty(term)

    def pty_resize(self, width, height):
        """
        Requests a pty resize of the channel with given width and height.

        @param width: terminal width
        @type width: int
        @param height: terminal height
        @type height: int

        @return: 0 on success or negative on failure
        @rtype: int
        """
        return self._channel.pty_resize(width, height)

    def read(self, size=4096):
        """
        Reads size bytes on the channel.

        @param size: size of the buffer storage
        @type size: int
        
        @return: bytes readed or negative on failure
        @rtype: str 
        """
        return self._channel.read(size)

    def send_eof(self):
        """
        Sends EOF status on the channel to remote server.

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._channel.send_eof()

    def setblocking(self, mode=1):
        """
        Sets blocking mode on the channel. Default mode is blocking.

        @param mode: blocking (1) or non blocking (0) mode
        @rtype: int
        """
        return self._channel.setblocking(mode)

    def setenv(self, name, value):
        """
        Sets envrionment variable on the channel.

        @param name: envrionment variable name
        @type name: str
        @param value: envrionment variable value
        @type value: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        return self._channel.setenv(name, value)

    def shell(self):
        """
        Requests a shell on the channel.

        @return: 0 on success or negative on failure
        @rtype: int
        """
        return self._channel.shell()

    def window_read(self, read_avail, window_size_initial):
        """
        Checks the status of the read window on the channel.

        @param read_avail: window limit to read
        @type read_avail: int
        @param window_size_initial: window initial size defined
        @type window_size_initial: int

        @return: the number of bytes which the remote end may send 
        without overflowing the window limit
        @rtype: int
        """
        return self._channel.window_read(read_avail, window_size_initial)

    def write(self, message):
        """
        Writes data on the channel.

        @param message: data to write
        @type message: str

        @return: 0 on sucess or failure
        @rtype: int
        """
        return self._channel.write(message)

    def x11_req(self, display):
        """
        Requests a X11 Forwarding on the channel.

        @param display: screen number
        @param display: int

        @return: 0 on success or negative on failure
        @rtype: int
        """
        return self._channel.x11_req(display)
