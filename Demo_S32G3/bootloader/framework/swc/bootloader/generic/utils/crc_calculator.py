# =================================================================================================
#
#    @file        crc_calculator.py
#
#    @brief       CRC calculator sample for NXP automotive communication peers
#                 Calculates the CRC of a binary image using the following CRC32 algorithm:
#                           CRC 32
#                           Initial value = 0xFFFFFFFF
#
#    @input       File location on disk of a binary image
#                 Alignment bytes for the binary image file
#    @output      Aligned binary image size in hexadecimal format
#                 CRC calculated value
#
# =================================================================================================
# @copyright (c)  Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
#                 All Rights Reserved.
#
#                 This file contains sample code only.
#                 It is not part of the production code deliverables.
# =================================================================================================
import argparse
from crccheck.crc import Crc32Mpeg2


def main():
    """
    Usage:
    python crc_calculator.py -i <binary_file_path>
    """
    parser = argparse.ArgumentParser(
                            usage='%(prog)s [alignment_size] -i [binary_file_path]',
                            description='Calculates the CRC of a binary image using the CRC32 algorithm')
    parser.add_argument('-i', dest='binary_file', required=True,
                        help='Path to the binary file whose CRC is to be calculated')
    parser.add_argument('alignment', metavar='[alignment_size]', nargs=1, type=int,
                        choices=[0x4, 0x8, 0x10, 0x20, 0x40], default=0x8,
                        help='Alignment for the binary image file, default is 8 byte')
    args = parser.parse_args()

    crc_instance = Crc32Mpeg2()

    with open(args.binary_file, 'rb') as f:
        buf = bytearray(f.read())

    print("Binary image file size : " + str(hex(len(buf))))

    while len(buf) % args.alignment[0]:
        buf.append(0xff)

    crc_instance.process(buf)

    print("CRC value over binary image %i bytes aligned : " % int(args.alignment[0]), str(hex(crc_instance.final())))


if __name__ == '__main__':
    main()
