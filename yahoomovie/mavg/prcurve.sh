#!/bin/bash

#this file is used for computer the precision and recall curve

#(1)get the user like set(8-13 denotes like, and 1-7 denotes dislike)
awk '
BEGIN {
    OFS="_";
}
{
    if ($3 >= 10) {
        print $1,$2;
    }
}' predictRet.txt > likeSet.txt
likeSetNum=`cat likeSet.txt | wc -l`

#(2) get the Rec set
awk '
{
    print $1,$2,$4;
}' predictRet.txt | sort -k3nr | awk 'BEGIN{OFS="_"}{print $1,$2;}'> rec_seq.txt


#(3)get the common set of rec and like
for i in 1 100 1000 5000 7000 8000 10000 15000 18000 21862
do
    head -n$i rec_seq.txt > rec_seq.top
    ../getset.sh likeSet.txt rec_seq.top
    commonNum=`cat common | wc -l`
    #echo $commonNum;continue;
    precision=`awk 'BEGIN{printf "%.5f",('$commonNum'/'$i')}'`
    recall=`awk 'BEGIN{printf "%.5f",('$commonNum'/'$likeSetNum')}'`
    echo $precision" "$recall
done

