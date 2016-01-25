#!/bin/bash

###### defines ######

local_dir=$PWD

###### defines ######
echo '#############'
echo 'making clean'
echo '#############'
make clean
rm -rf out
echo '#############'
echo 'making defconfig'
echo '#############'
make cyanogenmod_shieldtablet_defconfig
echo '#############'
echo 'making zImage'
echo '#############'
time make -j12
if [ -a arch/arm/boot/zImage ]; then
echo '#############'
echo 'copying files to ./out'
echo '#############'
echo ''
mkdir out
mkdir out/modules
cp arch/arm/boot/zImage out/zImage
cp arch/arm/boot/dts/tegra124-tn8-p1761-1270-a04-e-battery.dtb out/tegra124-tn8-p1761-1270-a04-e-battery.dtb
# Find and copy modules
find ./drivers -name '*.ko' | xargs -I {} cp {} ./out/modules/
find ./net -name '*.ko' | xargs -I {} cp {} ./out/modules/
find ./crypto -name '*.ko' | xargs -I {} cp {} ./out/modules/
echo 'done'
echo ''
echo '#############'
echo 'Making boot.img'
echo '#############'
echo ''
cd working
. ./mkbootimg.sh
if [[ $1 = -a ]]; then
	echo ''
	echo '#############'
	echo 'Making anykernel zip'
	echo '#############'
	echo ''
	. ./pack_cwm.sh
fi
cd $local_dir
mv ./working/boot.img ./out/boot.img
echo ''
echo '#############'
echo 'build finished successfully'
echo '#############'
else
echo '#############'
echo 'build failed!'
echo '#############'
fi
