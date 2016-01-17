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
echo '#############'
echo 'copying files to ./out'
echo '#############'
echo ''
mkdir out
mkdir out/modules
cp arch/arm/boot/zImage out/zImage
# Find and copy modules
find ./drivers -name '*.ko' | xargs -I {} cp {} ./out/modules/
find ./net -name '*.ko' | xargs -I {} cp {} ./out/modules/
find ./crypto -name '*.ko' | xargs -I {} cp {} ./out/modules/

echo 'done'
echo ''
if [ -a arch/arm/boot/zImage ]; then
echo '#############'
echo 'Making boot.img'
echo '#############'
echo ''
cd working
. ./mkbootimg.sh
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
