#!/bin/bash

for x in `ls data/pcap/*.pcap`;
do
  echo "Sessionizing $x";
  ./scripts/sessionize.sh $x;
done;

echo "Cleaning empty sessions"
for x in `find ./data/ -iname \*.txt -size 0`; do rm $x; done;

for x in `ls data/*.txt`;
do
  echo "Extracting featurevector from $x";
  ./extract $x "$(basename "$x" .txt).vec";
done;

for x in `ls *.vec`;
do
  if [ "${RANDOM:0:2}" -gt "12" ];
  then # training set
    export dest=trainingset;
  else # test set
    export dest=testset;
  fi;
  export type=`echo $x | sed 's/^stream_//g' | sed 's/_.*$//g'`
  mkdir -p $dest/label_$type;
  echo mv $x $dest/label_$type/;
  mv $x $dest/label_$type/;
done;
