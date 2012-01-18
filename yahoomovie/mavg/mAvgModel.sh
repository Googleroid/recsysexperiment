#!/bin/bash

#this file use to get a serial result of mAvg model(the average pearson similariy)
#1.split the file


ratio=0.0
count=0
while :;  do
    #ratio=$((ratio+0.1))
    ratio=$(echo "scale=3; $ratio+0.1" | bc)
    ((++count))
    ((count==10))&&break
    ratio=0$ratio
    echo "The ratio is: "$ratio
    cd /home/users/lvhongliang/recsysexperiment/yahoomovie
    ./splitRatings $ratio #split the ratings
    echo "split Ratings ok!"

    #get the similarity of every single criteria
    #generate the files: 1.sim, 2.sim, 3.sim, 4.sim, 5.sim
    cd /home/users/lvhongliang/recsysexperiment/yahoomovie/mavg
    for i in `seq 5`;do
        echo "compute the similarity of "$i":"
        ./getSim $i
    done

    #get average of all the sims
    ./getAvgSim.sh #output the avg.sim
    echo "get the average of all the sim ok!"

    #get the k-max of each item
    getkmaxth "avg.sim" #output avg.sim_kmax
    echo "get the k-maxth sim of each item ok!"

    #copy the avg.sim and avg.sim_kmax to /home/users/lvhongliang/recsyscode/knn
    cp avg.sim /home/users/lvhongliang/recsyscode/knn/movielens
    cp avg.sim /home/users/lvhongliang/recsyscode/knn/movielens_kmax

    #prepare the traing set and test set for knn model
    ./transferDataFormat.sh "../tmpData/training.txt" > /home/users/lvhongliang/recsyscode/dataset/movielens/u1.base
    ./transferDataFormat.sh "../tmpData/test.txt" > "/home/users/lvhongliang/recsyscode/dataset/movielens/u1.test"
    echo "cp the sim files and prepare the training set and test set ok!"

    #output the RMSE to file mavgmodel.ret
    cd /home/users/lvhongliang/recsyscode/knn
    ./knn mavgmodel.ret 
    exit(0)
done
