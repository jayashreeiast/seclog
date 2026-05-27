#
#   Makefile to build the IPC Shared Memory Driver sample application
#
#   Copyright 2022-2024 NXP
#
#   NXP Confidential. This software is owned or controlled by NXP and may only
#   be used strictly
#   in accordance with the applicable license terms.  By expressly accepting
#   such terms or by downloading, installing, activating and/or otherwise using
#   the software, you are agreeing that you have read, and that you agree to
#   comply with and are bound by, such license terms.  If you do not agree to
#   be bound by the applicable license terms, then you may not retain,
#   install, activate or otherwise use the software.
#
#   This file contains sample code only. It is not part of the production code
#   deliverables.
#

include project_parameters.mk

ifneq (,$(findstring S32G2,$(EXAMPLE_DERIVATIVE)))
FAMILY     := S32G2XX
endif
ifneq (,$(findstring S32G3,$(EXAMPLE_DERIVATIVE)))
FAMILY     := S32G3XX
endif
ifneq (,$(findstring S32R45,$(EXAMPLE_DERIVATIVE)))
FAMILY     := S32R45
endif
DERIVATIVE := $(EXAMPLE_DERIVATIVE)
PLATFORM := S32G3XX

#IPCF example specific parameters
compiler ?= gcc
SHM_PLATFORM ?= s32g3xx
SHM_OS_TARGET ?= baremetal

ODIR ?= out
GDIR=generate

EXTRA_FLAGS ?=
CCOPT += $(EXTRA_FLAGS)

# Path to folders
SRC_DIRS      = src \
                generate/src

INCLUDE_DIRS  = include \
                generate/include

INCLUDE_DIRS+=  $(foreach mod,$(MCAL_MODULE_LIST),$(PLUGINS_DIR)/$(mod)_$(AR_PKG_NAME)/include) \
                $(foreach mod,$(MCAL_MODULE_LIST_ADDON),$(PLUGINS_DIR_ADDON)/$(mod)_$(AR_PKG_NAME_ADDON)/include) \
                $(PLUGINS_DIR)/Platform_$(AR_PKG_NAME)/startup/include \
                $(PLUGINS_DIR)/BaseNXP_$(AR_PKG_NAME)/header

INCLUDE_DIRS+=  $(ADDITIONAL_INCLUDE_DIRS)

SRC_DIRS    +=  $(foreach mod,$(MCAL_MODULE_LIST),$(PLUGINS_DIR)/$(mod)_$(AR_PKG_NAME)/src) \
                $(foreach mod,$(MCAL_MODULE_LIST_ADDON),$(PLUGINS_DIR_ADDON)/$(mod)_$(AR_PKG_NAME_ADDON)/src) \
                $(PLUGINS_DIR)/Platform_$(AR_PKG_NAME)/startup/src/m7 \
				$(PLUGINS_DIR)/Platform_$(AR_PKG_NAME)/startup/src/m7/$(compiler)

MAPFILE = $(ODIR)/$(ELFNAME).map

# compiler must be: gcc, daib, ghs or iar
include $(compiler).mk

# IPCF specific parameters
include $(SHM_DRIVER_PATH)/src/ipc-shm-rtos.mk
SRC_DIRS += $(SHM_DRIVER_SRC_DIR)
INCLUDE_DIRS += $(SHM_DRIVER_INCLUDES_DIRS)

# symbols for multicore usage
# CCOPT += -DMULTIPLE_IMAGE -DCORE0 \
# 			-DSTART_SECONDARY_CORES \
# 			-DTURN_ON_CPU1 \
# 			-DSTART_CM7_1 \
# 			-DSTART_CM7_2
# ASOPT += -DMULTIPLE_IMAGE -DCORE0

#if compiling/linking into ram
CCOPT += -DSINTRAM
ASOPT += -DSINTRAM
CCOPT += -DDISABLE_MCAL_INTERMODULE_ASR_CHECK

CCOPT   :=  $(CCOPT) \
            -DD_CACHE_ENABLE_NOT \
            -DI_CACHE_ENABLE \
            -DENABLE_FPU \
            -DCPU_CORTEX_M7 \
            -DMPU_ENABLE_NOT

# -MT $@ Set the name of the target in the generated dependency file.
# -MMD Generate dependency information as a side-effect of compilation, not instead of compilation.
# -MP Adds a target for each prerequisite in the list, to avoid errors when deleting files.
# -MF Write the generated dependency file to out folder as .d extension
# DEP_FLAG =-MT"$(@)" -MMD -MP -MF"$(ODIR)/$(*).d"

# $(addsuffix suffix,names…)  : $(addsuffix .c,foo bar) ->  produces the result ‘foo.c bar.c’
#                             : $(addprefix src/,foo bar) -> produces the result ‘src/foo src/bar’
# Output: -Iinclude -I../include
CFLAGS=$(addprefix -I, $(INCLUDE_DIRS))
# Output: List all path .h files (include/Fls_TestSetup.h include/Platform_Types.h ....)
INCLUDE_FILES := $(foreach DIR, $(INCLUDE_DIRS),$(wildcard $(DIR)/*.h))
# Output: List all .c and .s files (Fls_TestSetup.c exceptions.c Startup.s Vector_core.s)
SOURCE_FILES := $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.c))) $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.s))) $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.S)))
# Output: Source files that need to be built
SOURCE_FILES    +=  nvic.c
SOURCE_FILES    +=  sys_init.c
SOURCE_FILES    +=  system.c
# Output: The path that contains 3 above source files
SRC_DIRS    +=  $(PLUGINS_DIR)/Platform_$(AR_PKG_NAME)/startup/src

# Object files
OUT_FILES := $(SOURCE_FILES:.c=.o)
OUT_FILES += $(SOURCE_FILES:.s=.o)
OUT_FILES += $(SOURCE_FILES:.S=.o)

winpath = $(foreach WINPATH_ITEM,$(1),$(patsubst /cygdrive/$(word 2,$(subst /, , $(WINPATH_ITEM)))%,$(word 2,$(subst /, ,$(WINPATH_ITEM))):%,$(WINPATH_ITEM)))
ifeq ($(realpath /),)
my_realpath = $1
my_abspath = $1
else
my_realpath = $(realpath $1)
my_abspath = $(abspath $1)
endif

## Testing to check whether it is a Windows environment or an Unix like
## one. This test is based on 'uname' command call. If the shell is a Windows one
## with no GNU binutils or similar tools, the command will raise a CreateProcess
## error but the makefile will detect the correct environment and work as expected.

SEPARATOR=======================================================================
UNAME_CMD:=uname
OS_DETECTED:=$(shell $(UNAME_CMD))
mkdir_message:=Creating directory for object files
clean_message:=Removing files and directories from the compilation output
clean_all_message:=Removing files and directories from the compilation output and for the generation output

ifeq ($(OS_DETECTED), )
mkdir=mkdir_windows
clean_target=clean_windows
clean_all_target=clean_all_windows
env_predetection_msg=Checked for '$(UNAME_CMD)', not found
env_detected_msg=Assuming Windows environment
else
mkdir=mkdir_linux
clean_target=clean_linux
clean_all_target=clean_all_linux
env_predetection_msg=Checked for '$(UNAME_CMD)', found: $(OS_DETECTED)
env_detected_msg=Assuming Unix like environment
endif

.PHONY: mkdir_windows
mkdir_windows:
	@echo $(SEPARATOR)
	@echo $(mkdir_message)
	@mkdir $(ODIR) || @echo Directory already exists

.PHONY: mkdir_linux
mkdir_linux:
	@echo $(SEPARATOR)
	@echo $(mkdir_message)
	@mkdir -p $(ODIR)

.PHONY: clean_windows
clean_windows:
	@echo $(SEPARATOR)
	@echo $(clean_message)
	@rmdir /s /q $(ODIR) || @echo Directory already cleaned

.PHONY: clean_linux
clean_linux:
	@echo $(SEPARATOR)
	@echo $(clean_message)
	@rm -fr $(ODIR)

.PHONY: clean_all_windows
clean_all_windows:
	@echo $(SEPARATOR)
	@echo $(clean_all_message)
	@rmdir /s /q $(ODIR) || @echo Directory already cleaned
	@rmdir /s /q $(GDIR) || @echo Directory already cleaned

.PHONY: clean_all_linux
clean_all_linux:
	@echo $(SEPARATOR)
	@echo $(clean_all_message)
	@rm -fr $(ODIR)
	@rm -fr $(GDIR)

.PHONY: generate
generate:
	$(TRESOS_DIR)/bin/tresos_cmd.bat  deleteProject $(TRESOS_PROJECT_NAME) || exit 0 2>&1
	$(TRESOS_DIR)/bin/tresos_cmd.bat  importProject $(call winpath, $(call my_abspath, ./TresosProject/$(TRESOS_PROJECT_NAME))) 2>&1
ifeq ($(VARIANT_NO),0)
	$(TRESOS_DIR)/bin/tresos_cmd.bat generate $(TRESOS_PROJECT_NAME) 2>&1
else
	$(TRESOS_DIR)/bin/tresos_cmd.bat  autoconfigure $(TRESOS_PROJECT_NAME) GenerateAllVariants 2>&1
endif

.PHONY: build
build: main.elf

main.elf : $(OUT_FILES)
#add path for .o and .c filer to easy to automatically look up in sub folder
vpath %.c $(SRC_DIRS)
vpath %.o $(ODIR)
#add the rule to check if ODIR has been already created before start but we do want to rebuild .o so ODIR is added as order-only-prerequisites
%.o: %.c | $(ODIR)
	@echo "Compiling $<"
    # The -c flag says to generate the object file, the -o $@ says to put the output of the compilation in the
    # file named on the left side of the :, the $< is the first item in the dependencies list and the CFLAGS macro is defined as above.
	@$(CC) $(CCOPT) -c -o $(ODIR)/$@ $< $(CFLAGS) $(DEP_FLAG)
$(ODIR): $(mkdir)
vpath %.s $(addsuffix :, $(SRC_DIRS))
vpath %.S $(addsuffix :, $(SRC_DIRS))
%.o : %.s
	@echo "Compiling $<"
	@$(AS) $(ASOPT) $< -o $(ODIR)/$@ $(DEP_FLAG)
%.o : %.S
	@echo "Compiling $<"
	@$(AS) $(ASOPT) $< -o $(ODIR)/$@ $(DEP_FLAG)
# Link all the object files to become one elf file
%.elf: %.o
	@echo "Linking $@"
	@$(LD) $(LDOPT) $(ODIR)/*.o -o $(ODIR)/$@
	@echo "Translating $< into $@"
	$(OT) $(ODIR)/$@ $(OTOPT) $(ODIR)/main.bin
	@echo "Renaming elf to $(ELFNAME).elf"
	@mv $(ODIR)/main.elf $(ODIR)/$(ELFNAME).elf
	@mv $(ODIR)/main.bin $(ODIR)/$(ELFNAME).bin

#include new .d files which contain new rule created from gcc auto-depend generation
-include $(wildcard $(ODIR)/*.d)
.PHONY: clean_all
clean_all: $(clean_all_target)

.PHONY: clean
clean: $(clean_target)

.PHONY: run
run:
	@echo "For running from command line sed is needed for replacing strings in config.t32 and run.cmm, otherwise the run.cmm ca be ran directly from T32"
	@sed -i 's/replace_t32_dir/$(subst /,\/,$(T32_DIR))/g' debug/config.t32
	@if [ ! -f $(T32_DIR)/target_device.t32 ]; then sed -i 's/\$$INCLUDE target_device.t32/ /g' debug/config.t32; fi;
	@sed -i 's/DO device.cmm/DO debug\/device.cmm/g' debug/run.cmm
	@sed -i 's/\.\.\/out/.\/out/g' debug/run.cmm
	$(T32_DIR)/bin/windows64/t32marm.exe -c debug/config.t32 -s debug/run.cmm
	@sed -i 's/debug\/device.cmm/device.cmm/g' debug/run.cmm
	@sed -i 's/\.\/out/..\/out/g' debug/run.cmm
