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
    Installation script for the libssh2 module
"""
from distutils.core import setup, Extension
import os, sys, glob

sys.path.append('libssh2')
import version as info

version = info.__version__
url = info.__url__
author = info.__author__
author_email = info.__authoremail__

long_description = '''Python binding for libssh2 library'''

classifiers = """Development Status :: 4 - Beta
License :: OSI Approved :: BSD License
Operating System :: POSIX
Programming Language :: C
Programming Language :: Python
Topic :: Security
Topic :: Software Development :: Libraries""".split('\n')

libssh2_src = glob.glob('src/*.c')
libssh2_dep = glob.glob('src/*.h')
libssh2_incdir = None
if 'bsd' in sys.platform[-1] or 'bsd' in os.uname()[0].lower():
    libssh2_incdir = ['/usr/local/include/']
    libssh2_libdir = ['/usr/local/lib/']
libssh2_lib = ['ssh2']
libssh2_libdir = None
libssh2_compile_args = ['-ggdb']

module = Extension('_libssh2',
                    define_macros = [
                        ('MAJOR_VERSION', version[0]),
                        ('MINOR_VERSION', version[2]),
                        ('PATCH_VERSION', version[4])
                    ],
                    sources = libssh2_src,
                    depends = libssh2_dep,
                    include_dirs = libssh2_incdir,
                    library_dirs = libssh2_libdir,
                    libraries = libssh2_lib,
                    extra_compile_args = libssh2_compile_args)

setup(name='pylibssh2',
      version=version,
      packages    = ['libssh2'],
      package_dir = { 
        'libssh2' : 'libssh2'
      },
      description = long_description,
      author=author,
      author_email=author_email,
      url=url,
      download_url='%s/download/pylibssh2-%s.tar.gz' % (url, version),
      ext_modules = [module],
      license = ['LGPL', 'BSD'],
      platforms = ['Linux', 'BSD'],
      long_description = long_description,
      classifiers=classifiers)
