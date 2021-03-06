#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
#
# Copyright 2008 Sun Microsystems, Inc. All rights reserved.
#
# THE BSD LICENSE
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer. 
# Redistributions in binary form must reproduce the above copyright notice, 
# this list of conditions and the following disclaimer in the documentation 
# and/or other materials provided with the distribution. 
#
# Neither the name of the  nor the names of its contributors may be
# used to endorse or promote products derived from this software without 
# specific prior written permission. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# The SAFs library contains server application functions shared among
# the netsite servers.


BUILD_ROOT=../../..
USE_NSPR=1
USE_CLIENTLIBS=1
USE_LDAPSDK=1
USE_XERCESC=1
USE_ZLIB=1

MODULE=LibSAFs
include ${BUILD_ROOT}/make/base.mk

all::

EXPORT_LIBRARIES=libsafs
AR_TARGET=libsafs

LOCAL_INC=-I.. -I../public -I../../support
LOCAL_DEF+= -DBUILD_DLL
LOCAL_DEF+= -DUSING_NSAPI

include safsobjs.mk

ifeq ($(OS_ARCH),Linux)
LOCAL_DEF+= -DUSE_POSIXFDPASSING
endif

AR_OBJS=$(SAFSOBJS)

ifneq ($(OS_ARCH),WINNT)
MAKE_CGISTUB=$(MAKE) -f Makefile.cgistub MAKEFLAGS='$(MAKEFLAGS) $(EXTRA_MAKEFLAGS)' $@ || exit 255

link::
	$(MAKE_CGISTUB)

publish_copy::
	$(MAKE_CGISTUB)

endif #!WINNT

# this should always be last!
include ${BUILD_ROOT}/make/rules.mk
