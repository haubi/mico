#
# MICO --- a CORBA 2.0 implementation
# Copyright (C) 1997 Kay Roemer & Arno Puder
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Send comments and/or bug reports to:
#                mico@informatik.uni-frankfurt.de
#

include ../../../MakeVars

CXXFLAGS := -I. -I../../../include $(CXXFLAGS) $(EHFLAGS)
LDLIBS    = -lmico$(VERSION) $(CONFLIBS)
LDFLAGS  := -L../../../orb $(LDFLAGS)

all .NOTPARALLEL: .depend client server

client: wstr.h wstr.o client.o ../../../orb/$(LIBMICO)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) wstr.o client.o $(LDLIBS) -o client
	$(POSTLD) $@

server: wstr.h wstr.o server.o ../../../orb/$(LIBMICO)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) wstr.o server.o $(LDLIBS) -o server
	$(POSTLD) $@

wstr.h wstr.cc : wstr.idl $(IDLGEN)
	$(IDL) wstr.idl

clean:
	rm -f wstr.cc wstr.h .depend *.o core client server test.ref *~

ifeq (.depend, $(wildcard .depend))
include .depend
endif

.depend :
	echo '# Module dependencies' > .depend
	$(MKDEPEND) $(CXXFLAGS) *.cc >> .depend

