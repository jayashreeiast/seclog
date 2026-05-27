#!/bin/bash

#!/bin/bash

INPUT="./Output/RTD_ON_A53.bin"
OUTPUT="./Output/RTD_A53_IMAGE.bin"
ALIGNMENT=64

# Get the file size
SIZE=$(stat -c%s "$INPUT")

# Calculate padding
PAD=$(( (ALIGNMENT - (SIZE % ALIGNMENT)) % ALIGNMENT ))

# Create padding file
dd if=/dev/zero bs=1 count=$PAD of=pad.bin status=none

# Concatenate original file and padding
cat "$INPUT" pad.bin > "$OUTPUT"

# Clean up
rm pad.bin

echo "Padded file saved as $OUTPUT (added $PAD bytes)"
