#!/bin/bash
if [ "$1" == "BUDDY" ]; then
  MM="BUDDY_MEMMAN"
else
  MM="MEMMAN"
fi

echo "Building with memory manager: $MM"

make clean
cd Toolchain
make clean
make all
cd ..
make all MM=$MM