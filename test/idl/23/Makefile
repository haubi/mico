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

IDLFILE1 = a
IDLFILE2 = b

CXXFLAGS := -I. -I../../../include $(CXXFLAGS) $(EHFLAGS)
LDLIBS    = -lmico$(VERSION) $(CONFLIBS)
LDFLAGS  := -L../../../orb $(LDFLAGS)

all .NOTPARALLEL: .depend demo

demo: $(IDLFILE1).h $(IDLFILE1).o main.o ../../../orb/$(LIBMICO) \
      $(IDLFILE2).h $(IDLFILE2).o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(IDLFILE1).o $(IDLFILE2).o \
           main.o $(LDLIBS) -o demo
	$(POSTLD) $@

$(IDLFILE1).h $(IDLFILE1).cc : $(IDLFILE1).idl $(IDLFILE2).h $(IDLGEN)
	$(IDL) $(IDLFILE1).idl

$(IDLFILE2).h $(IDLFILE2).cc : $(IDLFILE2).idl $(IDLGEN)
	$(IDL) $(IDLFILE2).idl

clean:
	rm -f $(IDLFILE1).cc $(IDLFILE1).h .depend *.o core demo *~ \
          $(IDLFILE2).cc $(IDLFILE2).h

ifeq (.depend, $(wildcard .depend))
include .depend
endif

.depend :
	echo '# Module dependencies' > .depend
	$(MKDEPEND) $(CXXFLAGS) *.cc >> .depend

