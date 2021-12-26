#!/bin/bash
export BNAME=`basename $1 .pcap`
echo "Splitting into flows"
./scripts/pcapflows.perl "$1" data/"$BNAME"_FLOW > /dev/null
for f in `ls data/"$BNAME"_FLOW*.pcap`;
do
  export FNAME=`basename $f .pcap`
  echo "Processing flow $f"
  tshark -r $f -q -z follow,tcp,ascii,0 | tail -n +7 | head -n -1 | ./scripts/joinfile.py > data/stream_"$BNAME"_"$FNAME".txt;
    rm $f;
done
echo "Done"
