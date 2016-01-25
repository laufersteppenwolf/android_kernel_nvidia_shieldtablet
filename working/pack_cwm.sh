#!/bin/bash
# CWM anykernel packing script
# By laufersteppenwolf

####### defines #######
zipname="Werewolf_v001_anykernel-$(date -u +%Y-%m-%d_%H%M).zip"
####### defines #######

echo "*****************************"
echo "CWM anykernel packing script"
echo "*****************************"
echo ""
echo "Looking for zImage"
if test -e ../out/zImage
   then echo "-----> zImage found"
echo ""
echo "Making CWM Anykernel zip"
mkdir out
cd out 

mkdir kernel
mkdir -p system/lib/modules

# Copy files needed to create the zip 
cp -r ../tools/META-INF META-INF
cp -r ../tools/kernel/* kernel
cp ../../out/zImage kernel/zImage
cp -r ../../out/modules/* system/lib/modules

cp ../tools/signapk.jar signapk.jar 
cp ../tools/testkey.x509.pem testkey.x509.pem
cp ../tools/testkey.pk8 testkey.pk8

# Zipping stuff up
zip -r temp_zip.zip META-INF kernel system 
echo "ZIP Ready, signing it"
java -jar signapk.jar -w testkey.x509.pem testkey.pk8 temp_zip.zip "$zipname"

cp -f "$zipname" ../"$zipname"

# Finishing up

cd ..
rm -rf out
mv "$zipname" ../out/"$zipname"
# All done
 
echo "Anykernel zip is $zipname"
echo ""
echo "All Done!"

else 
echo "No zImage found" 
fi
