#!/bin/sh
###

NBEG=0
NEND=9

NSIZE=${1:-10000}

for i in $(seq $NBEG $NEND)
do
    NEXTDIR="ListV$i"
    if [ -d $NEXTDIR ]
    then
        cd $NEXTDIR
        echo "$NEXTDIR:"
        time ./TestList-gnu $NSIZE $NSIZE
        time ./TestList-clang $NSIZE $NSIZE
        echo
        cd ..
    fi
done

