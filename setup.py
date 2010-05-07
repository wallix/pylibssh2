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
