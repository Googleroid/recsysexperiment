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
 * This file split the ratings to test set and train set, this file change the random function!
 *
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <stdlib.h>
#define  SOURCE "../yahooMovie/yahooMovie.data"
#define  SEPARATOR " "
#define USER_NUM 4652
#define ITEM_NUM 4244
using namespace std;
float get_rand();
//Rating sequence: MovieId, UserId, StoryRating, ActingRating, DirectionRating, VisualRating, OverallRating
struct rateNode
{
	short item;
	short story;
	short acting;
	short direction;
	short visual;
	short overall;
};

int main()
{
    //(1)load the data in a rateMatrix 
    //(2)random select the test rate for every
    //(3)save the rate in different files.
    ifstream in(SOURCE);
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);           //使用一个vector数组存储稀疏的打分矩阵
    char rateStr[200];
    set<int> myset;
    int itemId = 0, userId = 0, story = 0, acting = 0, direction = 0, visual = 0, overall = 0;
    char* pch;
    int i = 0;
    //load all the ratings to the rateMatrix
    while(in.getline(rateStr,200)) {
        string strTemp(rateStr);
    	if(strTemp.length() < 3)continue;
    	//cout<<rateStr<<endl;
    	i = 0;
    	pch = strtok (rateStr,SEPARATOR);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) userId = atoi(pch);
            else if(2 == i) story = atoi(pch);
            else if(3 == i) acting = atoi(pch);
            else if(4 == i) direction = atoi(pch);
            else if(5 == i) visual = atoi(pch);
            else if(6 == i) overall = atoi(pch);
            else if(i > 6) break;
            ++i;
            pch = strtok (NULL,SEPARATOR);
        }
    	//cout<<userId<<'\t'<<itemId<<'\t'<<rate<<endl;exit(1);
    	if(0 == itemId || 0 == userId ) {
    		cout<<strTemp<<"####	"<<userId<<"	"<<itemId<<"	"<<"#####################"<<endl;
    		exit(1);
    	}		
    	try {
    		rateNode tmpNode;
    		tmpNode.item = (short)itemId;
    		tmpNode.story = (short)story;
    		tmpNode.acting = (short)acting;
    		tmpNode.direction = (short)direction;
    		tmpNode.visual = (short)visual;
    		tmpNode.overall = (short)overall;
    		rateMatrix[userId].push_back(tmpNode);
    	}
    	catch (bad_alloc& ba)
    	{
    		cerr << "bad_alloc caught: " << ba.what() << endl;
    	}
    }
    in.close();
    cout<<"read file successfully!"<<endl;
    
    //split the ratings
    ofstream training("./tmpData/training.txt");
    ofstream test("./tmpData/test.txt");
    srand((unsigned)time(0));
    for(int i = 1; i < USER_NUM+1; ++i){
    	int vSize = rateMatrix[i].size();
		for(int j=0; j < vSize; ++j) {
		    float randTemp = get_rand();
			if(randTemp < 0.3) { // in test set
				test <<rateMatrix[i][j].item<<" "<<i<<" "<<rateMatrix[i][j].story<<" ";
				test <<rateMatrix[i][j].acting<<" "<<rateMatrix[i][j].direction<<" ";
				test <<rateMatrix[i][j].visual<<" "<<rateMatrix[i][j].overall<<endl;
			}
			else {
				training <<rateMatrix[i][j].item<<" "<<i<<" "<<rateMatrix[i][j].story<<" ";
				training <<rateMatrix[i][j].acting<<" "<<rateMatrix[i][j].direction<<" ";
				training <<rateMatrix[i][j].visual<<" "<<rateMatrix[i][j].overall<<endl;
			}
		}
		if(i % 3000 == 0)cout << "user "<<i<< " deal successfully!"<<endl;
    }
    test.close();
    training.close();
}


/**
 *	return a random num between 0 and 1
 */
float get_rand()
{
    return rand()/(float)RAND_MAX;
}
