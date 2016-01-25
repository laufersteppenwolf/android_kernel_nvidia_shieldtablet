#!/sbin/sh
cmdline=$(cat /tmp/boot.img-cmdline)
base=0x$(cat /tmp/boot.img-base)
echo $base
echo $cmdline
/tmp/mkbootimg --kernel /tmp/zImage --ramdisk /tmp/boot.img-ramdisk.gz --pagesize 2048 --base $base --cmdline "$cmdline" -o /tmp/newboot.img
