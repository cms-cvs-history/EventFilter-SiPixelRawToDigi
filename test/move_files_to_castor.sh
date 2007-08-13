#!/bin/sh

echo "Quick run and copy script by Freya.Blekman@cern.ch. Usage: \"move_files_to_castor.sh filelist.txt /castor/cern.ch/user/a/acastorpath/bla\""
FILES=$1
CASTORDIR=$2

for afile in `less $FILES`
do
    echo "now looking at file: " $afile
    pathname=${afile%/*}
    filename=${afile##*/}
    echo "moving "$filename" to "$CASTORDIR
    rfcp $afile $CASTORDIR"/"$filename
done
