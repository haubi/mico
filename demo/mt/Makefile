
include ../MakeVars

DIRS=callback prodcons-1

.PHONY: all $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) --directory $@

clean:
	for i in $(DIRS); do $(MAKE) -C $$i clean || exit 1; done
	rm -f *.o core *~ .depend

install:
	$(IDIRCMD) $(INSTDIR)/doc/mico/examples/mt
	$(ISRCCMD) Makefile $(INSTDIR)/doc/mico/examples/mt
	$(ISRCCMD) README $(INSTDIR)/doc/mico/examples/mt
	for i in $(DIRS); do $(MAKE) -C $$i install || exit 1; done
