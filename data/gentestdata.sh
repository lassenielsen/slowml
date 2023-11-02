#!/bin/bash

for objfile in `find ocr/train/ | grep \.vec | shuf | head -n 20000`;
do export bgfile=`find backgrounds/ | grep \.vec | shuf | head -n 1`
   export posx=`echo "print(0.80*float($RANDOM)/32767)" | python3`
   export posy=`echo "print(0.80*float($RANDOM)/32767)" | python3`
   export sizex=`echo "print(0.20+(0.80-$posx)*float($RANDOM)/32767)" | python3`
   export sizey=`echo "print(0.20+(0.80-$posy)*float($RANDOM)/32767)" | python3`
   export size=`(echo $sizex; echo $sizey;)| sort | head -n 1`;
   export label=`echo $objfile | sed 's/^ocr.train.//g' | sed 's/\/.*$//g'`
   mkdir -p detectocr/train/$label
   #slowml_imgvec.py
   ./revocr.py $bgfile 100 $objfile 28 $posx $posy $size $size detectocr/train/$label/img_100_"$posx"_"$posy"_"$size"_"$size".vec
done;

for objfile in `find ocr/test/ | grep \.vec | shuf | head -n 20000`;
do export bgfile=`find backgrounds/ | grep \.vec | shuf | head -n 1`
   export posx=`echo "print(0.85*float($RANDOM)/32767)" | python3`
   export posy=`echo "print(0.85*float($RANDOM)/32767)" | python3`
   export sizex=`echo "print(0.15+(0.85-$posx)*float($RANDOM)/32767)" | python3`
   export sizey=`echo "print(0.15+(0.85-$posy)*float($RANDOM)/32767)" | python3`
   export size=`(echo $sizex; echo $sizey;)| sort | head -n 1`;
   export label=`echo $objfile | sed 's/^ocr.test.//g' | sed 's/\/.*$//g'`
   mkdir -p detectocr/test/$label
   ./revocr.py $bgfile 100 $objfile 28 $posx $posy $size $size detectocr/test/$label/img_100_"$posx"_"$posy"_"$size"_"$size".vec
done;
