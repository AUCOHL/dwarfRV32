#!/bin/bash
#size = 0
for i in {1..100}
do
  c=$(( $RANDOM % 10 + 40))
  r=$(( $RANDOM % 26 + 5))
  ./randreg $c $r > "./tests/reg/reg$i.s"
  #echo "reg$i.s"
  ./test.sh "reg/reg$i.s"
  #actualsize=$(wc -c <"reg$1.diff")
  if [ -s "reg$1.diff" ]
  then
        echo $i failed!
  else
        echo $i passed!
  fi

done
#./clean.sh
