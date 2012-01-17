/*
 * Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * Please contact me via email honglianglv@gmail.com
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 *
 * It is free software; you can redistribute it and/or modify it under GPLV3.
 *
 * This file gives some define of the yahoo movie dataset
 *
 */
#define USER_NUM 4652
#define ITEM_NUM 4244
#define CRI_NUM 5
#define K_NUM 1
#define TRAINING_SET "../tmpData/training.txt"
#define TEST_SET "../tmpData/test.txt"
#define RATE_SP " "
#include "./yahoo.h"
#include "../common.cpp"
#include "./yahooBase.cpp"
#include "./gNbrHimBase.cpp"

int main(int argc, char ** argv)
{
    time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
    start = time(NULL);
    timeStartInfo = localtime(&start);
    string timeStartStr = asctime(timeStartInfo);
    float alpha1 = 0.01;  //0.0045according to the paper of "a guide to SVD for CF"
    float alpha2 = 0.01;  //0.0045according to the paper of "a guide to SVD for CF"
    float beta1 = 0.5;   //0.015 according to the paper of "a guide to SVD for CF"
    float beta2 = 0.5;   //according my own experiment,beta=0.05 is very good ,the RMSE of movielens(1M) test 
    //can reach 0.868692  at step=44
    
    //for(int i=0; i < 10; i++)
    {
        //alpha = i*0.0005 + 0.0025;
        //cout << alpha << endl;
        //void model(int dim, float  alpha1, float alpha2, float beta1, float beta2,
        // int maxStep=60,double slowRate=1,bool isUpdateBias=true)
        gNbrHim::model(K_NUM,alpha1,alpha2,beta1,beta2,160,0.91,true);
    }
    end = time(NULL);
    duration = (end-start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<timeStartStr<<", and end at "<< asctime(timeEndInfo);
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}

