#  This file contains sample code only. It is not part of the production code deliverables.
#
#  Copyright 2019-2024 NXP
#
#  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
#  be used strictly in accordance with the applicable license terms. By
#  expressly accepting such terms or by downloading, installing, activating
#  and/or otherwise using the software, you are agreeing that you have read,
#  and that you agree to comply with and are bound by, such license terms. If
#  you do not agree to be bound by the applicable license terms, then you may
#  not retain, install, activate or otherwise use the software.

################################################################################
# Options
################################################################################
DEBUG ?= TRUE
HW ?= RDB3
SWITCH_TASK_GPT_CHANNEL_CFG ?= 0
TM_STUB_GPT_CHANNEL_CFG ?= 1
CONFIG_MINIHIF ?= FALSE
CONFIG_MINIHIF_HW ?= HIF3
LOOPBACK_TEST ?= FALSE
USE_SERDES_DRIVER ?= FALSE
VERBOSITY_LEVEL ?= 8
FCI_L2BR_TEST ?= TRUE
FCI_RTABLE_TEST ?= FALSE
PING_TEST ?= TRUE
LWIP ?= FALSE
LWIP_STATUS_CALLBACK ?= 0
LOCAL_BUILD ?= FALSE
LOCAL_DIR ?= ..
SLAVE_DRIVER ?= FALSE
MISRA_SCAN ?= FALSE
CERT_C ?= FALSE
CALL_GRAPH ?=FALSE

include project_parameters.mk

ifeq ($(PING_TEST),TRUE)
    ifeq ($(LWIP),TRUE)
        $(error PING_TEST and LWIP can not be used both at the same time)
    endif
endif

ifeq ($(LWIP), TRUE)
    LWIP_MASK ?= 255.255.255.0
    LWIP_DEFAULT_GATEWAY_0 ?= 10.0.0.1
    LWIP_DEFAULT_GATEWAY_1 ?= 10.0.1.1
    LWIP_DEFAULT_GATEWAY_2 ?= 10.0.2.1
endif

ifeq ($(SLAVE_DRIVER), TRUE)
    IP_ADDRESS_CTRL_0 ?= 10.0.0.21
    IP_ADDRESS_CTRL_1 ?= 10.0.1.21
    IP_ADDRESS_CTRL_2 ?= 10.0.2.21
else
    IP_ADDRESS_CTRL_0 ?= 10.0.0.2
    IP_ADDRESS_CTRL_1 ?= 10.0.1.2
    IP_ADDRESS_CTRL_2 ?= 10.0.2.2
    IPV6_ADDRESS_CTRL_0 ?= fe80:0000:0000:0000:0000:0000:0000:0001
    IPV6_ADDRESS_CTRL_1 ?= fe80:0000:0000:0000:0000:0000:0000:0002
    IPV6_ADDRESS_CTRL_2 ?= fe80:0000:0000:0000:0000:0000:0000:0003
endif
RTABLE_SRC_IP_ADDRESS ?= 11.41.48.100
RTABLE_DES_IP_ADDRESS ?= 12.41.48.100
RTABLE_SRC_PORT ?= 11
RTABLE_DES_PORT ?= 12

TEXT_STATS ?= TRUE
# The CONFIGURE_SOC should be equal to FALSE when SLAVE_DRIVER=TRUE
ifeq ($(SLAVE_DRIVER), TRUE)
    CONFIGURE_SOC ?= FALSE
else
    CONFIGURE_SOC ?= TRUE
endif

# If the USER_MODE=TRUE, user needs to turn on each module's user_mode_support=true and re-generate the sources
USER_MODE ?= FALSE

COMPILER ?= GCC

# The path to the PFE configuration dir and output dir
ifeq ($(MISRA_SCAN),TRUE)
    CERT_C = TRUE
    ifeq ($(SLAVE_DRIVER),FALSE)
        LINKER_DIR := linker/linker_master_app
        ODIR := output_misra
        GDIR := generate_misra
        PROJ_CONFIG_NAME := MisraScanMaster
        APP_NAME := PFE_MASTER
    else
        LINKER_DIR := linker/linker_slave_app
        ODIR := output_slave_misra
        GDIR := generate_slave_misra
        PROJ_CONFIG_NAME := MisraScanSlave
        APP_NAME := slave_app
    endif
else
    ifeq ($(SLAVE_DRIVER),FALSE)
        LINKER_DIR := linker/linker_master_app
        ODIR := output
        GDIR := generate
        PROJ_CONFIG_NAME := MasterProject
        APP_NAME := PFE_MASTER
    else
        LINKER_DIR := linker/linker_slave_app
        ODIR := output_slave
        GDIR := generate_slave
        PROJ_CONFIG_NAME := SlaveProject
        APP_NAME := slave_app
    endif
endif

# Target platform definitions
HW_PLATFORM_G2 = FALSE
HW_PLATFORM_G3 = FALSE
TARGET_BOARD = $(HW)

################################################################################
# This is only due to backward compatibility with Jenkins build pipelines and will be removed in the future!
ifeq ($(HW),S32G2)
    $(warning Obsoleted HW=$(HW) platform name. Please use HW=EVB instead.)
    TARGET_BOARD = EVB
else ifeq ($(HW),S32G3)
    $(warning Obsoleted HW=$(HW) platform name. Please use HW=RDB3 instead.)
    TARGET_BOARD = RDB3
else ifeq ($(HW),S32G3_REV1_1)
    $(warning Obsoleted HW=$(HW) platform name. Please use HW=EVB3 instead.)
    TARGET_BOARD = EVB3
endif
################################################################################

ifeq ($(TARGET_BOARD),EVB)
    HW_PLATFORM_G2 = TRUE
else ifeq ($(TARGET_BOARD),EVB3)
    HW_PLATFORM_G3 = TRUE
else ifeq ($(TARGET_BOARD),RDB2)
    HW_PLATFORM_G2 = TRUE
else ifeq ($(TARGET_BOARD),RDB3)
    HW_PLATFORM_G3 = TRUE
else
    $(error Error: HW=$(HW) is not a valid HW platform! Use make help to display valid targets)
endif

ODIR := $(ODIR)_$(TARGET_BOARD)
GDIR := $(GDIR)_$(TARGET_BOARD)
PROJ_CONFIG_NAME := $(PROJ_CONFIG_NAME)_$(TARGET_BOARD)

# Copy ETH_43_PFE.xdm to config project
ifeq ($(FCI_L2BR_TEST), TRUE)
    PRE_GENERATE_CMD = cp eth_pfe_configs/00_pfe_fci_l2br_test/$(PROJ_CONFIG_NAME)/Eth_1.xdm $(PROJ_CONFIG_NAME)/config
else
    PRE_GENERATE_CMD = cp eth_pfe_configs/01_pfe_normal/$(PROJ_CONFIG_NAME)/Eth_1.xdm $(PROJ_CONFIG_NAME)/config
endif

################################################################################
# Compiler specific
################################################################################
ifeq ($(COMPILER),GHS)
    ifeq ($(LWIP), TRUE)
        $(error LWIP cannot be used with GHS. It can only be compiled with GCC)
    endif

    COMPILER_LC := ghs
    CC := $(GHS_DIR)/ccthumb.exe
    LD := $(GHS_DIR)/ccthumb.exe
    AS := $(GHS_DIR)/ccthumb.exe

    LINKER_DEF = $(LINKER_DIR)/autosar_intram.ld
    CLIB := $(GHS_DIR)/lib/thumb2

    CCOPT := \
        -DGHS \
        -c \
        -cpu=cortexm7 \
        -thumb \
        -fpu=vfpv5_d16 \
        -fsingle \
        -C99 \
        --ghstd=last \
        --gnu_asm \
        -dual_debug \
        -G \
        -keeptempfiles \
        -Wimplicit-int \
        -Wshadow \
        -Wtrigraphs \
        -Wundef \
        --unsigned_chars \
        --unsigned_fields \
        --no_commons \
        --no_exceptions \
        --no_slash_comment \
        --prototype_errors \
        --incorrect_pragma_warnings

    CFLAGS_WERROR=--quit_after_warnings

    ifeq ($(DEBUG),FALSE)
        CCOPT += -Osize
    else
        CCOPT += -O0
    endif

    LDOPT := \
        -e Reset_Handler \
        -T $(LINKER_DEF) \
        -map \
        -keepmap \
        -Mn \
        -delete \
        -ignore_debug_references \
        -L$(CLIB) \
        -larch \
        -lstartup \
        -lind_sd \
        -lnk=-v \
        -nostartfiles

    ASOPT := \
        -cpu=cortexm7 \
        -c \
        -preprocess_assembly_files \
        -list

else
ifeq ($(COMPILER),GCC)
    COMPILER_LC := gcc
    CC := $(GCC_DIR)/bin/arm-none-eabi-gcc.exe
    LD := $(GCC_DIR)/bin/arm-none-eabi-gcc.exe
    AS := $(GCC_DIR)/bin/arm-none-eabi-gcc.exe
    CP := $(GCC_DIR)/bin/arm-none-eabi-objcopy.exe

    LINKER_DEF = $(LINKER_DIR)/autosar_intram.gld
    CLIB  := $(GCC_DIR)/arm-none-eabi/newlib

    CCOPT := \
        -DGCC \
        -c \
        -mcpu=cortex-m7 \
        -mthumb \
        -std=c99 \
        -mfpu=fpv5-sp-d16 \
        -mfloat-abi=hard  \
        -ggdb3 \
        -mlittle-endian \
        -Wall \
        -Wextra \
        -Wstrict-prototypes \
        -Wundef \
        -Wunused \
        -Werror=implicit-function-declaration \
        -Wsign-compare \
        -Wdouble-promotion \
        -fno-short-enums \
        -funsigned-char \
        -funsigned-bitfields \
        -fomit-frame-pointer \
        -fno-common \
        -pedantic \
        --sysroot="$(CLIB)"

    CFLAGS_WERROR=-Werror

    ifeq ($(DEBUG),FALSE)
        CCOPT += -Os
        CCOPT += -fstack-usage
        CCOPT += -fdump-ipa-all
    else
        CCOPT += -O0
    endif

    ifeq ($(CALL_GRAPH),TRUE)
        CCOPT += -fdump-rtl-expand
    endif

    LDOPT := \
        --entry=Reset_Handler \
        -T$(LINKER_DEF) \
        --sysroot="$(CLIB)" \
        -specs=nano.specs \
        -specs=nosys.specs \
        -nostartfiles \
        -mcpu=cortex-m7 \
        -mthumb \
        -mfpu=fpv5-sp-d16 \
        -mfloat-abi=hard  \
        -ggdb3 \
        -Wl,-Map,$(ODIR)/$(APP_NAME).map \
        -lc \
        -lm \
        -lgcc \
        -mlittle-endian

    ASOPT := \
        -c \
        -mthumb \
        -mcpu=cortex-m7 \
        -x assembler-with-cpp

else
ifeq ($(COMPILER),DIAB)
    ifeq ($(LWIP), TRUE)
        $(error LWIP cannot be used with DIAB. It can only be compiled with GCC)
    endif

    COMPILER_LC := diab
    CC := $(DIAB_DIR)/WIN64/bin/dcc.exe
    LD := $(DIAB_DIR)/WIN64/bin/dld.exe
    AS := $(CC)

    LINKER_DEF := $(LINKER_DIR)/autosar_intram.dld
    LIB_PATH := $(DIAB_DIR)/ARMV7EMMG

    CCOPT := \
        -tARMCORTEXM7MG:simple \
        -DDIAB \
        -c \
        -mthumb \
        -std=c99 \
        -g \
        -fstandalone-debug \
        -Wstrict-prototypes \
        -Wsign-compare \
        -Wdouble-promotion \
        -Wunknown-pragmas \
        -Wundef \
        -Wextra \
        -Wall \
        -pedantic \
        -Werror=implicit-function-declaration \
        -fno-common \
        -fno-signed-char \
        -fno-trigraphs

    CFLAGS_WERROR=-Werror

    ifeq ($(DEBUG),FALSE)
        CCOPT += -Oz
        CCOPT += -V
    else
        CCOPT += -O0
    endif

    LDOPT := \
        $(LINKER_DEF) \
        -e Reset_Handler \
        -m30 \
        -Xstack-usage \
        -Xpreprocess-lecl \
        -L$(LIB_PATH) \
        -lc \
        -lm \
        -@O=$(ODIR)/$(APP_NAME).map

    ASOPT := \
        -c \
        -mthumb \
        -Xpreprocess-assembly \
        -Xassembly-listing

else
    $(error "This compiler is not supported")
endif
endif
endif

################################################################################
# Path to folders
################################################################################
ifeq ($(LOCAL_BUILD),TRUE)
    WORKSPACE_DIR := $(LOCAL_DIR)
    include $(LOCAL_DIR)/mcal_eth/file_list.mak
    PFE_INCLUDE_DIRS := $(sort $(dir $(HEADER_FILES) $(HEADER_FILES_NO_M4)))
    PFE_SRC_DIRS     := $(sort $(dir $(SOURCE_FILES)))
    PFE_SOURCE_FILES := $(notdir $(SOURCE_FILES))
else
    PFE_INCLUDE_DIRS := $(PLUGINS_DIR)/Eth_43_PFE_$(AR_PFE_PKG_NAME)/include
    PFE_SRC_DIRS     := $(PLUGINS_DIR)/Eth_43_PFE_$(AR_PFE_PKG_NAME)/src
    PFE_SOURCE_FILES := $(notdir $(wildcard $(PFE_SRC_DIRS)/*.c))
endif

INCLUDE_DIRS  = include \
                $(PFE_INCLUDE_DIRS) \
                $(GDIR)/include \
                $(PLUGINS_DIR)/BaseNXP_$(AR_MCAL_PKG_NAME)/header \
                $(PLUGINS_DIR)/Platform_$(AR_MCAL_PKG_NAME)/startup/include \
                $(foreach mod,$(MCAL_MODULE_LIST),$(PLUGINS_DIR)/$(mod)_$(AR_MCAL_PKG_NAME)/include)

SRC_DIRS      = src \
                $(GDIR)/src \
                $(PLUGINS_DIR)/Platform_$(AR_MCAL_PKG_NAME)/startup/src \
                $(PLUGINS_DIR)/Platform_$(AR_MCAL_PKG_NAME)/startup/src/m7 \
                $(PLUGINS_DIR)/Platform_$(AR_MCAL_PKG_NAME)/startup/src/m7/$(COMPILER_LC) \
                $(foreach mod,$(MCAL_MODULE_LIST),$(PLUGINS_DIR)/$(mod)_$(AR_MCAL_PKG_NAME)/src)

ifeq ($(SLAVE_DRIVER),FALSE)
    INCLUDE_DIRS    +=  $(FW_DIR)
    SRC_DIRS        +=  $(FW_DIR)
endif

ifeq ($(LWIP),TRUE)
    LWIPDIR := $(LWIP_DIR)/src
    include $(LWIPDIR)/Filelists.mk
    LWIP_SOURCE_PATHS    += $(COREFILES) \
                            $(CORE4FILES) \
                            $(APIFILES) \
                            $(NETIFFILES) \
                            $(HTTPFILES) \
                            $(LWIPERFFILES)
    INCLUDE_DIRS += $(LWIPDIR)/include
    SRC_DIRS += $(sort $(foreach PATH,$(LWIP_SOURCE_PATHS), $(dir $(PATH))))
endif


################################################################################
# Generic compiler options
################################################################################
ifeq ($(HW_PLATFORM_G2),TRUE)
    CCOPT += -DS32G2XX
else ifeq ($(HW_PLATFORM_G3),TRUE)
    CCOPT += -DS32G3XX
else
    $(error Error: Unknown HW family.)
    # should not happen!
endif

CCOPT += \
        -DS32XX \
        -DUSE_SW_VECTOR_MODE \
        -DD_CACHE_ENABLE \
        -DI_CACHE_ENABLE \
        -DENABLE_FPU \
        -DPFE_CFG_VERBOSITY_LEVEL=$(VERBOSITY_LEVEL) \
        -DTARGET_$(TARGET_BOARD)

ifeq ($(LOCAL_BUILD),TRUE)
    # supply expanded M4 macros via CCOPT
    M4_MACROS = 1       5       0      \
                4  4  0 \
                b41fcd76405916e88d43a560a9f56a50

    CCOPT += -DM4_FILE_VERSION_CHECK_HDR= \
             -DM4_FILE_VERSION_CHECK_SRC= \
             $(foreach macro, $(M4_MACROS), -D$(macro)=$(shell echo $(macro) | m4 -P $(LOCAL_DIR)/mcal_eth/version_def.m4 -))
endif

#Use the usermode
ifeq ($(USER_MODE),TRUE)
    CCOPT +=-DMCAL_ENABLE_USER_MODE_SUPPORT
    ASOPT +=-DMCAL_ENABLE_USER_MODE_SUPPORT
endif

ifeq ($(DEBUG),TRUE)
    CCOPT := $(CCOPT) \
             -DNXP_LOG_ENABLED \
             -DPFE_CFG_NULL_ARG_CHECK \
             -DLWIP_DEBUG

# LWIP_DEBUG is very heavy, so each module need to be enabled individually.
# Here is an example on how to do this:
    #CCOPT += -DTCP_DEBUG=LWIP_DBG_ON


# check if PFE statistics to text functionality should be enabled
    ifeq ($(TEXT_STATS),TRUE)
        CCOPT := $(CCOPT) \
                 -DPFE_CFG_TEXT_STATS
    endif
    ifeq ($(LWIP),TRUE)
            CCOPT := $(CCOPT) \
                     -DLWIP_NETIF_STATUS_CALLBACK=$(LWIP_STATUS_CALLBACK)
    endif
endif

# Enable PFE_DEV_ERROR_DETECT macro for CERT C analysis
ifeq ($(CERT_C),TRUE)
    CCOPT := $(CCOPT) \
             -DPFE_DEV_ERROR_DETECT
endif

# Configure GPT channel to use for TM Stub and switch tasks
TM_STUB_GPT_CHANNEL ?= GptConf_GptChannelConfiguration_GptChannelConfiguration_$(TM_STUB_GPT_CHANNEL_CFG)
SWITCH_TASK_GPT_CHANNEL ?= GptConf_GptChannelConfiguration_GptChannelConfiguration_$(SWITCH_TASK_GPT_CHANNEL_CFG)
CCOPT :=  $(CCOPT) \
          -DTM_STUB_GPT_CHANNEL=$(TM_STUB_GPT_CHANNEL) \
          -DSWITCH_TASK_GPT_CHANNEL=$(SWITCH_TASK_GPT_CHANNEL)

ifeq ($(CONFIG_MINIHIF),TRUE)
    CCOPT :=  $(CCOPT) \
              -DCONFIG_MINIHIF
endif

# Configure HIF channel to use for minihif detached driver
# HIF0 is used by master, do not use it
# HIF3 is used by default
ifeq ($(CONFIG_MINIHIF_HW), HIF0)
    MINIHIF_HW_USE := PFE_PHY_IF_ID_HIF3
else
    MINIHIF_HW_USE := PFE_PHY_IF_ID_$(CONFIG_MINIHIF_HW)
endif
CCOPT :=  $(CCOPT) \
          -DMINIHIF_HW_USE=$(MINIHIF_HW_USE)
ifeq ($(LOOPBACK_TEST),TRUE)
    CCOPT :=  $(CCOPT) \
              -DLOOPBACK_TEST
endif
ifeq ($(LWIP),TRUE)
    CCOPT :=    $(CCOPT) \
                -DLWIP \
                -DLWIP_DEFAULT_GATEWAY_0=\"$(LWIP_DEFAULT_GATEWAY_0)\" \
                -DLWIP_DEFAULT_GATEWAY_1=\"$(LWIP_DEFAULT_GATEWAY_1)\" \
                -DLWIP_DEFAULT_GATEWAY_2=\"$(LWIP_DEFAULT_GATEWAY_2)\" \
                -DLWIP_MASK=\"$(LWIP_MASK)\"
endif
ifeq ($(USE_SERDES_DRIVER),TRUE)
    CCOPT :=  $(CCOPT) \
              -DUSE_SERDES_DRIVER
endif
ifeq ($(FCI_L2BR_TEST),TRUE)
    CCOPT :=  $(CCOPT) \
              -DFCI_L2BR_TEST
endif
ifeq ($(PING_TEST),TRUE)
    CCOPT :=  $(CCOPT) \
              -DPING_TEST
endif
CCOPT :=  $(CCOPT) \
          -DIPV4_LOCAL_IP_ADDR_0=\"$(IP_ADDRESS_CTRL_0)\" \
          -DIPV4_LOCAL_IP_ADDR_1=\"$(IP_ADDRESS_CTRL_1)\" \
          -DIPV4_LOCAL_IP_ADDR_2=\"$(IP_ADDRESS_CTRL_2)\" \
          -DIPV6_LOCAL_IP_ADDR_0=\"$(IPV6_ADDRESS_CTRL_0)\" \
          -DIPV6_LOCAL_IP_ADDR_1=\"$(IPV6_ADDRESS_CTRL_1)\" \
          -DIPV6_LOCAL_IP_ADDR_2=\"$(IPV6_ADDRESS_CTRL_2)\" \
          -DRTABLE_IPV4_SRC_IP_ADDR=\"$(RTABLE_SRC_IP_ADDRESS)\" \
          -DRTABLE_IPV4_DES_IP_ADDR=\"$(RTABLE_DES_IP_ADDRESS)\" \
          -DRTABLE_IPV4_SRC_PORT=$(RTABLE_SRC_PORT) \
          -DRTABLE_IPV4_DES_PORT=$(RTABLE_DES_PORT)
ifeq ($(FCI_RTABLE_TEST),TRUE)
    CCOPT :=  $(CCOPT) \
              -DFCI_RTABLE_TEST
endif
ifeq ($(SLAVE_DRIVER),TRUE)
    CCOPT :=  $(CCOPT) \
              -DUSE_SLAVE_DRIVER \
              -DMULTIPLE_IMAGE

    ASOPT += -DMULTIPLE_IMAGE
else
    ifeq ($(CONFIGURE_SOC),TRUE)
        CCOPT :=  $(CCOPT) \
                  -DCONFIGURE_SOC
    endif
endif

# For compiling/linking into ram
CCOPT       := $(CCOPT) -DSINTRAM
CCOPT       := $(CCOPT) -DDISABLE_MCAL_INTERMODULE_ASR_CHECK

# Add define for HM string
CCOPT += -DPFE_CFG_HM_STRINGS_ENABLED
################################################################################
# Compiled files
################################################################################
# $(addsuffix suffix,names…)  : $(addsuffix .c,foo bar) ->  produces the result ‘foo.c bar.c’
#                             : $(addprefix src/,foo bar) -> produces the result ‘src/foo src/bar’
# Output: -Iinclude -I../include
CCINCLUDES=$(addprefix -I, $(INCLUDE_DIRS))
# Output: List all path .h files (include/Fls_TestSetup.h include/Platform_Types.h ....)
INCLUDE_FILES := $(foreach DIR, $(INCLUDE_DIRS),$(wildcard $(DIR)/*.h))

ifeq ($(PING_TEST), FALSE)
INCLUDE_FILES_EXC += include/queue.h

INCLUDE_FILES := $(filter-out $(INCLUDE_FILES_EXC), $(INCLUDE_FILES))
endif

ifeq ($(USE_SERDES_DRIVER), FALSE)
INCLUDE_FILES_EXC += $(GDIR)/include/Serdes_Cfg.h \
                     $(GDIR)/include/Serdes_Ip_Cfg.h \
                     $(GDIR)/include/Serdes_Ip_CfgDefines.h \
                     $(GDIR)/include/Serdes_Ip_PBCfg.h \
                     $(GDIR)/include/Serdes_Ipw_PBCfg.h

INCLUDE_FILES := $(filter-out $(INCLUDE_FILES_EXC), $(INCLUDE_FILES))
endif

ifeq ($(LWIP),FALSE)
INCLUDE_FILES_EXC += $(GDIR)/include/lwip_demo.h \
                     $(GDIR)/include/lwipopts.h \
                     $(GDIR)/include/arch/cc.h

INCLUDE_FILES := $(filter-out $(INCLUDE_FILES_EXC), $(INCLUDE_FILES))
else
# This file and its .c file is excluded because of compiler warnings in LWIP
INCLUDE_FILES_EXC += $(GDIR)/include/http_client.h
INCLUDE_FILES := $(filter-out $(INCLUDE_FILES_EXC), $(INCLUDE_FILES))
endif

# Output: List all .c and .s files (Fls_TestSetup.c exceptions.c Startup.s Vector_core.s)
SOURCE_FILES := $(PFE_SOURCE_FILES) \
                $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.c))) \
                $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.s))) \
                $(foreach DIR,$(SRC_DIRS),$(notdir $(wildcard $(DIR)/*.S)))

ifeq ($(LWIP),TRUE)
SOURCE_FILES_EXC +=fsdata.c \
                   http_client.c

SOURCE_FILES := $(filter-out $(SOURCE_FILES_EXC), $(SOURCE_FILES))
else
SOURCE_FILES_EXC += lwip_main.c \
                    lwip_ports.c

SOURCE_FILES := $(filter-out $(SOURCE_FILES_EXC), $(SOURCE_FILES))
endif

ifeq ($(PING_TEST), FALSE)
SOURCE_FILES_EXC +=queue.c

SOURCE_FILES := $(filter-out $(SOURCE_FILES_EXC), $(SOURCE_FILES))
endif

ifeq ($(USE_SERDES_DRIVER), FALSE)
SOURCE_FILES_EXC +=Serdes_Cfg.c \
                   Serdes_Ip_PBCfg.c \
                   Serdes_Ipw_PBCfg.c

SOURCE_FILES := $(filter-out $(SOURCE_FILES_EXC), $(SOURCE_FILES))
endif

################################################################################
# Object files
################################################################################
OUT_FILES := $(SOURCE_FILES:.c=.o)
OUT_FILES := $(OUT_FILES:.s=.o)
OUT_FILES := $(OUT_FILES:.S=.o)

OUT_FILES := $(addprefix $(ODIR)/, $(OUT_FILES))

################################################################################
# Targets
################################################################################
.PHONY: build
build: $(ODIR) $(OUT_FILES) $(ODIR)/$(APP_NAME).elf

vpath %.c $(addsuffix :, $(SRC_DIRS) $(PFE_SRC_DIRS))

$(ODIR)/%.o: %.c $(INCLUDE_FILES)
	@echo CC $<
	@$(CC) $(CCOPT) $(if $(findstring Eth_43_PFE, $<),$(CFLAGS_WERROR),) $(CCINCLUDES) $< -o $@

vpath %.s $(addsuffix :, $(SRC_DIRS))
vpath %.S $(addsuffix :, $(SRC_DIRS))

$(ODIR)/%.o : %.s
	@echo AS $<
	@$(AS) $(ASOPT) $< -o $@

$(ODIR)/%.o : %.S
	@echo AS $<
	@$(AS) $(ASOPT) $< -o $@

# Link all the object files to become one elf file
$(ODIR)/$(APP_NAME).elf: $(OUT_FILES) $(LINKER_DEF)
	@echo LD $@
	$(LD) $(LDOPT) $(OUT_FILES) -o $@
	$(CP) -O binary $(ODIR)/$(APP_NAME).elf $(ODIR)/$(APP_NAME).bin


$(ODIR):
	mkdir -p $(ODIR)

.PHONY : clean_all
clean_all: clean_gen clean

.PHONY: clean_gen
clean_gen:
	rm -fr $(GDIR)
	-$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) deleteProject $(PROJ_CONFIG_NAME) 2>&1

.PHONY: clean
clean:
	rm -fr $(ODIR)

.PHONY: generate
generate:
	$(PRE_GENERATE_CMD)
	-$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) importProject $(shell cygpath -m $(abspath $(PROJ_CONFIG_NAME))) 2>&1
	$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) autoconfigure $(PROJ_CONFIG_NAME) GenerateAllVariants 2>&1
	$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) make generate_swcd $(PROJ_CONFIG_NAME) 2>&1
	$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) make verify_swcd $(PROJ_CONFIG_NAME) 2>&1
ifneq ($(MISRA_SCAN),TRUE)
	$(TRESOS_DIR)/bin/tresos_cmd.bat -data $(TRESOS_WORKSPACE_DIR) import $(PROJ_CONFIG_NAME) bswmd 2>&1
endif
.PHONY : help
help :
	@Option () { \
		local vals=""; \
		local argv=("$$@"); \
		for (( i = 2; i < $$#; i += 1 )) do \
			if [ "$$vals" != "" ]; then \
				vals+=" | "; \
			fi; \
			if [ "$$2" = "$${argv[$$i]}" ]; then \
				vals+="\e[32m$${argv[$$i]}\e[0m"; \
			else \
				vals+=$${argv[$$i]}; \
			fi; \
		done; \
		echo -e "$$1 { $$vals }"; \
	}; \
	echo; \
	echo -e "Targets (\e[32mdefault\e[0m):"; \
	echo "  help - just print this and exit"; \
    echo "  generate - generate headers and sources from a configuration project"; \
	echo -e "  \e[32mbuild\e[0m - to build application (default target)"; \
	echo "  clean - delete objects files and output file"; \
	echo "  clean_gen - delete generated files"; \
	echo "  clean_all - clean + clean_gen"; \
	echo ;\
	echo -e "Options (\e[32mcurrent value\e[0m):"; \
	echo -e "  TRESOS_DIR - TRESOS installation path (\e[32m$(TRESOS_DIR)\e[0m)"; \
	echo -e "  PLUGINS_DIR - PFE plugin path (\e[32m$(PLUGINS_DIR)\e[0m)"; \
	echo -e "  TRESOS_WORKSPACE_DIR - TRESOS workspace used for build (\e[32m$(TRESOS_WORKSPACE_DIR)\e[0m)"; \
	echo -e "  FW_DIR - Path to PFE firmware to include in build (\e[32m$(FW_DIR)\e[0m)"; \
	echo "           If empty no firmware will be included and proper firmware location must be specified via PfeClassFwMemoryAddress parameter in TRESOS"; \
	echo -e "  GHS_DIR - Path to the GreenHills compiler instalation dir (\e[32m$(GHS_DIR)\e[0m)"; \
	echo -e "  GCC_DIR - Path to the GCC compiler instalation dir (\e[32m$(GCC_DIR)\e[0m)"; \
	echo -e "  DIAB_DIR - Path to the  WinderRiver compiler instalation dir (\e[32m$(DIAB_DIR)\e[0m)"; \
	Option "  LOCAL_BUILD - Build PFE driver from local repo " $(LOCAL_BUILD) "TRUE" "FALSE"; \
	echo -e "  LOCAL_DIR - Path to the local PFE driver repo dir (\e[32m$(LOCAL_DIR)\e[0m)"; \
	Option "  COMPILER - To use for build " $(COMPILER) "GCC" "GHS" "DIAB"; \
	Option "  DEBUG - Debug mode " $(DEBUG) "TRUE" "FALSE"; \
	Option "  HW - Target hardware platform " $(HW) "EVB" "EVB3" "RDB2" "RDB3"; \
	Option "  TEXT_STATS - Build PFE text statistics functionality (in case of DEBUG = TRUE)" $(TEXT_STATS) "TRUE" "FALSE"; \
	Option "  SWITCH_TASK_GPT_CHANNEL_CFG -" $(SWITCH_TASK_GPT_CHANNEL_CFG) "0" "1"; \
	Option "  TM_STUB_GPT_CHANNEL_CFG -" $(TM_STUB_GPT_CHANNEL_CFG) "0" "1"; \
	Option "  CONFIG_MINIHIF -" $(CONFIG_MINIHIF) "TRUE" "FALSE"; \
	Option "  CONFIG_MINIHIF_HW  -" $(CONFIG_MINIHIF_HW) "HIF0" "HIF1" "HIF2" "HIF3"; \
	Option "  LOOPBACK_TEST -" $(LOOPBACK_TEST) "TRUE" "FALSE"; \
	Option "  USE_SERDES_DRIVER -" $(USE_SERDES_DRIVER) "TRUE" "FALSE"; \
	echo -e "  VERBOSITY_LEVEL - of message output 1 - 10 (\e[32m$(VERBOSITY_LEVEL)\e[0m)"; \
	Option "  FCI_L2BR_TEST -" $(FCI_L2BR_TEST) "TRUE" "FALSE"; \
	Option "  FCI_RTABLE_TEST -" $(FCI_RTABLE_TEST) "TRUE" "FALSE"; \
	Option "  PING_TEST -" $(PING_TEST) "TRUE" "FALSE"; \
	Option "  SLAVE_DRIVER -" $(SLAVE_DRIVER) "TRUE" "FALSE"; \
	Option "  CONFIGURE_SOC -" $(CONFIGURE_SOC) "TRUE" "FALSE"; \
	Option "  CERT_C - Enable for CERT C static analysis " $(CERT_C) "TRUE" "FALSE"; \
	Option "  LWIP - Enable LWIP stack with iPerf. LWIP_DIR must be defined in environment " $(LWIP) "TRUE" "FALSE";
