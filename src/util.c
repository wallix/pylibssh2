/*-
 * pylibssh2 - python bindings for libssh2 library
 *
 * Copyright (C) 2005 Keyphrene.com.
 * Copyright (C) 2010 Wallix Inc.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "util.h"

/* {{{ get_flags
 */
unsigned long 
get_flags(char *mode) {
    int i=0;
    unsigned long f=0;

    struct {
        char mode;
        unsigned long flags;
    } modeflags[5] = {
        {'a', LIBSSH2_FXF_APPEND },
        {'w', LIBSSH2_FXF_WRITE | LIBSSH2_FXF_TRUNC | LIBSSH2_FXF_CREAT },
        {'r', LIBSSH2_FXF_READ },
        {'+', LIBSSH2_FXF_READ | LIBSSH2_FXF_WRITE },
        {'x', LIBSSH2_FXF_WRITE | LIBSSH2_FXF_TRUNC | LIBSSH2_FXF_EXCL | LIBSSH2_FXF_CREAT }
    };

    for(i=0; i<5; i++) {
        if(strchr(mode, modeflags[i].mode)) {
            f |= modeflags[i].flags;
        }
    }

    return f;
}
/* }}} */
