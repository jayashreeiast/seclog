# =================================================================================================
#
#    @file        bin_to_array.py
#
#    @brief       This file converts binary files from .bin to .dat format which can be passed as
#                 input to a Lauterbach TRACE 32 cmm script.
#    @input       File location on disk of a binary image
#                 Endianness(BIG/LITTLE)
#    @output      Trace32 .dat file
#
# =================================================================================================
# @copyright (c)  Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
#                 All Rights Reserved.
#
#                 This file contains sample code only.
#                 It is not part of the production code deliverables.
# =================================================================================================
import sys

if(len(sys.argv) != 4):
    print("Error in command line parameters!")
    print("Usage: bin_to_array.py <bin file name> <output hex file name> <BIG/LITTLE>")
    sys.exit(1)

bin_file = open(str(sys.argv[1]), "rb")
hex_file = open(str(sys.argv[2]), "w")

word = bin_file.read(4)

while word != "":
    hex_file.write("0x")
    if(str(sys.argv[3]).lower() == "little"):
        for byte in range(3, -1, -1):
            hex_file.write(hex(ord(word[byte]))[2:].zfill(2))

        hex_file.write("\n")
        word = bin_file.read(4)

    elif(str(sys.argv[3]).lower() == "big"):
        for byte in range(0, 4):
            hex_file.write(hex(ord(word[byte]))[2:].zfill(2))

        hex_file.write("\n")
        word = bin_file.read(4)

hex_file.close()
