#!/bin/bash
#this file use to get one result of mGNbr model(the muliti-criteria global neighborhood model)
#just input the ratio and then the programme will split the data by ratio and then give the predict values of testSet.
#
ratio=$1
echo "The ratio of testSet is: "$ratio
cd /home/users/lvhongliang/recsysexperiment/yahoomovie
./splitRatings.sh ../yahooMovie/yahooMovie.data  $ratio #split the ratings
echo "split Ratings ok!"

#output the RMSE to file mavgmodel.ret
cd -
./mGNbrModel "mGNbrmodel.ret" $ratio
