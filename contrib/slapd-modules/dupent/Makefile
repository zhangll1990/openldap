# $OpenLDAP$
# This work is part of OpenLDAP Software <http://www.openldap.org/>.
#
# Copyright 1998-2010 The OpenLDAP Foundation.
# Copyright 2004 Howard Chu, Symas Corp. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted only as authorized by the OpenLDAP
# Public License.
#
# A copy of this license is available in the file LICENSE in the
# top-level directory of the distribution or, alternatively, at
# <http://www.OpenLDAP.org/license.html>.

LIBTOOL=../../../libtool
OPT=-DSLAPD_OVER_DUPENT=2 -g -O2
#LIBTOOL=../../../../ldap-devel/libtool
#OPT=-DSLAPD_OVER_DUPENT=2 -g -O0
CC=gcc

LDAP_INC=-I../../../include -I../../../servers/slapd
#LDAP_INC=-I../../../include -I../../../servers/slapd -I../../../../ldap-devel/include
INCS=$(LDAP_INC)

LDAP_LIB=-lldap_r -llber
LIBS=$(LDAP_LIB)

prefix=/usr/local
exec_prefix=$(prefix)
ldap_subdir=/openldap

libdir=$(exec_prefix)/lib
libexecdir=$(exec_prefix)/libexec
moduledir = $(libexecdir)$(ldap_subdir)

all:	dupent.la


dupent.lo:	dupent.c
	$(LIBTOOL) --mode=compile $(CC) $(OPT) $(DEFS) $(INCS) -c $?

dupent.la:	dupent.lo
	$(LIBTOOL) --mode=link $(CC) $(OPT) -version-info 0:0:0 \
	-rpath $(moduledir) -module -o $@ $? $(LIBS)

clean:
	rm -f dupent.lo dupent.la

install: dupent.la
	mkdir -p $(DESTDIR)$(moduledir)
	$(LIBTOOL) --mode=install cp dupent.la $(DESTDIR)$(moduledir)

