.PHONY: all release clean clean-release
all release clean clean-release:
	$(MAKE) -C printf $@
	$(MAKE) -C test $@
	$(MAKE) -C libc-demo $@
	$(MAKE) -C module-loader $@
