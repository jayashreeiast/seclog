#   Copyright 2023 NXP
#
#   NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
#   in accordance with the applicable license terms.  By expressly accepting
#   such terms or by downloading, installing, activating and/or otherwise using
#   the software, you are agreeing that you have read, and that you agree to
#   comply with and are bound by, such license terms.  If you do not agree to
#   be bound by the applicable license terms, then you may not retain,
#   install, activate or otherwise use the software.
#
#   This file contains sample code only. It is not part of the production code deliverables.
#

# Compiler paths
cbin        := $(GCC_DIR)/bin
cinc        := .
clib        := $(GCC_DIR)/arm-none-eabi/newlib
specs       := nano.specs \
               nosys.specs

CC          := $(cbin)/arm-none-eabi-gcc.exe
LD          := $(cbin)/arm-none-eabi-gcc.exe
AS          := $(cbin)/arm-none-eabi-gcc.exe
OT          := $(cbin)/arm-none-eabi-objcopy.exe

# linker definition  per compiler
LINKER_DEF = lcf/$(compiler)/linker_ram.ld

################################################################################
# Compiler options
################################################################################
CCOPT   :=  $(CCOPT) \
            $(MISRA) \
            -D$(PLATFORM) \
            -D$(FAMILY) \
            -D$(DERIVATIVE) \
            -DGCC \
            $(SUBDERIVATIVE_NAME) \
            -DEU_DISABLE_ANSILIB_CALLS \
            -DUSE_SW_VECTOR_MODE \
            -c \
            -mcpu=cortex-m7 \
            -mthumb \
            -std=c99 \
            -Os \
            -mfpu=fpv5-sp-d16 \
            -mfloat-abi=hard \
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
            --sysroot="$(clib)" \
            $(foreach spec, $(specs), -specs=$(spec))

LDOPT   :=  -Wl,-Map,"$(MAPFILE)" \
            --entry=Reset_Handler \
            --sysroot="$(clib)" \
            $(foreach spec, $(specs), -specs=$(spec)) \
            -nostartfiles \
            -mcpu=cortex-m7 \
            -mthumb \
            -mfpu=fpv5-sp-d16 \
            -mfloat-abi=hard \
            -ggdb3 \
            -mlittle-endian \
            -lc \
            -lm \
            -lgcc \
            -T $(LINKER_DEF)

ASOPT   :=  $(ASOPT_CORE) \
            -c \
            -mthumb \
            -mcpu=cortex-m7 \
            -mfpu=fpv5-sp-d16 \
            -mfloat-abi=hard \
            -x assembler-with-cpp

OTOPT :=    -O binary
