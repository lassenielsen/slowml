#!/bin/bash

#export bgfile=`ls backgrounds/ | grep \.vec | shuf | head -n 1`
#export objfile=`find ocr/train/ | grep \.vec | shuf | head -n 1`

for objfile in `find ocr/train/ | grep \.vec | shuf | head -n 2000`;
do export bgfile=`find backgrounds/ | grep \.vec | shuf | head -n 1`
   #for bgfile in `find backgrounds/ | grep \.vec`; do
   export posx=`echo "print(0.95*float($RANDOM)/32767)" | python`
   export posy=`echo "print(0.95*float($RANDOM)/32767)" | python`
   export sizex=`echo "print(0.05+(0.95-$posx)*float($RANDOM)/32767)" | python`
   export sizey=`echo "print(0.05+(0.95-$posy)*float($RANDOM)/32767)" | python`
   export label=`echo $objfile | sed 's/^ocr.train.//g' | sed 's/\/.*$//g'`
   mkdir -p detectocr/train/$label
   ./revocr.py $bgfile 100 $objfile 28 $posx $posy $sizex $sizey detectocr/train/$label/img_100_"$posx"_"$posy"_"$sizex"_"$sizey".vec
   #done;
done;
