#
# Copyright 2025 NXP.
#

include config.mk

ifeq ($(LWIP), TRUE)
    LWIP_MASK ?= 255.255.255.0
    LWIP_DEFAULT_GATEWAY_0 ?= 10.0.0.1
    LWIP_DEFAULT_GATEWAY_1 ?= 10.0.1.1
    LWIP_DEFAULT_GATEWAY_2 ?= 10.0.2.1
endif

INC_RTD_COMMON_PLUGINS = -I$(PLUGIN_DIR)/BaseNXP_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/BaseNXP_TS_$(MCAL_PLUGIN_DIR_VER)/header \
						-I$(PLUGIN_DIR)/Det_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/CanIf_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/EcuM_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/EthTrcv_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/Gpt_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/EthSwt_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/Dem_TS_$(MCAL_PLUGIN_DIR_VER)/include \
						-I$(PLUGIN_DIR)/Mcu_TS_$(MCAL_PLUGIN_DIR_VER)/include \


CC_INCLUDES += $(INC_RTD_COMMON_PLUGINS) \
			-I$(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE \
			-I$(PFE_DEMO_DIR)/include  \
			-I$(LLCE_DEMO_DIR)/include \
			-I$(IPCF_DEMO_DIR)/include \
			-I$(A53_MCAL_CONFIG_DIR)/include \
			-I$(LLCE_PLUGIN_DIR)/Can_43_LLCE_TS_$(LLCE_PLUGIN_DIR_VER)/include \
			-I$(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/common \
			-I$(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/hw \
			-I$(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/hw/s32g3xx \
			-I$(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/os \
			-I$(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/os/baremetal \
			-I$(PLUGIN_DIR)/Eth_43_PFE_TS_$(PFE_PLUGIN_DIR_VER)/include \
	
SRC_RTD_COMMON_PLUGINS += $(wildcard $(PLUGIN_DIR)/BaseNXP_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c)	\
						$(wildcard $(PLUGIN_DIR)/Det_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c)	\
						$(wildcard $(PLUGIN_DIR)/Dem_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c) \
						$(wildcard $(PLUGIN_DIR)/Mcu_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c) \
						$(wildcard $(PLUGIN_DIR)/Gpt_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c) \
						$(wildcard $(PLUGIN_DIR)/EthTrcv_$(MCAL_PLUGIN_DIR_VER)/src/*.c) \
						$(wildcard $(PLUGIN_DIR)/EthSwt_TS_$(MCAL_PLUGIN_DIR_VER)/src/*.c) \
						$(wildcard $(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/src/SchM_Mcu.c) \
						$(wildcard $(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/src/SchM_Can_43_LLCE.c) \
						$(wildcard $(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/src/SchM_Eth_43_PFE.c) \
						$(wildcard $(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/src/SchM_Gpt.c) 
							                         

C_SRC_LLCE_BIN := $(wildcard $(LLCE_BIN_DIR)/*.c)
C_SRC_CAN := $(wildcard $(LLCE_PLUGIN_DIR)/Can_43_LLCE_TS_$(LLCE_PLUGIN_DIR_VER)/src/*.c)
C_SRC_ETH := $(wildcard $(PLUGIN_DIR)/Eth_43_PFE_TS_$(PFE_PLUGIN_DIR_VER)/src/*.c)

C_SRC_IPCF += $(wildcard $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/common/*.c) \
			$(wildcard $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/hw/s32g3xx/ipc-hw-s32g3xx.c) \
			$(wildcard $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/os/baremetal/ipc-os-baremetal.c)

C_SRC_LLCE_DEMO := $(wildcard $(LLCE_DEMO_DIR)/src/*.c)
C_SRC_IPCF_DEMO := $(wildcard $(IPCF_DEMO_DIR)/src/*.c)
C_SRC_PFE_DEMO := $(wildcard $(PFE_DEMO_DIR)/src/*.c)

ifeq ($(PING_TEST),TRUE)
	C_SRC_PING_EXC := $(PFE_DEMO_DIR)/src/lwip_main.c \
					$(PFE_DEMO_DIR)/src/lwip_ports.c
	C_SRC_PFE_DEMO := $(filter-out $(C_SRC_PING_EXC), $(C_SRC_PFE_DEMO))
endif

ifeq ($(LWIP),TRUE)
	LWIPDIR := $(LWIP_DIR)/src
    include $(LWIPDIR)/Filelists.mk
    C_SRC_LWIP := $(COREFILES) \
				$(CORE4FILES) \
				$(APIFILES) \
				$(NETIFFILES) \
				$(HTTPFILES) \
				$(LWIPERFFILES) \

	C_SRC_LWIP_EXC += $(PLUGIN_DIR)/Eth_43_PFE_TS_$(PFE_PLUGIN_DIR_VER)/src/fsdata.c \
                   	$(PLUGIN_DIR)/Eth_43_PFE_TS_$(PFE_PLUGIN_DIR_VER)/src/http_client.c
	C_SRC_ETH := $(filter-out $(C_SRC_LWIP_EXC), $(C_SRC_ETH))
endif


ALL_SRC := \
	$(SRC_RTD_COMMON_PLUGINS) \
	$(C_SRC_LLCE_BIN) \
	$(C_SRC_CAN) \
	$(C_SRC_IPCF) \
	$(C_SRC_ETH)  \
 	$(C_SRC_LLCE_DEMO) \
	$(C_SRC_IPCF_DEMO) \
	$(C_SRC_PFE_DEMO) \

ifeq ($(LWIP),TRUE)
	ALL_SRC += $(C_SRC_LWIP)
endif
	

AS := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
CC := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
LD := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
CP := $(CrossToolDir)/bin/aarch64-none-elf-objcopy.exe
OD := $(CrossToolDir)/bin/aarch64-none-elf-objdump.exe


OSCFLAGS := -std=c99 -Os -funsigned-char  -fomit-frame-pointer                             \
			-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast                              \
			-ggdb3 -pedantic -Wall -Wextra -c -fno-short-enums -funsigned-bitfields        \
			-fno-common -Wunused -Wstrict-prototypes -Wsign-compare                        \
			-Werror=implicit-function-declaration -Wundef -mlittle-endian                  \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib" -mstrict-align                \
			-mcpu=cortex-a53 -specs=nosys.specs  -I"include" -I"$(OsPath)/Source/include"  \
			-DGUEST -DAARCH64_LLCE -DS32G3XX  \

ifeq ($(PING_TEST), TRUE)
	OSCFLAGS +=	-DPING_TEST
endif

ifeq ($(LWIP),TRUE)
    OSCFLAGS += -DLWIP \
                -DLWIP_DEFAULT_GATEWAY_0=\"$(LWIP_DEFAULT_GATEWAY_0)\" \
                -DLWIP_DEFAULT_GATEWAY_1=\"$(LWIP_DEFAULT_GATEWAY_1)\" \
                -DLWIP_DEFAULT_GATEWAY_2=\"$(LWIP_DEFAULT_GATEWAY_2)\" \
                -DLWIP_MASK=\"$(LWIP_MASK)\"                           \
				-DLWIP_NOASSERT                                        \
				-DSSIZE_MAX											   \
				-I"$(LWIPDIR)/include"                                 \
				-I"$(PFE_DEMO_DIR)/include/lwip_demo"                  \
				-I"$(PFE_DEMO_DIR)/include/arch"                       \

endif

OSCFLAGS += -DIPV4_LOCAL_IP_ADDR_0=\"$(IP_ADDRESS_CTRL_0)\"    \
          	-DIPV4_LOCAL_IP_ADDR_1=\"$(IP_ADDRESS_CTRL_1)\"    \
          	-DIPV4_LOCAL_IP_ADDR_2=\"$(IP_ADDRESS_CTRL_2)\"    \
          	-DIPV6_LOCAL_IP_ADDR_0=\"$(IPV6_ADDRESS_CTRL_0)\"  \
          	-DIPV6_LOCAL_IP_ADDR_1=\"$(IPV6_ADDRESS_CTRL_1)\"  \
          	-DIPV6_LOCAL_IP_ADDR_2=\"$(IPV6_ADDRESS_CTRL_2)\"  


OSASFLAGS := -c -x assembler-with-cpp -mlittle-endian -mcpu=cortex-a53 -specs=nosys.specs  \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib" -DGUEST 


LDFLAGS  := -nostartfiles --entry=Reset_Handler -ggdb3 -T "./image.ld"                       \
			-Wl,-Map,$(OutDir)/RTD_ON_A53.map -mlittle-endian -mcpu=cortex-a53 -specs=nosys.specs \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib"

OBJFILES_OS :=  $(OutDir)/portASM.o      \
				$(OutDir)/tasks.o        \
				$(OutDir)/timers.o       \
				$(OutDir)/event_groups.o \
				$(OutDir)/queue.o        \
				$(OutDir)/list.o         \
				$(OutDir)/croutine.o     \
				$(OutDir)/port.o         \
				$(OutDir)/heap_4.o

$(OutDir)/%.o: $(OsPath)/Source/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE/%.S
	$(AS) $(OSASFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(OsPath)/Source/portable/MemMang/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

OBJFILES_APP := $(OutDir)/core_utility.o     \
				$(OutDir)/exceptions_stack.o \
				$(OutDir)/exceptions.o       \
				$(OutDir)/gic.o              \
				$(OutDir)/main.o             \
				$(OutDir)/startup.o          \
				$(OutDir)/system.o           \
				$(OutDir)/tick_config.o      \
				$(OutDir)/Vector_Table.o     \
				$(OutDir)/mmu_init.o         \
				$(OutDir)/Can_43_LLCE_AFcfg.o \
				$(OutDir)/Can_43_LLCE_Cfg.o  \
				$(OutDir)/Can_43_LLCE_VS_0_PBcfg.o \
				$(OutDir)/ipcf_Ip_Cfg.o       \
				$(OutDir)/Eth_43_PFE_Cfg.o \
				$(OutDir)/Eth_43_PFE_VS_0_PBcfg.o \
				$(OutDir)/Gpt_VS_0_PBcfg.o  \
				$(OutDir)/Gpt_Ipw_VS_0_PBcfg.o \
				$(OutDir)/Pit_Ip_VS_0_PBcfg.o \
				$(OutDir)/Clock_Ip_VS_0_PBcfg.o \
				$(OutDir)/Mcu_VS_0_PBcfg.o \
				$(OutDir)/Power_Ip_VS_0_PBcfg.o
			
				
$(OutDir)/%.o: src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: src/%.s
	$(AS) $(OSASFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: src/%.S
	$(AS) $(OSASFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(LLCE_BIN_DIR)/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

ifeq ($(LWIP),TRUE)	

$(OutDir)/%.o: $(LWIPDIR)/core/%.c 
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(LWIPDIR)/core/ipv4/%.c 
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(LWIPDIR)/api/%.c 
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)	

$(OutDir)/%.o: $(LWIPDIR)/netif/%.c 
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)	

$(OutDir)/%.o: $(LWIPDIR)/apps/http/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)   
	
$(OutDir)/%.o: $(LWIPDIR)/apps/lwiperf/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)                          
endif

$(OutDir)/%.o: $(PFE_DEMO_DIR)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(LLCE_DEMO_DIR)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES) 

$(OutDir)/%.o: $(IPCF_DEMO_DIR)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(A53_MCAL_CONFIG_DIR)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Eth_43_PFE_TS_$(PFE_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(LLCE_PLUGIN_DIR)/Can_43_LLCE_TS_$(LLCE_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/common/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/hw/s32g3xx/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(IPCF_PLUGIN_DIR)/Ipcf_TS_$(IPCF_PLUGIN_DIR_VER)/src/os/baremetal/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/BaseNXP_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Det_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Rte_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Mcu_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/EthSwt_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Gpt_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/EthTrcv_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

$(OutDir)/%.o: $(PLUGIN_DIR)/Dem_TS_$(MCAL_PLUGIN_DIR_VER)/src/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $< $(CC_INCLUDES)

	
########################################################################################

SRC_TARGET_WITHOUT_PATH = $(notdir $(ALL_SRC))

ALL_OBJ := $(SRC_TARGET_WITHOUT_PATH:%.c=$(OutDir)/%.o)

OBJFILES := $(OBJFILES_OS) $(OBJFILES_APP) $(ALL_OBJ) 

# default goal
all: clean make_dir $(OutDir)/RTD_ON_A53.elf

clean:
	rm -f $(OutDir)/*

make_dir:
	mkdir -p $(OutDir)
# Linking
$(OutDir)/RTD_ON_A53.elf: $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJFILES)
	$(CP) -S -I elf32-littlearm -O binary $(OutDir)/RTD_ON_A53.elf $(OutDir)/RTD_ON_A53.bin
	$(OD) -m arm -Mforce-thumb --target=binary -D $(OutDir)/RTD_ON_A53.bin > $(OutDir)/RTD_ON_A53.asm
	$(OD)  -D $(OutDir)/RTD_ON_A53.elf > $(OutDir)/RTD_ON_A53.S
	sh ./padding.sh
