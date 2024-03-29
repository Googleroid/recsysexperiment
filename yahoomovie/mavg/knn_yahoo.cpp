/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * The purpose of this program is to implement the in the SVD method of koren's SIGKDD'08 paper,
 * using the movielens dataset.
 *
 */
 
#include "../commonHeader.h"
#define USER_NUM 943 //10K:943 1M:6040
#define ITEM_NUM 1682 //10K:1682 1M:3900
#define CRI_NUM 5 //the number of criteria
#define MIN_RATE 1  //min of the rate
#define MAX_RATE 13 //max of the rate
#define K_NUM 100   // the dimension 
#include "./common.cpp"
#include "./yahooBase.cpp"
#define TRAINING_SET "../tmpData/training.txt" //training set
#define PROBE_SET "../tmpData/test.txt" //test set
#define RATE_SP " "  //rate Separator
#include "./knnBase.cpp"

int main(int argc, char ** argv)
{
    time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
    start = time(NULL);
    timeStartInfo = localtime(&start);
    string timeStartStr = asctime(timeStartInfo);
    knn::model("movielens", "movielens_kmax");
    end = time(NULL);
    duration = (end-start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<timeStartStr<<", and end at "<< asctime(timeEndInfo);
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
