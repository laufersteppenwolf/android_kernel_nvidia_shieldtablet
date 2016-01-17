#!/bin/bash

./mkbootimg --kernel ../out/zImage --ramdisk boot.img-ramdisk.gz --base 0x10000000 --pagesize 2048 --output boot.img
