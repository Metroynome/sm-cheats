.DEFAULT_GOAL := all
MODULE_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
LIBRAC5 := $(abspath $(MODULE_ROOT)/../../librac5)
include $(MODULE_ROOT)/../Makefile.pref

REGION ?= ntscu
MODULE_NAME ?= $(notdir $(CURDIR))
MODULE_PORTABLE ?= 0
SUPPORTED_REGIONS ?= ntscu pal ntscj
REGION_ID_ntscu = 1
REGION_ID_pal = 2
REGION_ID_ntscj = 3
REGION_DEFS_ntscu = -DNTSCU -DSM_NTSCU -DRAC5_NTSCU
REGION_DEFS_pal = -DPAL -DSM_PAL -DRAC5_PAL
REGION_DEFS_ntscj = -DNTSCJ -DSM_NTSCJ -DRAC5_NTSCJ
ifeq ($(filter $(REGION),ntscu pal ntscj),)
$(error REGION must be ntscu, pal or ntscj)
endif
include $(MODULE_ROOT)/modules/layout.mk
MODULE_ADDRESS ?= $(MODULE_LOW_START)
MODULE_REGION = $(if $(filter 1,$(MODULE_PORTABLE)),0,$(REGION_ID_$(REGION)))
SOURCES ?= $(wildcard *.c *.S)
OBJ = obj/$(REGION)
OBJECTS = $(addprefix $(OBJ)/,$(addsuffix .o,$(basename $(SOURCES))))
ELF = bin/$(MODULE_NAME).$(REGION).elf
BIN = $(ELF:.elf=.bin)
OUTPUT ?= $(MODULE_ROOT)/hostfs/sm/modules/$(MODULE_NAME).bin
CFLAGS = -std=gnu99 -Os -G0 -Wall -W -ffreestanding -fno-builtin -fno-pic -mno-abicalls -ffunction-sections -fdata-sections -D_EE $(EE_DEFS) $(REGION_DEFS_$(REGION)) $(MODULE_CFLAGS)
INCLUDES = -I$(MODULE_ROOT) -I$(OBJ)/include -I$(LIBRAC5)/include -I$(PS2SDK)/common/include -I$(PS2SDK)/ee/include
LIBRARY = $(if $(filter 1,$(MODULE_PORTABLE)),,$(LIBRAC5)/lib/librac5$(REGION).a)

.PHONY: all clean release library headers supported FORCE
all: $(BIN)
	@mkdir -p $(dir $(OUTPUT))
	cp $(BIN) $(OUTPUT)

supported:
	@test -n "$(filter $(REGION),$(SUPPORTED_REGIONS))" || { echo "$(MODULE_NAME) supports: $(SUPPORTED_REGIONS)"; exit 1; }

headers:
	@mkdir -p $(OBJ)/include/librac5
	@cp $(LIBRAC5)/include/*.h $(OBJ)/include/librac5/

library:
ifneq ($(MODULE_PORTABLE),1)
	$(MAKE) -C $(LIBRAC5) -f Makefile.$(REGION)
endif

$(OBJ)/%.o: %.c $(MODULE_ROOT)/module.h FORCE | headers supported
	@mkdir -p $(@D)
	$(EE_CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ)/%.o: %.S FORCE | headers supported
	@mkdir -p $(@D)
	$(EE_CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(ELF): $(OBJECTS) $(MODULE_ROOT)/module.ld FORCE | library
	@mkdir -p $(@D)
	$(EE_CC) -nostdlib -nostartfiles -Wl,--defsym,MODULE_ADDRESS=$(MODULE_ADDRESS),--defsym,MODULE_REGION=$(MODULE_REGION),-T,$(MODULE_ROOT)/module.ld,-Map,$(@:.elf=.map) -o $@ $(OBJECTS) $(LIBRARY) -lgcc

$(BIN): $(ELF)
	@start=$$(( $(MODULE_ADDRESS) )); end=$$($(EE_PREFIX)nm $(ELF) | awk '$$3 == "_memory_end" {print $$1}'); test -n "$$end"; end=$$((0x$$end)); test $$((start & 15)) -eq 0 && { { test $$start -ge $$(( $(MODULE_LOW_START) )) && test $$end -le $$(( $(MODULE_LOW_END) )); } || { test $$start -ge $$(( $(MODULE_HEAP_START) )) && test $$end -le $$(( $(MODULE_HEAP_END) )); }; } || { echo "Module exceeds loader arenas"; exit 1; }
	$(EE_OBJCOPY) -O binary $< $@

release: all
	@mkdir -p $(MODULE_ROOT)/release/hostfs/sm/modules
	cp $(BIN) $(MODULE_ROOT)/release/hostfs/sm/modules/$(MODULE_NAME).bin

clean:
	rm -rf obj bin
	rm -f $(OUTPUT)

FORCE:
-include $(OBJECTS:.o=.d)
