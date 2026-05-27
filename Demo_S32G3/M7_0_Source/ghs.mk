#   Copyright 2023-2024 NXP
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
cbin        := $(GHS_DIR)
cinc        := .
clib        := $(GHS_DIR)/lib/thumb2

CC          := $(cbin)/ccthumb.exe
LD          := $(cbin)/ccthumb.exe
AS          := $(cbin)/ccthumb.exe

OT          := $(cbin)/gmemfile.exe

#linker definition  per compiler
LINKER_DEF = lcf/$(compiler)/linker_ram.ld

################################################################################
# Compiler options
################################################################################
CCOPT           :=  $(CCOPT) \
                    $(MISRA) \
                    -D$(PLATFORM) \
                    -D$(FAMILY) \
                    -D$(DERIVATIVE) \
                    -DGHS \
                    $(SUBDERIVATIVE_NAME) \
                    -DEU_DISABLE_ANSILIB_CALLS \
                    -DUSE_SW_VECTOR_MODE \
                    -c \
                    -cpu=cortexm7 \
                    -thumb \
                    -fpu=vfpv5_d16 \
                    -fsingle\
                    -C99 \
                    --ghstd=last \
                    -Osize \
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
                    --incorrect_pragma_warnings \
                    $(foreach spec, $(specs), -specs=$(spec))

LDOPT           :=  -map=$(MAPFILE) \
                    -e Reset_Handler \
                    -map \
                    -keepmap \
                    -Mn \
                    -delete \
                    -ignore_debug_references \
                    -lnk=-keep=C40_Ip_AccessCode \
                    -L$(clib) \
                    -larch \
                    -lstartup \
                    -lind_sd \
                    -lnk=-v \
                    -nostartfiles \
                    -T $(LINKER_DEF)

ASOPT           :=  $(ASOPT_CORE) \
                    -c \
                    -cpu=cortexm7 \
                    -fpu=vfpv5_d16 \
                    -fsingle\
                    -preprocess_assembly_files \
                    -list

OTOPT :=    -o
