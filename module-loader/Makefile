.PHONY: all release clean clean-release ntscu pal ntscj

all release:
	$(MAKE) -f ../Makefile.ntscu $@ PACKAGE_MODULES=0
	$(MAKE) -f ../Makefile.pal $@ PACKAGE_MODULES=0
	$(MAKE) -f ../Makefile.ntscj $@ PACKAGE_MODULES=0
	$(MAKE) -f ../Makefile.ntscu module-files

clean clean-release:
	$(MAKE) -f ../Makefile.ntscu $@
	$(MAKE) -f ../Makefile.pal $@
	$(MAKE) -f ../Makefile.ntscj $@

ntscu pal ntscj:
	$(MAKE) -f ../Makefile.$@

.PHONY: modules modules-clean
modules:
	$(MAKE) -C modules

modules-clean:
	$(MAKE) -C modules clean
