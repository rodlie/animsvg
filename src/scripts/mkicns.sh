#!/bin/sh

set -e -x

ICON=`pwd`/src/app/icons/enve2d.png
ICONSET=`pwd`/enve2d.iconset

if [ -d "$ICONSET" ]; then
  rm -rf $ICONSET
fi
mkdir -p $ICONSET

sips -z 16 16     $ICON --out $ICONSET/icon_16x16.png
sips -z 32 32     $ICON --out $ICONSET/icon_16x16@2x.png
sips -z 32 32     $ICON --out $ICONSET/icon_32x32.png
sips -z 64 64     $ICON --out $ICONSET/icon_32x32@2x.png
sips -z 128 128   $ICON --out $ICONSET/icon_128x128.png
sips -z 256 256   $ICON --out $ICONSET/icon_128x128@2x.png
sips -z 256 256   $ICON --out $ICONSET/icon_256x256.png
sips -z 512 512   $ICON --out $ICONSET/icon_256x256@2x.png
sips -z 512 512   $ICON --out $ICONSET/icon_512x512.png
cp $ICON $ICONSET/icon_512x512@2x.png
iconutil -c icns $ICONSET

rm -rf $ICONSET
