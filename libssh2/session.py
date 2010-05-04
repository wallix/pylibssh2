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
Abstraction for libssh2 L{Session} object
"""

import _libssh2

from channel import Channel

class SessionException(Exception):
    """
    Exception raised when L{Session} actions fails.
    """
    pass

class Session(object):
    """
    Session object
    """
    def __init__(self):
        """
        Create a new session object.
        """
        self._session = _libssh2.Session()

    def callback_set(self, callback_type, callback):
        """
        Sets callback on the session.

        @param callback_type: value of libssh2.LIBSSH2_CALLBACK_* constant
        @type callback_set: int
        @param callback: a callback Python object
        @type callback: function
        """
        self._session.callback_set(callback_type, callback)

    def close(self, reason="Disconnect"):
        """
        Closes the session.

        @param reason: human readable reason for disconnection
        @type reason: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._session.close(reason)

    def direct_tcpip(self, host, port, shost, sport):
        """
        Tunnels a TCP connection through the session.

        @param host: remote host
        @type host: str
        @param port: remote port
        @type port: int
        @param shost: local host
        @type shost: str
        @param sport: local port
        @type sport: int

        @return: new opened L{Channel}
        @rtype: L{Channel}
        """
        return self._session.direct_tcpip(host, port, shost, sport)

    def forward_listen(self, host, port, bound_port, queue_maxsize):
        """
        Forwards a TCP connection through the session.

        @param host: remote host
        @type host: str
        @param port: remote port
        @type port: int
        @param bound_port: populated with the actual port on the remote host
        @type bound_port: int
        @param queue_maxsize: maxium number of pending connections
        @type int

        @return: new L{Listener} instance on success or None on failure
        @rtype: L{Listener}
        """
        return self._session.forward_listen(
            host, port, bound_port, queue_maxsize
        )

    def hostkey_hash(self, hashtype):
        """
        Returns the computed digest of the remote host's key.

        @param hashtype: values possible are 1 (HASH_MD5) or 2 (HASH_SHA1)
        @type hashtype: int

        @return: string representation of the computed hash value
        @rtype: str
        """
        return self._session.hostkey_hash(hashtype)

    def last_error(self):
        """
        Returns the last error in tuple format (code, message).

        @return: error tuple (int, str)
        @rtype: tuple
        """
        return self._session.last_error()

    def open_session(self):
        """
        Allocates a new L{Channel} for the session.

        @return: new channel opened
        @rtype: L{Channel}
        """
        return Channel(self._session.open_session())


    def set_trace(self, bitmask):
        """
        Sets trace level on the session.

        @param bitmask: bitmask on libssh2.LIBSSH2_TRACE_* constant
        """
        self._session.set_trace(bitmask)


    def scp_recv(self, remote_path):
        """
        Gets a remote file via SCP Protocol.

        @param remote_path: absolute path of remote file to transfer
        @type remote_path: str

        @return: new channel opened
        @rtype: L{Channel}
        """
        return Channel(self._session.scp_recv(remote_path))

    def scp_send(self, path, mode, size):
        """
        Sends a file to remote host via SCP protocol.

        @param path: absolute path of file to transfer
        @type path: str
        @param mode: file access mode to create file
        @type mode: int
        @param size: size of file being transmitted
        @type size: int

        @return: new channel opened
        @rtype: L{Channel}
        """
        return Channel(self._session.scp_send(path, mode, size))

    def session_method_pref(self, method_type, pref):
        """
        Sets preferred methods to be negociated. Theses preferences must be
        prior to calling L{startup}.

        @param method_type: the method type constants
        @type method_type: int
        @param pref: coma delimited list of preferred methods
        @type pref: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._session.session_methods(method_type, pref)

    def session_methods(self):
        """
        Returns dictionnary with the current actives algorithms.
        CS keys is Client to Server and SC keys is Server to Client.

        @return: dictionnary with actual method negociated
        @rtype: dict
        """
        return self.session_methods()

    def set_banner(self, banner=_libssh2.DEFAULT_BANNER):
        """
        Sets the banner that will be sent to remote host. This is optional, the
        banner L{_libssh2.DEFAULT_BANNER} will be sent by default.

        @param banner: an user defined banner
        @type banner: str
        
        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._session.set_banner(banner)

    def sftp_init(self):
        """
        Open an Sftp Channel.

        @return: new Sftp channel opened
        @rtype: L{Sftp}
        """
        raise NotImplementedError()

    def startup(self, sock):
        """
        Starts up the session form a socket created by a socket.socket() call.

        @param sock: a connected socket object
        @type sock: socket._socketobject

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._session.startup(sock)

    def userauth_authenticated(self):
        """
        Returns authentification status for the given session.

        @return: non-zero if authenticated or 0 if not
        @rtype: int
        """
        return self._session.userauth_authenticated()

    def userauth_list(self, username):
        """
        Lists the authentification methods supported by a server.

        @param username: username which will be used while authenticating
        @type username: str

        @return: string containing a comma-separated list of authentication 
        methods
        @rtype: str
        """
        self._session.userauth_list(username)

    def userauth_password(self, username, password):
        """
        Authenticates a session with the given username and password.

        @param username: user to authenticate
        @type username: str
        @param password: password to use for the authentication
        @type password: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        self._session.userauth_password(username, password)

    def userauth_publickey_fromfile(
            self, username, publickey, privatekey, passphrase
        ):
        """
        Authenticates a session as username using a key pair found in the 
        pulickey and privatekey files, and passphrase if provided.

        @param username: user to authenticate
        @type username: str
        @param publickey: path and name of public key file
        @type publickey: str
        @param privatekey: path and name of private key file
        @type privatekey: str
        @param passphrase: passphrase to use when decoding private file
        @type passphrase: str

        @return: 0 on success or negative on failure
        @rtype: int
        """
        raise NotImplementedError()
