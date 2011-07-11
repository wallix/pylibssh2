#
# Copyright (c) 2011 WALLIX, SAS. All rights reserved.
# Licensed computer software. Property of WALLIX.
# Product Name: pylibssh2
# Author(s) : Sofian Brabez <sbz@wallix.com>
# Id: $Id: test_session.py 145 2011-01-11 11:12:38Z sbrabez $
# URL: $URL: svn+ssh://ohervieu@zoo.ifr.lan/srv/svn/pylibssh2/trunk/tests/test_session.py $
# Module description:
# Compile with:
#
"""
Unit tests for Session
"""

import socket
import unittest

class SessionTest(unittest.TestCase):
    def setUp(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("127.0.0.1", 22))

    def test_import(self):
        ok = False
        try:
            import libssh2 # pyflakes:ignore
            ok=True
        except ImportError:
            ok=False

        self.assertTrue(ok)

    def test_session_create(self):
        import libssh2
        session = libssh2.Session()
        self.assertTrue(isinstance(session, libssh2.session.Session))

    def test_session_startup(self):
        import libssh2
        session = libssh2.Session()
        session.startup(self.socket)
        self.assertEqual(session.userauth_authenticated(), 0)

    def tearDown(self):
        self.socket.close()

if __name__ == '__main__':
    unittest.main()
