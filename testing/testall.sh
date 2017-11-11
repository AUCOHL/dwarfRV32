for i in ./tests/*.c ./tests/*.s; do
    [ -f "$i" ] || break
    file=${i##*/}
    echo Testing: $file
    ./test.sh $file
done
