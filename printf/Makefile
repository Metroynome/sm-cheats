.PHONY: all release clean clean-release ntscu pal ntscj

all release clean clean-release:
	$(MAKE) -f ../Makefile.ntscu $@
	$(MAKE) -f ../Makefile.pal $@
	$(MAKE) -f ../Makefile.ntscj $@

ntscu pal ntscj:
	$(MAKE) -f ../Makefile.$@
