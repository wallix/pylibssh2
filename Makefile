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
TARGET=libssh2
PY_TARGET=py${TARGET}
DEBIAN_TARGET=python-libssh2

build: clean
	python setup.py clean
	python setup.py build --force

install: build
	sudo python setup.py install

installdeb:
	sudo dpkg -i ../${DEBIAN_TARGET}*.deb

dist:
	python setup.py sdist --format=gztar

deb:
	dpkg-buildpackage -tc

doc:
	epydoc --no-private -n ${PY_TARGET} -o doc ${TARGET}

clean:
	rm -rf build dist
	rm -rf MANIFEST *.egg-info
	rm -rf ${PY_TARGET}/*pyc

cleandoc:
	rm -rf doc

cleandeb:
	rm -rf ../*.deb ../*.dsc ../*.tar.gz ../*.changes

distclean: clean cleandoc cleandeb
