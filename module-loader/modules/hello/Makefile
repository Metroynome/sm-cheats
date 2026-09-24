.DEFAULT_GOAL := all
include ../../../Makefile.pref

MODULE_ADDRESS ?= 0x000f4000
MODULE_REGION ?= 0
OUTPUT = ../../hostfs/sm/modules/hello.bin

.PHONY: all clean FORCE
all: $(OUTPUT)

obj/main.o: main.c ../../module.h
	@mkdir -p obj
	$(EE_CC) -std=gnu99 -Os -G0 -Wall -W -ffreestanding -fno-builtin -fno-pic -mno-abicalls -ffunction-sections -fdata-sections -c $< -o $@

obj/hello.elf: obj/main.o ../../module.ld FORCE
	$(EE_CC) -nostdlib -nostartfiles -Wl,--defsym,MODULE_ADDRESS=$(MODULE_ADDRESS),--defsym,MODULE_REGION=$(MODULE_REGION),-T,../../module.ld,-Map,obj/hello.map -o $@ $< -lgcc

$(OUTPUT): obj/hello.elf
	@mkdir -p $(@D)
	$(EE_OBJCOPY) -O binary $< $@

clean:
	rm -rf obj
	rm -f $(OUTPUT)

FORCE:
