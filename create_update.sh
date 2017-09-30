#!/bin/bash

echo "update lib"

rm -f ./update_package/lib/libmarkWidget.so*
cp -a libmarkWidget.so* ./update_package/lib/

sync

shCreate update_package

version=`grep -m 1 版本号 ./update_package/share/versionInfo.txt | cut -dV -f2`

fileName="QT_CCD-v${version}_更新.sh"

fileDir="/home/u/Desktop/qt_ccd_update"

mkdir -p  "$fileDir"

cp -aT update_package.sh "$fileDir/$fileName"

echo "生成 $fileName 到 $fileDir"

sync

exit 0
