#!/bin/sh
###

NBEG=0
NEND=9

for i in $(seq $NBEG $NEND)
do
    NEXTDIR="ListV$i"
    if [ -d $NEXTDIR ]
    then
        cd $NEXTDIR
        echo "$NEXTDIR:"
        g++ -O3 -std=c++14 -pedantic-errors -o TestList-gnu TestList.cpp
        clang++ -O3 -std=c++14 -stdlib=libc++ -pedantic-errors -o TestList-clang TestList.cpp
        echo
        cd ..
    fi
done

