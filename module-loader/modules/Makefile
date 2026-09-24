.DEFAULT_GOAL := all
MODULES := $(patsubst %/Makefile,%,$(wildcard */Makefile))
.PHONY: all clean release $(MODULES)
all clean release:
	@set -e; for module in $(MODULES); do $(MAKE) -C $$module $@; done

$(MODULES):
	$(MAKE) -C $@
