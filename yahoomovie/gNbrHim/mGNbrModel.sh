#!/bin/bash
#this file use to get a serial result of mGNbr model(the muliti-criteria global neighborhood model)
#1.split the file
ratio=0.0
count=0
while :;  do
    #ratio=$((ratio+0.1))
    ratio=$(echo "scale=3; $ratio+0.1" | bc)
    ((++count))
    ((count==10))&&break
    ratio=0$ratio
    #echo $ratio&&continue
    #ratio=0.9
    echo "The ratio of testSet is: "$ratio
    cd /home/users/lvhongliang/recsysexperiment/yahoomovie
    ./splitRatings.sh ../yahooMovie/yahooMovie.data  $ratio #split the ratings
    echo "split Ratings ok!"

    #output the RMSE to file mavgmodel.ret
    cd /home/users/lvhongliang/recsyscode/knn
    ./mGNbrModel "mGNbrmodel.ret" $ratio
done
