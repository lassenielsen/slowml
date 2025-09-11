#!/bin/bash
# Create Snakes
./rlsnake_train -n '13->[[4*[all]]]' rlsnake_1_easy.mod 1 -i 0
./rlsnake_train -n '13->[[10*[all]],[4*[all]]]' rlsnake_1_medium.mod 1 -i 0
./rlsnake_train -n '13->[[20*[all]],[20*[all]],[20*[all]],[4*[all]]]' rlsnake_1_hard.mod 1 -i 0
./rlsnake_train -n '29->[[4*[all]]]' rlsnake_2_easy.mod 2 -i 0
./rlsnake_train -n '29->[[10*[all]],[4*[all]]]' rlsnake_2_medium.mod 2 -i 0
./rlsnake_train -n '29->[[20*[all]],[20*[all]],[20*[all]],[4*[all]]]' rlsnake_2_hard.mod 2 -i 0
./rlsnake_train -n '53->[[4*[all]]]' rlsnake_3_easy.mod 3 -i 0
./rlsnake_train -n '53->[[10*[all]],[4*[all]]]' rlsnake_3_medium.mod 3 -i 0
./rlsnake_train -n '53->[[20*[all]],[20*[all]],[20*[all]],[4*[all]]]' rlsnake_3_hard.mod 3 -i 0
./rlsnake_train -n '85->[[4*[all]]]' rlsnake_4_easy.mod 4 -i 0
./rlsnake_train -n '85->[[10*[all]],[4*[all]]]' rlsnake_4_medium.mod 4 -i 0
./rlsnake_train -n '85->[[20*[all]],[20*[all]],[20*[all]],[4*[all]]]' rlsnake_4_hard.mod 4 -i 0
./rlsnake_train -n '13->[[4*[all]]]' rlsnake_n8_easy.mod -8 -i 0
./rlsnake_train -n '13->[[10*[all]],[4*[all]]]' rlsnake_n8_medium.mod -8 -i 0
./rlsnake_train -n '13->[[20*[all]],[20*[all]],[20*[all]],[4*[all]]]' rlsnake_n8_hard.mod -8 -i 0
# Initial Training
for w in `seq 10 5 30`;
do for m in 'easy' 'medium' 'hard';
   do for x in `seq 5`;
      do echo Iteration $m-$w:$x;
         for d in `seq 1 4`;
         do cp rlsnake_"$d"_"$m".mod rlsnake_"$d"_"$w"x"$w"_"$x"_"$m".mod;
            ./rlsnake_train -i 1 -s 100 -r 500 -c rlsnake_"$d"_"$m".mod "$d" -w $w -h $w -rnd 0.02 -gdl 0.3;
         done;  
         cp rlsnake_n8_$m.mod rlsnake_n8_"$w"x"$w"_"$x"_$m.mod;
         ./rlsnake_train -w $w -h $w -i 1 -s 100 -r 500 -c rlsnake_n8_$m.mod -8 -rnd 0.02 -gdl 0.3;
      done;
  done;
done
# Competitive Training
for w in `seq 35 5 60`;
do for m1 in 'easy' 'medium' 'hard';
   do for m2 in `(echo easy; echo medium; echo hard;) | grep -v $m1`;
      do for x in `seq 5`;
         do echo Iteration "$m1"vs"$m2"-$w:$x;
            for d in `seq 1 4`;
            do cp rlsnake_"$d"_"$m1".mod rlsnake_"$d"_"$w"x"$w"_"$x"_"$m1"_1.mod;
               cp rlsnake_"$d"_"$m2".mod rlsnake_"$d"_"$w"x"$w"_"$x"_"$m2"_2.mod;
               ./rlsnake_train -i 1 -s 100 -r 500 -c rlsnake_"$d"_"$m1".mod "$d" -c rlsnake_"$d"_"$m2".mod "$d" -w $w -h $w -rnd 0.02 -gdl 0.3;
            done;  
            cp rlsnake_n8_$m1.mod rlsnake_n8_"$w"x"$w"_"$x"_"$m1"_1.mod;
            cp rlsnake_n8_$m2.mod rlsnake_n8_"$w"x"$w"_"$x"_"$m2"_2.mod;
            ./rlsnake_train -w $w -h $w -i 1 -s 100 -r 500 -c rlsnake_n8_$m1.mod -8 -c rlsnake_n8_$m2.mod -8 -rnd 0.02 -gdl 0.3;
         done;
      done;
  done;
done
mkdir -p models
mv *.mod models/
