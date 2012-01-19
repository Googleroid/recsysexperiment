#!/bin/bash
if [ $# -ne 2 ];
then
	echo "Usage: $0 filename ratio";
	exit 1
fi

filename=$1
#total_num=`cat $filename | wc -l`
#ratio=`awk 'BEGIN{printf "%.6f",('$select_num'/'$total_num')}'`
ratio=$2

#del the training set and test set
rm tmpData/training.txt
rm tmpData/test.txt

awk -v threshold="$ratio" ' \
BEGIN{
	srand();
}
{
	if( rand() < threshold ) {
		print $0 >> "tmpData/test.txt";
	}
    else {
        print $0 >> "tmpData/training.txt";
    }
}' $filename
