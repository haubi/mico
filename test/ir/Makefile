#
# MICO --- a free CORBA implementation
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

include ../../MakeVars

CXXFLAGS := -I. -I../../include $(CXXFLAGS)
LDLIBS    = -lmicoir$(VERSION) -lmico$(VERSION) $(CONFLIBS)
LDFLAGS  := -L../../orb -L../../ir $(LDFLAGS)

all .NOTPARALLEL: .depend irtest

irtest: irtest.o ../../orb/$(LIBMICO) ../../ir/$(LIBMICOIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) irtest.o $(LDLIBS) -o irtest
	$(POSTLD) $@

clean:
	rm -f irtest *.o core *~ .depend *.ior


check:
	@echo "Testing ./ir..."
	@if ./irtest|cmp expected-output - >/dev/null; then : ; \
	else echo "FAILED:"; echo "===============================" \
	./irtest|diff -u expected-output - ; \
	echo "==============================="; fi
	@./feed-test.sh


ifeq (.depend, $(wildcard .depend))
include .depend
endif

.depend:
	echo "# module dependencies" > .depend
	$(MKDEPEND) $(CXXFLAGS) *.cc >> .depend
